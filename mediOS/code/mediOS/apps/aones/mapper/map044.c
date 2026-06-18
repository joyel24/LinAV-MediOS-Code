#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8  regs[8];

static uint32 rom_bank;
static uint32 prg0,prg1;
static uint32 chr01,chr23,chr4,chr5,chr6,chr7;

static uint32 map44_chr_swap() { return regs[0] & 0x80; }
static uint32 map44_prg_swap() { return regs[0] & 0x40; }

static uint8 irq_enabled; // IRQs enabled
static uint8 irq_counter; // IRQ scanline counter, decreasing
static uint8 irq_latch;   // IRQ scanline counter latch
/////////////////////////////////////////////////////////////////////
// Mapper 44

static void map44_MMC3_set_CPU_banks()
{
	if(map44_prg_swap())
	{
		/*set_CPU_bank4(((rom_bank == 6) ? 0x1e : 0x0e) | (rom_bank << 4));
		set_CPU_bank5(((rom_bank == 6) ? 0x1f & prg1 : 0x0f & prg1) | (rom_bank << 4));
		set_CPU_bank6(((rom_bank == 6) ? 0x1f & prg0 : 0x0f & prg0) | (rom_bank << 4));
		set_CPU_bank7(((rom_bank == 6) ? 0x1f : 0x0f) | (rom_bank << 4));*/
		mmc_bankrom4(((rom_bank == 6) ? 0x1e : 0x0e) | (rom_bank << 4));
		mmc_bankrom5(((rom_bank == 6) ? 0x1f & prg1 : 0x0f & prg1) | (rom_bank << 4));
		mmc_bankrom6(((rom_bank == 6) ? 0x1f & prg0 : 0x0f & prg0) | (rom_bank << 4));
		mmc_bankrom7(((rom_bank == 6) ? 0x1f : 0x0f) | (rom_bank << 4));
	}
	else
	{
		/*set_CPU_bank4(((rom_bank == 6) ? 0x1f & prg0: 0x0f & prg0) | (rom_bank << 4));
		set_CPU_bank5(((rom_bank == 6) ? 0x1f & prg1: 0x0f & prg1) | (rom_bank << 4));
		set_CPU_bank6(((rom_bank == 6) ? 0x1e : 0x0e) | (rom_bank << 4));
		set_CPU_bank7(((rom_bank == 6) ? 0x1f : 0x0f) | (rom_bank << 4));*/
		mmc_bankrom4(((rom_bank == 6) ? 0x1f & prg0: 0x0f & prg0) | (rom_bank << 4));
		mmc_bankrom5(((rom_bank == 6) ? 0x1f & prg1: 0x0f & prg1) | (rom_bank << 4));
		mmc_bankrom6(((rom_bank == 6) ? 0x1e : 0x0e) | (rom_bank << 4));
		mmc_bankrom7(((rom_bank == 6) ? 0x1f : 0x0f) | (rom_bank << 4));
	}
}

