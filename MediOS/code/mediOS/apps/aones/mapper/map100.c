#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8  regs[8];

static uint32 prg0,prg1,prg2,prg3;
static uint32 chr0,chr1,chr2,chr3,chr4,chr5,chr6,chr7;

static uint8 irq_enabled; // IRQs enabled
static uint8 irq_counter; // IRQ scanline counter, decreasing
static uint8 irq_latch;   // IRQ scanline counter latch

static void map100_MMC3_set_cpu_banks()
	{
		mmc_bankromALL4(prg0,prg1,prg2,prg3);
	}

static void map100_MMC3_set_ppu_banks()
	{
		if(num_1k_VROM_banks)
		{
			mmc_bankvromALL8(chr0,chr1,chr2,chr3,chr4,chr5,chr6,chr7);
		}
	}


/////////////////////////////////////////////////////////////////////
// Mapper 100
static void map100_Reset()
{
	int i;
	// clear registers FIRST!!!
	for(i = 0; i < 8; i++) regs[i] = 0x00;

	// set CPU bank pointers
	prg0 = 0;
	prg1 = 1;
	prg2 = num_8k_ROM_banks - 2;
	prg3 = num_8k_ROM_banks - 1;
	map100_MMC3_set_cpu_banks();

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
		map100_MMC3_set_ppu_banks();
	}
	else
	{
		chr0 = chr2 = chr4 = chr5 = chr6 = chr7 = 0;
		chr1 = chr3 = 1;
	}

	irq_enabled = 0;
	irq_counter = 0;
	irq_latch = 0;
}

static void map100_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr & 0xE001)
	{
	case 0x8000:
		{
			regs[0] = data;
		}
		break;

	case 0x8001:
		{
			uint32 bank_num;
			regs[1] = data;
			bank_num = regs[1];

			switch(regs[0] & 0xC7)
			{
			case 0x00:
				{
					if(num_1k_VROM_banks)
					{
						bank_num &= 0xfe;
						chr0 = bank_num;
						chr1 = bank_num + 1;
						map100_MMC3_set_ppu_banks();
					}
				}
				break;

			case 0x01:
				{
					if(num_1k_VROM_banks)
					{
						bank_num &= 0xfe;
						chr2 = bank_num;
						chr3 = bank_num + 1;
						map100_MMC3_set_ppu_banks();
					}
				}
				break;

			case 0x02:
				{
					if(num_1k_VROM_banks)
					{
						chr4 = bank_num;
						map100_MMC3_set_ppu_banks();
					}
				}
				break;

			case 0x03:
				{
					if(num_1k_VROM_banks)
					{
						chr5 = bank_num;
						map100_MMC3_set_ppu_banks();
					}
				}
				break;

			case 0x04:
				{
					if(num_1k_VROM_banks)
					{
						chr6 = bank_num;
						map100_MMC3_set_ppu_banks();
					}
				}
				break;

			case 0x05:
				{
					if(num_1k_VROM_banks)
					{
						chr7 = bank_num;
						map100_MMC3_set_ppu_banks();
					}
				}
				break;

			case 0x06:
				{
					prg0 = bank_num;
					map100_MMC3_set_cpu_banks();
				}
				break;

			case 0x07:
				{
					prg1 = bank_num;
					map100_MMC3_set_cpu_banks();
				}
				break;

			case 0x46:
				{
					prg2 = bank_num;
					map100_MMC3_set_cpu_banks();
				}
				break;

			case 0x47:
				{
					prg3 = bank_num;
					map100_MMC3_set_cpu_banks();
				}
				break;

			case 0x80:
				{
					if(num_1k_VROM_banks)
					{
						bank_num &= 0xfe;
						chr4 = bank_num;
						chr5 = bank_num + 1;
						map100_MMC3_set_ppu_banks();
					}
				}
				break;

			case 0x81:
				{
					if(num_1k_VROM_banks)
					{
						bank_num &= 0xfe;
						chr6 = bank_num;
						chr7 = bank_num + 1;
						map100_MMC3_set_ppu_banks();
					}
				}
				break;

			case 0x82:
				{
					if(num_1k_VROM_banks)
					{
						chr0 = bank_num;
						map100_MMC3_set_ppu_banks();
					}
				}
				break;

			case 0x83:
				{
					if(num_1k_VROM_banks)
					{
						chr1 = bank_num;
						map100_MMC3_set_ppu_banks();
					}
				}
				break;

			case 0x84:
				{
					if(num_1k_VROM_banks)
					{
						chr2 = bank_num;
						map100_MMC3_set_ppu_banks();
					}
				}
				break;

			case 0x85:
				{
					if(num_1k_VROM_banks)
					{
						chr3 = bank_num;
						map100_MMC3_set_ppu_banks();
					}
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

static void map100_HSync(uint32 scanline)
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

static void map100_getstate(SnssMapperBlock *state)
{
	
}


static void map100_setstate(SnssMapperBlock *state)
{
	
}


mapintf_t map100_intf =
{
   100, /* mapper number */
   "Map1010", /* mapper name */
   map100_Reset, /* init routine */
   NULL, /* vblank callback */
   map100_HSync, /* hblank callback */
   map100_getstate, /* get state (snss) */
   map100_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map100_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

