#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8  regs[1];
static uint32 prg0,prg1;
static uint32 chr01,chr23,chr4,chr5,chr6,chr7;

static uint32 map95_chr_swap() { return regs[0] & 0x80; }
static uint32 map95_prg_swap() { return regs[0] & 0x40; }

static void map95_MMC3_set_CPU_banks()
{
	if(map95_prg_swap())
	{
		mmc_bankromALL4(num_8k_ROM_banks-2,prg1,prg0,num_8k_ROM_banks-1);
	}
	else
	{
		mmc_bankromALL4(prg0,prg1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
	}
}

static void map95_MMC3_set_PPU_banks()
{
	if(num_1k_VROM_banks)
	{
		if(map95_chr_swap())
		{
			mmc_bankvromALL8(chr4,chr5,chr6,chr7,chr01,chr01+1,chr23,chr23+1);
		}
		else
		{
			mmc_bankvromALL8(chr01,chr01+1,chr23,chr23+1,chr4,chr5,chr6,chr7);
		}
	}
}

// Mapper 95
static void map95_Reset()
{
	regs[0] = 0x00;

	// set CPU bank pointers
	prg0 = 0;
	prg1 = 1;
	map95_MMC3_set_CPU_banks();

	// set VROM banks
	if(num_1k_VROM_banks)
	{
		chr01 = 0;
		chr23 = 2;
		chr4  = 4;
		chr5  = 5;
		chr6  = 6;
		chr7  = 7;
		map95_MMC3_set_PPU_banks();
	}
	else
	{
		chr01 = chr23 = chr4 = chr5 = chr6 = chr7 = 0;
	}
}

static void map95_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr & 0xE001)
	{
	case 0x8000:
		{
			regs[0] = data;
			map95_MMC3_set_PPU_banks();
			map95_MMC3_set_CPU_banks();
		}
		break;

	case 0x8001:
		{
			uint32 bank_num;

			if(regs[0] <= 0x05)
			{
				if(data & 0x20)
				{
					ppu_mirror(1,1,1,1);
					
				}
				else
				{
					ppu_mirror(0,0,0,0);
				}
				data &= 0x1F;
			}

			bank_num = data;

			switch(regs[0] & 0x07)
			{
			case 0x00:
				{
					if(num_1k_VROM_banks)
					{
						bank_num &= 0xfe;
						chr01 = bank_num;
						map95_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x01:
				{
					if(num_1k_VROM_banks)
					{
						bank_num &= 0xfe;
						chr23 = bank_num;
						map95_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x02:
				{
					if(num_1k_VROM_banks)
					{
						chr4 = bank_num;
						map95_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x03:
				{
					if(num_1k_VROM_banks)
					{
						chr5 = bank_num;
						map95_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x04:
				{
					if(num_1k_VROM_banks)
					{
						chr6 = bank_num;
						map95_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x05:
				{
					if(num_1k_VROM_banks)
					{
						chr7 = bank_num;
						map95_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x06:
				{
					prg0 = bank_num;
					map95_MMC3_set_CPU_banks();
				}
				break;

			case 0x07:
				{
					prg1 = bank_num;
					map95_MMC3_set_CPU_banks();
				}
				break;
			}
		}
		break;
	}
}

#define MAP95_ROM(ptr)  (((ptr)-mmc_getinfo()->var.prg_beg)  >> 13)
#define MAP95_VROM(ptr) (((ptr)-mmc_getinfo()->var.chr_beg) >> 10)

static void map95_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper95.last8000Write = regs[0];
}


static void map95_setstate(SnssMapperBlock *state)
{
	regs[0] = state->extraData.mapper95.last8000Write;
	if(num_1k_VROM_banks)
	{
		if(map95_chr_swap())
		{
			chr01 = MAP95_VROM(mmc_getinfo()->PPUPageIndex[4]);
			chr23 = MAP95_VROM(mmc_getinfo()->PPUPageIndex[6]);
			chr4  = MAP95_VROM(mmc_getinfo()->PPUPageIndex[0]);
			chr5  = MAP95_VROM(mmc_getinfo()->PPUPageIndex[1]);
			chr6  = MAP95_VROM(mmc_getinfo()->PPUPageIndex[2]);
			chr7  = MAP95_VROM(mmc_getinfo()->PPUPageIndex[3]);
		}
		else
		{
			chr01 = MAP95_VROM(mmc_getinfo()->PPUPageIndex[0]);
			chr23 = MAP95_VROM(mmc_getinfo()->PPUPageIndex[2]);
			chr4  = MAP95_VROM(mmc_getinfo()->PPUPageIndex[4]);
			chr5  = MAP95_VROM(mmc_getinfo()->PPUPageIndex[5]);
			chr6  = MAP95_VROM(mmc_getinfo()->PPUPageIndex[6]);
			chr7  = MAP95_VROM(mmc_getinfo()->PPUPageIndex[7]);
		}
	}
}


mapintf_t map95_intf =
{
   95, /* mapper number */
   "Map95", /* mapper name */
   map95_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map95_getstate, /* get state (snss) */
   map95_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map95_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

