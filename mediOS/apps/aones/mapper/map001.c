#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint32 write_count;
static uint8  bits;
static uint8  regs[4];
static uint32 last_write_addr;



#define MMC1_SMALL 0
#define MMC1_512K  1
#define MMC1_1024K 2

static uint8 MMC1_Size;
static uint32 MMC1_256K_base;
static uint32 MMC1_swap;

	// these are the 4 ROM banks currently selected
static uint32 MMC1_bank1;
static uint32 MMC1_bank2;
static uint32 MMC1_bank3;
static uint32 MMC1_bank4;

static uint32 MMC1_HI1;
static uint32 MMC1_HI2;
/////////////////////////////////////////////////////////////////////
// Mapper 1
static void map1_MMC1_set_CPU_banks()
{
	mmc_bankromALL4((MMC1_256K_base << 5) + (MMC1_bank1 & ((256/8)-1)),
	              (MMC1_256K_base << 5) + (MMC1_bank2 & ((256/8)-1)),
	              (MMC1_256K_base << 5) + (MMC1_bank3 & ((256/8)-1)),
	              (MMC1_256K_base << 5) + (MMC1_bank4 & ((256/8)-1)));
}

static void map1_Reset(void)
{		
	if(mmc_getinfo()->var.crc == 0x9b565541 || // Triathron, The
	        mmc_getinfo()->var.crc == 0x95e4e594)   // Qix
	{
		mmc_getinfo()->var.frame_irq_disenabled = 1;
	}

	write_count = 0;
	bits = 0x00;
	regs[0] = 0x0C; // reflects initial ROM state
	regs[1] = 0x00;
	regs[2] = 0x00;
	regs[3] = 0x00;
	{
		uint32 size_in_K = num_8k_ROM_banks * 8;

		if(size_in_K == 1024)
		{
			MMC1_Size = MMC1_1024K;
		}
		else if(size_in_K == 512)
		{
			MMC1_Size = MMC1_512K;
		}
		else
		{
			MMC1_Size = MMC1_SMALL;
		}
	}
	MMC1_256K_base = 0; // use first 256K
	MMC1_swap = 0;

	if(MMC1_Size == MMC1_SMALL)
	{
		// set two high pages to last two banks
		MMC1_HI1 = num_8k_ROM_banks-2;
		MMC1_HI2 = num_8k_ROM_banks-1;
	}
	else
	{
		// set two high pages to last two banks of current 256K region
		MMC1_HI1 = (256/8)-2;
		MMC1_HI2 = (256/8)-1;
	}

	// set CPU bank pointers
	MMC1_bank1 = 0;
	MMC1_bank2 = 1;
	MMC1_bank3 = MMC1_HI1;
	MMC1_bank4 = MMC1_HI2;

	map1_MMC1_set_CPU_banks();
}


