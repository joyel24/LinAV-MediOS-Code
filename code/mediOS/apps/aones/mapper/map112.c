#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8  regs[8];
static uint32 prg0,prg1;
static uint32 chr01,chr23,chr4,chr5,chr6,chr7;
static uint32 map112_chr_swap() { return regs[0] & 0x80; }
static uint32 map112_prg_swap() { return regs[0] & 0x40; }
static uint8 irq_enabled; // IRQs enabled
static uint8 irq_counter; // IRQ scanline counter, decreasing
static uint8 irq_latch;   // IRQ scanline counter latch

static void map112_MMC3_set_CPU_banks()
{
	if(map112_prg_swap())
	{
		mmc_bankromALL4(num_8k_ROM_banks-2,prg1,prg0,num_8k_ROM_banks-1);
	}
	else
	{
		mmc_bankromALL4(prg0,prg1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
	}
}
static void map112_MMC3_set_PPU_banks()
{
	if(num_1k_VROM_banks)
	{
		if(map112_chr_swap())
		{
			mmc_bankvromALL8(chr4,chr5,chr6,chr7,chr01,chr01+1,chr23,chr23+1);
		}
		else
		{
			mmc_bankvromALL8(chr01,chr01+1,chr23,chr23+1,chr4,chr5,chr6,chr7);
		}
	}
}
/////////////////////////////////////////////////////////////////////
// Mapper 112
static void map112_Reset()
{
	int i;
	// clear registers FIRST!!!
	for(i = 0; i < 8; i++) regs[i] = 0x00;

	// set CPU bank pointers
	prg0 = 0;
	prg1 = 1;
	map112_MMC3_set_CPU_banks();

	// set VROM banks
	if(num_1k_VROM_banks)
	{
		chr01 = 0;
		chr23 = 2;
		chr4  = 4;
		chr5  = 5;
		chr6  = 6;
		chr7  = 7;
		map112_MMC3_set_PPU_banks();
	}
	else
	{
		chr01 = chr23 = chr4 = chr5 = chr6 = chr7 = 0;
	}

	irq_enabled = 0;
	irq_counter = 0;
	irq_latch = 0;
}

static void map112_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr & 0xE001)
	{
	case 0x8000:
		{
			regs[0] = data;
			map112_MMC3_set_PPU_banks();
			map112_MMC3_set_CPU_banks();
		}
		break;

	case 0xA000:
		{
			uint32 bank_num;
			regs[1] = data;
			bank_num = regs[1];
			switch(regs[0] & 0x07)
			{
			case 0x02:
				{
					if(num_1k_VROM_banks)
					{
						bank_num &= 0xfe;
						chr01 = bank_num;
						map112_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x03:
				{
					if(num_1k_VROM_banks)
					{
						bank_num &= 0xfe;
						chr23 = bank_num;
						map112_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x04:
				{
					if(num_1k_VROM_banks)
					{
						chr4 = bank_num;
						map112_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x05:
				{
					if(num_1k_VROM_banks)
					{
						chr5 = bank_num;
						map112_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x06:
				{
					if(num_1k_VROM_banks)
					{
						chr6 = bank_num;
						map112_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x07:
				{
					if(num_1k_VROM_banks)
					{
						chr7 = bank_num;
						map112_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x00:
				{
					prg0 = bank_num;
					map112_MMC3_set_CPU_banks();
				}
				break;

			case 0x01:
				{
					prg1 = bank_num;
					map112_MMC3_set_CPU_banks();
				}
				break;
			}
		}
		break;

	case 0x8001:
		{
			regs[2] = data;
			//if(parent_NES->ROM->get_mirroring() != NES_PPU::MIRROR_FOUR_SCREEN)
			if (mmc_getinfo()->var.mirroring != 5)
			{
				if(data & 0x01)
				{
					//set_mirroring(NES_PPU::MIRROR_VERT);
					ppu_mirror(0,1,0,1);
				}
				else
				{
					//set_mirroring(NES_PPU::MIRROR_HORIZ);
					ppu_mirror(0,0,1,1);
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
		{
			regs[4] = data;
			irq_counter = regs[4];
		}
		break;

	case 0xC001:
		{
			regs[5] = data;
			irq_latch = regs[5];
		}
		break;

	case 0xE000:
		{
			regs[6] = data;
			irq_enabled = 0;
			if(data)
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
		break;

	case 0xE001:
		{
			regs[7] = data;
			irq_enabled = 1;
		}
		break;
	}
}

#if 0
static void map112_HSync(uint32 scanline)
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
#endif

#define MAP112_ROM(ptr)  (((ptr)-mmc_getinfo()->var.prg_beg)  >> 13)
#define MAP112_VROM(ptr) (((ptr)-mmc_getinfo()->var.chr_beg) >> 10)

/////////////////////////////////////////////////////////////////////

static void map112_getstate(SnssMapperBlock *state)
{
	
}


static void map112_setstate(SnssMapperBlock *state)
{
	prg0 = MAP112_ROM(mmc_getinfo()->CPUPageIndex[map112_prg_swap() ? 6 : 4]);
	prg1 = MAP112_ROM(mmc_getinfo()->CPUPageIndex[5]);
	if(num_1k_VROM_banks)
	{
		if(map112_chr_swap())
		{
			chr01 = MAP112_VROM(mmc_getinfo()->PPUPageIndex[4]);
			chr23 = MAP112_VROM(mmc_getinfo()->PPUPageIndex[6]);
			chr4  = MAP112_VROM(mmc_getinfo()->PPUPageIndex[0]);
			chr5  = MAP112_VROM(mmc_getinfo()->PPUPageIndex[1]);
			chr6  = MAP112_VROM(mmc_getinfo()->PPUPageIndex[2]);
			chr7  = MAP112_VROM(mmc_getinfo()->PPUPageIndex[3]);
		}
		else
		{
			chr01 = MAP112_VROM(mmc_getinfo()->PPUPageIndex[0]);
			chr23 = MAP112_VROM(mmc_getinfo()->PPUPageIndex[2]);
			chr4  = MAP112_VROM(mmc_getinfo()->PPUPageIndex[4]);
			chr5  = MAP112_VROM(mmc_getinfo()->PPUPageIndex[5]);
			chr6  = MAP112_VROM(mmc_getinfo()->PPUPageIndex[6]);
			chr7  = MAP112_VROM(mmc_getinfo()->PPUPageIndex[7]);
		}
	}
}


mapintf_t map112_intf =
{
   112, /* mapper number */
   "Map112", /* mapper name */
   map112_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map112_getstate, /* get state (snss) */
   map112_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map112_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

