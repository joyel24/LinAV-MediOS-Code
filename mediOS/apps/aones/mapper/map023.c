#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"



static uint8 regs[9];
static uint32 patch;
static uint8 irq_enabled;
static uint8 irq_counter;
static uint8 irq_latch;
static uint8 irq_tmp;

/////////////////////////////////////////////////////////////////////
// Mapper 23
static void map23_Reset()
{
	patch = 0xFFFF;

	if(mmc_getinfo()->var.crc == 0x93794634) // Akumajou Special - Boku Dracula Kun
	{
		patch = 0xF00C;
	}

	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	//set_PPU_banks(0,1,2,3,4,5,6,7);

	regs[0] = 0;
	regs[1] = 1;
	regs[2] = 2;
	regs[3] = 3;
	regs[4] = 4;
	regs[5] = 5;
	regs[6] = 6;
	regs[7] = 7;
	regs[8] = 0;

	irq_enabled = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_tmp = 0;
}

static void map23_write(uint32 addr, uint8 data)
{
	// regs[0] ... 1K VROM bank at PPU $0000
	// regs[1] ... 1K VROM bank at PPU $0400
	// regs[2] ... 1K VROM bank at PPU $0800
	// regs[3] ... 1K VROM bank at PPU $0C00
	// regs[4] ... 1K VROM bank at PPU $1000
	// regs[5] ... 1K VROM bank at PPU $1400
	// regs[6] ... 1K VROM bank at PPU $1800
	// regs[7] ... 1K VROM bank at PPU $1C00
	// regs[8] ... $9008 swap

	switch(addr & patch)
	{
	case 0x8000:
	case 0x8004:
	case 0x8008:
	case 0x800C:
		{
			if(regs[8])
			{
				//set_CPU_bank6(data);
				mmc_bankrom(8,0xC000,data);
			}
			else
			{
				//set_CPU_bank4(data);
				mmc_bankrom(8,0x8000,data);
			}
		}
		break;

	case 0x9000:
		{
			//if(data != 0xFF)
			{
				data &= 0x03;
				if(data == 0)
				{
					//set_mirroring(NES_PPU::MIRROR_VERT);
					ppu_mirror(0,1,0,1);
				}
				else if(data == 1)
				{
					//set_mirroring(NES_PPU::MIRROR_HORIZ);
					ppu_mirror(0,0,1,1);
				}
				else if(data == 2)
				{
					//set_mirroring(0,0,0,0);
					ppu_mirror(0,0,0,0);
				}
				else
				{
					//set_mirroring(1,1,1,1);
					ppu_mirror(1,1,1,1);
				}
			}
		}
		break;

	case 0x9008:
		{
			regs[8] = data & 0x02;
		}
		break;

	case 0xA000:
	case 0xA004:
	case 0xA008:
	case 0xA00C:
		{
			//set_CPU_bank5(data);
			mmc_bankrom(8,0xA000,data);
		}
		break;

	case 0xB000:
		{
			regs[0] = (regs[0] & 0xF0) | (data & 0x0F);
			//set_PPU_bank0(regs[0]);
			mmc_bankvrom(1,0,regs[0]);
		}
		break;

	case 0xB001:
	case 0xB004:
		{
			regs[0] = (regs[0] & 0x0F) | ((data & 0x0F) << 4);
			//set_PPU_bank0(regs[0]);
			mmc_bankvrom(1,0,regs[0]);
		}
		break;

	case 0xB002:
	case 0xB008:
		{
			regs[1] = (regs[1] & 0xF0) | (data & 0x0F);
			//set_PPU_bank1(regs[1]);
			mmc_bankvrom(1,0x400,regs[1]);
		}
		break;

	case 0xB003:
	case 0xB00C:
		{
			regs[1] = (regs[1] & 0x0F) | ((data & 0x0F) << 4);
			//set_PPU_bank1(regs[1]);
			mmc_bankvrom(1,0x400,regs[1]);
		}
		break;

	case 0xC000:
		{
			regs[2] = (regs[2] & 0xF0) | (data & 0x0F);
			//set_PPU_bank2(regs[2]);
			mmc_bankvrom(1,0x800,regs[2]);
		}
		break;

	case 0xC001:
	case 0xC004:
		{
			regs[2] = (regs[2] & 0x0F) | ((data & 0x0F) << 4);
			//set_PPU_bank2(regs[2]);
			mmc_bankvrom(1,0x800,regs[2]);
		}
		break;

	case 0xC002:
	case 0xC008:
		{
			regs[3] = (regs[3] & 0xF0) | (data & 0x0F);
			//set_PPU_bank3(regs[3]);
			mmc_bankvrom(1,0xC00,regs[3]);
		}
		break;

	case 0xC003:
	case 0xC00C:
		{
			regs[3] = (regs[3] & 0x0F) | ((data & 0x0F) << 4);
			//set_PPU_bank3(regs[3]);
			mmc_bankvrom(1,0xC00,regs[3]);
		}
		break;

	case 0xD000:
		{
			regs[4] = (regs[4] & 0xF0) | (data & 0x0F);
			//set_PPU_bank4(regs[4]);
			mmc_bankvrom(1,0x1000,regs[4]);
		}
		break;

	case 0xD001:
	case 0xD004:
		{
			regs[4] = (regs[4] & 0x0F) | ((data & 0x0F) << 4);
			//set_PPU_bank4(regs[4]);
			mmc_bankvrom(1,0x1000,regs[4]);
		}
		break;

	case 0xD002:
	case 0xD008:
		{
			regs[5] = (regs[5] & 0xF0) | (data & 0x0F);
			//set_PPU_bank5(regs[5]);
			mmc_bankvrom(1,0x1400,regs[5]);
		}
		break;

	case 0xD003:
	case 0xD00C:
		{
			regs[5] = (regs[5] & 0x0F) | ((data & 0x0F) << 4);
			//set_PPU_bank5(regs[5]);
			mmc_bankvrom(1,0x1400,regs[5]);
		}
		break;

	case 0xE000:
		{
			regs[6] = (regs[6] & 0xF0) | (data & 0x0F);
			//set_PPU_bank6(regs[6]);
			mmc_bankvrom(1,0x1800,regs[6]);
		}
		break;

	case 0xE001:
	case 0xE004:
		{
			regs[6] = (regs[6] & 0x0F) | ((data & 0x0F) << 4);
			//set_PPU_bank6(regs[6]);
			mmc_bankvrom(1,0x1800,regs[6]);
		}
		break;

	case 0xE002:
	case 0xE008:
		{
			regs[7] = (regs[7] & 0xF0) | (data & 0x0F);
			//set_PPU_bank7(regs[7]);
			mmc_bankvrom(1,0x1C00,regs[7]);
		}
		break;

	case 0xE003:
	case 0xE00C:
		{
			regs[7] = (regs[7] & 0x0F) | ((data & 0x0F) << 4);
			//set_PPU_bank7(regs[7]);
			mmc_bankvrom(1,0x1C00,regs[7]);
		}
		break;

	case 0xF000:
		{
			irq_latch = (irq_latch & 0xF0) | (data & 0x0F);
		}
		break;

	case 0xF004:
		{
			irq_latch = (irq_latch & 0x0F) | ((data & 0x0F) << 4);
		}
		break;

	case 0xF008:
		{			
			irq_counter = (0x100 - irq_latch);
			irq_tmp = data & 0x01;
			irq_enabled = data & 0x02;			
			/*if(irq_enabled & 0x02)
			{
				irq_counter = irq_latch;
			}*/
		}
		break;

	case 0xF00C:
		{
			irq_enabled = irq_tmp;//(irq_enabled & 0x01) * 3;
		}
		break;
	}
}