static void map44_MMC3_set_PPU_banks()
{
	if(num_1k_VROM_banks)
	{
		if(map44_chr_swap())
		{
			mmc_bankvrom0(((rom_bank == 6) ? 0xff & chr4 : 0x7f & chr4) | (rom_bank << 7));
			mmc_bankvrom1(((rom_bank == 6) ? 0xff & chr5 : 0x7f & chr5) | (rom_bank << 7));
			mmc_bankvrom2(((rom_bank == 6) ? 0xff & chr6 : 0x7f & chr6) | (rom_bank << 7));
			mmc_bankvrom3(((rom_bank == 6) ? 0xff & chr7 : 0x7f & chr7) | (rom_bank << 7));
			mmc_bankvrom4(((rom_bank == 6) ? 0xff & chr01 : 0x7f & chr01) | (rom_bank << 7));
			mmc_bankvrom5(((rom_bank == 6) ? 0xff & (chr01+1) : 0x7f & (chr01+1)) | (rom_bank << 7));
			mmc_bankvrom6(((rom_bank == 6) ? 0xff & chr23: 0x7f & chr23) | (rom_bank << 7));
			mmc_bankvrom7(((rom_bank == 6) ? 0xff & (chr23+1) : 0x7f & (chr23+1)) | (rom_bank << 7));
		}
		else
		{
			mmc_bankvrom0(((rom_bank == 6) ? 0xff & chr01 : 0x7f & chr01) | (rom_bank << 7));
			mmc_bankvrom1(((rom_bank == 6) ? 0xff & (chr01+1) : 0x7f & (chr01+1)) | (rom_bank << 7));
			mmc_bankvrom2(((rom_bank == 6) ? 0xff & chr23: 0x7f & chr23) | (rom_bank << 7));
			mmc_bankvrom3(((rom_bank == 6) ? 0xff & (chr23+1) : 0x7f & (chr23+1)) | (rom_bank << 7));
			mmc_bankvrom4(((rom_bank == 6) ? 0xff & chr4 : 0x7f & chr4) | (rom_bank << 7));
			mmc_bankvrom5(((rom_bank == 6) ? 0xff & chr5 : 0x7f & chr5) | (rom_bank << 7));
			mmc_bankvrom6(((rom_bank == 6) ? 0xff & chr6 : 0x7f & chr6) | (rom_bank << 7));
			mmc_bankvrom7(((rom_bank == 6) ? 0xff & chr7 : 0x7f & chr7) | (rom_bank << 7));
		}
	}
}


static void map44_Reset()
{
	// clear registers FIRST!!!
	int i;
	for(i = 0; i < 8; i++) regs[i] = 0x00;

	// set game select register
	rom_bank = 0;

	// set CPU bank pointers
	prg0 = 0;
	prg1 = 1;
	map44_MMC3_set_CPU_banks();

	// set VROM banks
	if(num_1k_VROM_banks)
	{
		chr01 = 0;
		chr23 = 2;
		chr4  = 4;
		chr5  = 5;
		chr6  = 6;
		chr7  = 7;
		map44_MMC3_set_PPU_banks();
	}
	else
	{
		chr01 = chr23 = chr4 = chr5 = chr6 = chr7 = 0;
	}

	irq_enabled = 0;
	irq_counter = 0;
	irq_latch = 0;
}

static void map44_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr & 0xE001)
	{
	case 0x8000:
		{
			regs[0] = data;
			map44_MMC3_set_PPU_banks();
			map44_MMC3_set_CPU_banks();
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
						map44_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x01:
				{
					if(num_1k_VROM_banks)
					{
						bank_num &= 0xfe;
						chr23 = bank_num;
						map44_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x02:
				{
					if(num_1k_VROM_banks)
					{
						chr4 = bank_num;
						map44_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x03:
				{
					if(num_1k_VROM_banks)
					{
						chr5 = bank_num;
						map44_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x04:
				{
					if(num_1k_VROM_banks)
					{
						chr6 = bank_num;
						map44_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x05:
				{
					if(num_1k_VROM_banks)
					{
						chr7 = bank_num;
						map44_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x06:
				{
					prg0 = bank_num;
					map44_MMC3_set_CPU_banks();
				}
				break;

			case 0x07:
				{
					prg1 = bank_num;
					map44_MMC3_set_CPU_banks();
				}
				break;
			}
		}
		break;

	case 0xA000:
		{
			regs[2] = data;

			//if(parent_NES->ROM->get_mirroring() != NES_PPU::MIRROR_FOUR_SCREEN)
			if(mmc_getinfo()->var.mirroring != 5)
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
			rom_bank = data & 0x07;
			if(rom_bank == 7)
			{
				rom_bank = 6;
			}
			map44_MMC3_set_CPU_banks();
			map44_MMC3_set_PPU_banks();
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

static void map44_HSync(uint32 scanline)
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

static void map44_getstate(SnssMapperBlock *state)
{
}

static void map44_setstate(SnssMapperBlock *state)
{
}

mapintf_t map44_intf =
{
   44, /* mapper number */
   "map44", /* mapper name */
   map44_Reset, /* init routine */
   NULL, /* vblank callback */
   map44_HSync, /* hblank callback */
   map44_getstate, /* get state (snss) */
   map44_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map44_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};
