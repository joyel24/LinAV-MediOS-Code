#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 regs[8];

static uint32 prg0,prg1;
static uint32 chr01,chr23,chr4,chr5,chr6,chr7;

static uint32 map119_chr_swap() { return regs[0] & 0x80; }
static uint32 map119_prg_swap() { return regs[0] & 0x40; }

static uint8 irq_enabled; // IRQs enabled
static uint8 irq_counter; // IRQ scanline counter, decreasing
static uint8 irq_latch;   // IRQ scanline counter latch
/////////////////////////////////////////////////////////////////////
// Mapper 119

static void map119_MMC3_set_CPU_banks()
{
	if(map119_prg_swap())
	{
		mmc_bankromALL4(num_8k_ROM_banks-2,prg1,prg0,num_8k_ROM_banks-1);
	}
	else
	{
		mmc_bankromALL4(prg0,prg1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
	}
}

static void map119_MMC3_set_PPU_banks()
{
	uint8 chr_bank[8];

	if(map119_chr_swap())
	{
		chr_bank[0] = chr4;
		chr_bank[1] = chr5;
		chr_bank[2] = chr6;
		chr_bank[3] = chr7;
		chr_bank[4] = chr01+0;
		chr_bank[5] = chr01+1;
		chr_bank[6] = chr23+0;
		chr_bank[7] = chr23+1;
	}
	else
	{
		chr_bank[0] = chr01+0;
		chr_bank[1] = chr01+1;
		chr_bank[2] = chr23+0;
		chr_bank[3] = chr23+1;
		chr_bank[4] = chr4;
		chr_bank[5] = chr5;
		chr_bank[6] = chr6;
		chr_bank[7] = chr7;
	}

	if(chr_bank[0] & 0x40)
		mmc_VRAM_bank(0, chr_bank[0] & 0x07);
	else
		mmc_bankvrom0(chr_bank[0]);

	if(chr_bank[1] & 0x40)
		mmc_VRAM_bank(1, chr_bank[1] & 0x07);
	else
		mmc_bankvrom1(chr_bank[1]);

	if(chr_bank[2] & 0x40)
		mmc_VRAM_bank(2, chr_bank[2] & 0x07);
	else
		mmc_bankvrom2(chr_bank[2]);

	if(chr_bank[3] & 0x40)
		mmc_VRAM_bank(3, chr_bank[3] & 0x07);
	else
		mmc_bankvrom3(chr_bank[3]);

	if(chr_bank[4] & 0x40)
		mmc_VRAM_bank(4, chr_bank[4] & 0x07);
	else
		mmc_bankvrom4(chr_bank[4]);

	if(chr_bank[5] & 0x40)
		mmc_VRAM_bank(5, chr_bank[5] & 0x07);
	else
		mmc_bankvrom5(chr_bank[5]);

	if(chr_bank[6] & 0x40)
		mmc_VRAM_bank(6, chr_bank[6] & 0x07);
	else
		mmc_bankvrom6(chr_bank[6]);

	if(chr_bank[7] & 0x40)
		mmc_VRAM_bank(7, chr_bank[7] & 0x07);
	else
		mmc_bankvrom7(chr_bank[7]);
}

static void map119_Reset()
{
	int i;
	// clear registers FIRST!!!
	for(i = 0; i < 8; i++) regs[i] = 0x00;

	// set CPU bank pointers
	prg0 = 0;
	prg1 = 1;
	map119_MMC3_set_CPU_banks();

	// set VROM banks
	chr01 = 0;
	chr23 = 2;
	chr4  = 4;
	chr5  = 5;
	chr6  = 6;
	chr7  = 7;
	map119_MMC3_set_PPU_banks();

	irq_enabled = 0;
	irq_counter = 0;
	irq_latch = 0;

	//parent_NES->ppu->vram_write_protect = 0;
	
}

static void map119_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr & 0xE001)
	{
	case 0x8000:
		{
			regs[0] = data;
			map119_MMC3_set_PPU_banks();
			map119_MMC3_set_CPU_banks();
		}
		break;

	case 0x8001:
		{
			uint32 bank_num;

			regs[1] = data;
			bank_num = regs[1];

			switch(regs[0] & 0x07)
			{
			case 0x00:
				{
					if(num_1k_VROM_banks)
					{
						bank_num &= 0xfe;
						chr01 = bank_num;
						map119_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x01:
				{
					if(num_1k_VROM_banks)
					{
						bank_num &= 0xfe;
						chr23 = bank_num;
						map119_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x02:
				{
					if(num_1k_VROM_banks)
					{
						chr4 = bank_num;
						map119_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x03:
				{
					if(num_1k_VROM_banks)
					{
						chr5 = bank_num;
						map119_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x04:
				{
					if(num_1k_VROM_banks)
					{
						chr6 = bank_num;
						map119_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x05:
				{
					if(num_1k_VROM_banks)
					{
						chr7 = bank_num;
						map119_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x06:
				{
					prg0 = bank_num;
					map119_MMC3_set_CPU_banks();
				}
				break;

			case 0x07:
				{
					prg1 = bank_num;
					map119_MMC3_set_CPU_banks();
				}
				break;
			}
		}
		break;

	case 0xA000:
		{
			regs[2] = data;

			//if(parent_NES->ROM->get_mirroring() != NES_PPU::MIRROR_FOUR_SCREEN)
			if (mmc_getinfo()->var.mirroring != 5)
			{
				if(data & 0x01)
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
		}
		break;

	case 0xA001:
		{
			regs[3] = data;
		}
		break;

	case 0xC000:
		regs[4] = data;
		irq_counter = regs[4];
		break;

	case 0xC001:
		regs[5] = data;
		irq_latch = regs[5];
		break;

	case 0xE000:
		regs[6] = data;
		irq_enabled = 0;
		break;

	case 0xE001:
		regs[7] = data;
		irq_enabled = 1;
		break;
	}
}

static void map119_HSync(uint32 scanline)
{
	if(irq_enabled)
	{
		if((scanline >= 0) && (scanline <= 239))
		{
			//if(parent_NES->ppu->spr_enabled() || parent_NES->ppu->bg_enabled())
			if (ppu_enabled())
			{
				if(!(irq_counter--))
				{
					irq_counter = irq_latch;
					//parent_NES->cpu->DoIRQ();
					doIRQ();
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////

static void map119_getstate(SnssMapperBlock *state)
{
	
}


static void map119_setstate(SnssMapperBlock *state)
{
	
}


mapintf_t map119_intf =
{
   119, /* mapper number */
   "Map119", /* mapper name */
   map119_Reset, /* init routine */
   NULL, /* vblank callback */
   map119_HSync, /* hblank callback */
   map119_getstate, /* get state (snss) */
   map119_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map119_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