static void map23_HSync(uint32 scanline)
{   
   if (irq_enabled)
	{		
		if(--irq_counter <= 0)
		{			
			irq_counter = (0x100 - irq_latch);			
			PendingIRQ();
		}		
	}
}

#define MAP23_VROM(ptr) (((ptr)-mmc_getinfo()->var.chr_beg) >> 10)

static void map23_getstate(SnssMapperBlock *state)
{
   state->extraData.mapper23.irqCounter = irq_counter;
   state->extraData.mapper23.irqCounterEnabled = irq_enabled;
   state->extraData.mapper23.irqLatchCounter = irq_latch;
   state->extraData.mapper23.last9008Write = regs[8];   
}

static void map23_setstate(SnssMapperBlock *state)
{
   irq_counter = state->extraData.mapper23.irqCounter;
   irq_enabled = state->extraData.mapper23.irqCounterEnabled;
   irq_latch = state->extraData.mapper23.irqLatchCounter;
   regs[8] = state->extraData.mapper23.last9008Write;
   
   regs[0] = MAP23_VROM(mmc_getinfo()->PPUPageIndex[0]);
   regs[1] = MAP23_VROM(mmc_getinfo()->PPUPageIndex[1]);
   regs[2] = MAP23_VROM(mmc_getinfo()->PPUPageIndex[2]);
   regs[3] = MAP23_VROM(mmc_getinfo()->PPUPageIndex[3]);
   regs[4] = MAP23_VROM(mmc_getinfo()->PPUPageIndex[4]);
   regs[5] = MAP23_VROM(mmc_getinfo()->PPUPageIndex[5]);
   regs[6] = MAP23_VROM(mmc_getinfo()->PPUPageIndex[6]);
   regs[7] = MAP23_VROM(mmc_getinfo()->PPUPageIndex[7]);
}

mapintf_t map23_intf =
{
   23, /* mapper number */
   "VRC2", /* mapper name */
   map23_Reset, /* init routine */
   NULL, /* vblank callback */
   map23_HSync, /* hblank callback */
   map23_getstate, /* get state (snss) */
   map23_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map23_write,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};

