#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8  regs[8];
static uint32 prg0,prg1;
static uint32 chr0,chr1,chr2,chr3,chr4,chr5,chr6,chr7;
static uint32 map118_chr_swap() { return regs[0] & 0x80; }
static uint32 map118_prg_swap() { return regs[0] & 0x40; }
static uint8 irq_enabled; // IRQs enabled
static uint8 irq_counter; // IRQ scanline counter, decreasing
static uint8 irq_latch;   // IRQ scanline counter latch

static void map118_MMC3_set_CPU_banks()
{
	if(map118_prg_swap())
	{
		mmc_bankromALL4(num_8k_ROM_banks-2,prg1,prg0,num_8k_ROM_banks-1);
	}
	else
	{
		mmc_bankromALL4(prg0,prg1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
	}
}
static void map118_MMC3_set_PPU_banks()
{
	if(num_1k_VROM_banks)
	{
		if(map118_chr_swap())
		{
			mmc_bankvromALL8(chr4,chr5,chr6,chr7,chr0,chr1,chr2,chr3);
		}
		else
		{
			mmc_bankvromALL8(chr0,chr1,chr2,chr3,chr4,chr5,chr6,chr7);
		}
	}
}
/////////////////////////////////////////////////////////////////////
// Mapper 118
void map118_Reset()
{
	int i;
	// clear registers FIRST!!!
	for(i = 0; i < 8; i++) regs[i] = 0x00;

	// set CPU bank pointers
	prg0 = 0;
	prg1 = 1;
	map118_MMC3_set_CPU_banks();

	// set VROM banks
	if(num_1k_VROM_banks)
	{
		chr0 = 0;
		chr1 = 1;
		chr2 = 2;
		chr3 = 3;
		chr4 = 4;
		chr5 = 5;
		chr6 = 6;
		chr7 = 7;
		map118_MMC3_set_PPU_banks();
	}
	else
	{
		chr0 = chr1 = chr2 = chr3 = chr4 = chr5 = chr6 = chr7 = 0;
	}

	irq_enabled = 0;
	irq_counter = 0;
	irq_latch = 0;
}

void map118_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr & 0xE001)
	{
	case 0x8000:
		{
			regs[0] = data;
			map118_MMC3_set_PPU_banks();
			map118_MMC3_set_CPU_banks();
		}
		break;

	case 0x8001:
		{
			uint32 bank_num;
			regs[1] = data;
			bank_num = regs[1];

			if((regs[0] & 0x07) < 6)
			{
				if(data & 0x80)
				{
					ppu_mirror(0,0,0,0);
				}
				else
				{
					ppu_mirror(1,1,1,1);
				}
			}

			switch(regs[0] & 0x07)
			{
			case 0x00:
				{
					if(num_1k_VROM_banks)
					{
						bank_num &= 0xfe;
						chr0 = bank_num;
						chr1 = bank_num+1;
						map118_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x01:
				{
					if(num_1k_VROM_banks)
					{
						bank_num &= 0xfe;
						chr2 = bank_num;
						chr3 = bank_num+1;
						map118_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x02:
				{
					if(num_1k_VROM_banks)
					{
						chr4 = bank_num;
						map118_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x03:
				{
					if(num_1k_VROM_banks)
					{
						chr5 = bank_num;
						map118_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x04:
				{
					if(num_1k_VROM_banks)
					{
						chr6 = bank_num;
						map118_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x05:
				{
					if(num_1k_VROM_banks)
					{
						chr7 = bank_num;
						map118_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x06:
				{
					prg0 = bank_num;
					map118_MMC3_set_CPU_banks();
				}
				break;

			case 0x07:
				{
					prg1 = bank_num;
					map118_MMC3_set_CPU_banks();
				}
				break;
			}
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

void map118_HSync(uint32 scanline)
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

#define MAP118_ROM(ptr)  (((ptr)-mmc_getinfo()->var.prg_beg)  >> 13)
#define MAP118_VROM(ptr) (((ptr)-mmc_getinfo()->var.chr_beg) >> 10)


static void map118_getstate(SnssMapperBlock *state)
{
	
}


static void map118_setstate(SnssMapperBlock *state)
{
	prg0 = MAP118_ROM(mmc_getinfo()->CPUPageIndex[map118_prg_swap() ? 6 : 4]);
	prg1 = MAP118_ROM(mmc_getinfo()->CPUPageIndex[5]);
	if(num_1k_VROM_banks)
	{
		if(map118_chr_swap())
		{
			chr0 = MAP118_VROM(mmc_getinfo()->PPUPageIndex[4]);
			chr1 = MAP118_VROM(mmc_getinfo()->PPUPageIndex[5]);
			chr2 = MAP118_VROM(mmc_getinfo()->PPUPageIndex[6]);
			chr3 = MAP118_VROM(mmc_getinfo()->PPUPageIndex[7]);
			chr4 = MAP118_VROM(mmc_getinfo()->PPUPageIndex[0]);
			chr5 = MAP118_VROM(mmc_getinfo()->PPUPageIndex[1]);
			chr6 = MAP118_VROM(mmc_getinfo()->PPUPageIndex[2]);
			chr7 = MAP118_VROM(mmc_getinfo()->PPUPageIndex[3]);
		}
		else
		{
			chr0 = MAP118_VROM(mmc_getinfo()->PPUPageIndex[0]);
			chr1 = MAP118_VROM(mmc_getinfo()->PPUPageIndex[1]);
			chr2 = MAP118_VROM(mmc_getinfo()->PPUPageIndex[2]);
			chr3 = MAP118_VROM(mmc_getinfo()->PPUPageIndex[3]);
			chr4 = MAP118_VROM(mmc_getinfo()->PPUPageIndex[4]);
			chr5 = MAP118_VROM(mmc_getinfo()->PPUPageIndex[5]);
			chr6 = MAP118_VROM(mmc_getinfo()->PPUPageIndex[6]);
			chr7 = MAP118_VROM(mmc_getinfo()->PPUPageIndex[7]);
		}
	}
}


mapintf_t map118_intf =
{
   118, /* mapper number */
   "Map118", /* mapper name */
   map118_Reset, /* init routine */
   NULL, /* vblank callback */
   map118_HSync, /* hblank callback */
   map118_getstate, /* get state (snss) */
   map118_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map118_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