static void map1_MemoryWrite(uint32 addr, uint8 data)
{
	uint32 reg_num;

	// if write is to a different reg, reset
	if((addr & 0x6000) != (last_write_addr & 0x6000))
	{
		write_count = 0;
		bits = 0x00;
	}
	last_write_addr = addr;

	// if bit 7 set, reset and return
	if(data & 0x80)
	{
		write_count = 0;
		bits = 0x00;
		return;
	}

	if(data & 0x01) bits |= (1 << write_count);
	write_count++;
	if(write_count < 5) return;

	reg_num = (addr & 0x7FFF) >> 13;
	regs[reg_num] = bits;

	write_count = 0;
	bits = 0x00;

	//  LOG("MAP1 REG" << reg_num << ": " << HEX(regs[reg_num],2) << endl);

	switch(reg_num)
	{
	case 0:
		{
			//        LOG("REG0: " << HEX(regs[0],2) << endl);

			// set mirroring
			if(regs[0] & 0x02)
			{
				if(regs[0] & 0x01)
				{
					//set_mirroring(NES_PPU::MIRROR_HORIZ);
					ppu_mirror(0,0,1,1);
				}
				else
				{
					//set_mirroring(NES_PPU::MIRROR_VERT);
					ppu_mirror(0,1,0,1);
				}
			}
			else
			{
				// one-screen mirroring
				if(regs[0] & 0x01)
				{
					//set_mirroring(1,1,1,1);
					ppu_mirror(1,1,1,1);
				}
				else
				{
					ppu_mirror(0,0,0,0);
				}
			}
		}
		break;

	case 1:
		{
			uint8 bank_num = regs[1];

			//        LOG("REG1: " << HEX(regs[1],2) <<"REG2: " << HEX(regs[2],2) << endl);

			if(MMC1_Size == MMC1_1024K)
			{
				if(regs[0] & 0x10)
				{
					if(MMC1_swap)
					{
						MMC1_256K_base = (regs[1] & 0x10) >> 4;
						if(regs[0] & 0x08)
						{
							MMC1_256K_base |= ((regs[2] & 0x10) >> 3);
						}
						map1_MMC1_set_CPU_banks();
						MMC1_swap = 0;
					}
					else
					{
						MMC1_swap = 1;
					}
				}
				else
				{
					// use 1st or 4th 256K banks
					MMC1_256K_base = (regs[1] & 0x10) ? 3 : 0;
					map1_MMC1_set_CPU_banks();
				}
			}
			else if((MMC1_Size == MMC1_512K) && (!num_1k_VROM_banks))
			{
				MMC1_256K_base = (regs[1] & 0x10) >> 4;
				map1_MMC1_set_CPU_banks();
			}
			else if(num_1k_VROM_banks)
			{
				// set VROM bank at $0000
				if(regs[0] & 0x10)
				{
					// swap 4K
					//bank_num <<= 2;
					mmc_bankvrom(4,0x0000,bank_num);
				}
				else
				{
					// swap 8K
					//bank_num <<= 2;					
					mmc_bankvrom(4,0x0000,bank_num);
					mmc_bankvrom(4,0x1000,bank_num+1);
				}
			}
			else
			{
				if(regs[0] & 0x10)
				{
					bank_num <<= 2;
					mmc_VRAM_bank(0, bank_num+0);
					mmc_VRAM_bank(1, bank_num+1);
					mmc_VRAM_bank(2, bank_num+2);
					mmc_VRAM_bank(3, bank_num+3);
				}
			}
		}
		break;

	case 2:
		{
			uint8 bank_num = regs[2];

			//        LOG("REG2: " << HEX(regs[2],2) << endl);

			if((MMC1_Size == MMC1_1024K) && (regs[0] & 0x08))
			{
				if(MMC1_swap)
				{
					MMC1_256K_base =  (regs[1] & 0x10) >> 4;
					MMC1_256K_base |= ((regs[2] & 0x10) >> 3);
					map1_MMC1_set_CPU_banks();
					MMC1_swap = 0;
				}
				else
				{
					MMC1_swap = 1;
				}
			}

			if(!num_1k_VROM_banks)
			{
				if(regs[0] & 0x10)
				{
					bank_num <<= 2;
					mmc_VRAM_bank(4, bank_num+0);
					mmc_VRAM_bank(5, bank_num+1);
					mmc_VRAM_bank(6, bank_num+2);
					mmc_VRAM_bank(7, bank_num+3);
					break;
				}
			}

			// set 4K VROM bank at $1000
			if(regs[0] & 0x10)
			{
				// swap 4K
				//bank_num <<= 2;
				mmc_bankvrom(4,0x1000,bank_num);
			}
		}
		break;

	case 3:
		{
			uint8 bank_num = regs[3];

			//        LOG("REG3: " << HEX(regs[3],2) << endl);

			// set ROM bank
			if(regs[0] & 0x08)
			{
				// 16K of ROM
				bank_num <<= 1;

				if(regs[0] & 0x04)
				{
					// 16K of ROM at $8000
					MMC1_bank1 = bank_num;
					MMC1_bank2 = bank_num+1;
					MMC1_bank3 = MMC1_HI1;
					MMC1_bank4 = MMC1_HI2;
				}
				else
				{
					// 16K of ROM at $C000
					if(MMC1_Size == MMC1_SMALL)
					{
						MMC1_bank1 = 0;
						MMC1_bank2 = 1;
						MMC1_bank3 = bank_num;
						MMC1_bank4 = bank_num+1;
					}
				}
			}
			else
			{
				// 32K of ROM at $8000
				bank_num <<= 1; // thanx rinao

				MMC1_bank1 = bank_num;
				MMC1_bank2 = bank_num+1;
				if(MMC1_Size == MMC1_SMALL)
				{
					MMC1_bank3 = bank_num+2;
					MMC1_bank4 = bank_num+3;
				}
			}

			map1_MMC1_set_CPU_banks();
		}
		break;
	}
}
/////////////////////////////////////////////////////////////////////

static void map1_getstate(SnssMapperBlock *state)
{
   state->extraData.mapper1.registers[0] = regs[0];
   state->extraData.mapper1.registers[1] = regs[1];
   state->extraData.mapper1.registers[2] = regs[2];
   state->extraData.mapper1.registers[3] = regs[3];
   state->extraData.mapper1.latch = bits; //latch;
   state->extraData.mapper1.numberOfBits = write_count; //bitcount;
}


static void map1_setstate(SnssMapperBlock *state)
{
   regs[0] = state->extraData.mapper1.registers[0];
   regs[1] = state->extraData.mapper1.registers[1];
   regs[2] = state->extraData.mapper1.registers[2];
   regs[3] = state->extraData.mapper1.registers[3];
   bits = state->extraData.mapper1.latch;
   write_count = state->extraData.mapper1.numberOfBits; 
}


mapintf_t map1_intf =
{
   1, /* mapper number */
   "MMC1", /* mapper name */
   map1_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map1_getstate, /* get state (snss) */
   map1_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map1_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

