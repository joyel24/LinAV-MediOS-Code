#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"



extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 patch;
static uint8 regs[11];
static uint8 irq_enabled;
static uint8 irq_counter;
static uint8 irq_latch;
/////////////////////////////////////////////////////////////////////
// Mapper 25
static void map25_Reset()
{
	int i;
	patch = 0;

	if(mmc_getinfo()->var.crc == 0xea74c587 || // Teenage Mutant Ninja Turtles
	        mmc_getinfo()->var.crc == 0x5f82cb7d || // Teenage Mutant Ninja Turtles 2
	        mmc_getinfo()->var.crc == 0xa2e68da8)   // Racer Mini Yonku - Japan Cup
	{
		patch = 1;
	}

	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/

	for(i = 0; i < 8; i++)
		regs[i] = 0;

	regs[8] = 0;
	regs[9] = num_8k_ROM_banks-2;
	regs[10] = 0;

	irq_enabled = 0;
	irq_counter = 0;
	irq_latch = 0;
}

static void map25_MemoryWrite(uint32 addr, uint8 data)
{
	// regs[0] ... 1K VROM bank at PPU $0000
	// regs[1] ... 1K VROM bank at PPU $0400
	// regs[2] ... 1K VROM bank at PPU $0800
	// regs[3] ... 1K VROM bank at PPU $0C00
	// regs[4] ... 1K VROM bank at PPU $1000
	// regs[5] ... 1K VROM bank at PPU $1400
	// regs[6] ... 1K VROM bank at PPU $1800
	// regs[7] ... 1K VROM bank at PPU $1C00
	// regs[8] ... 8K ROM bank at CPU $8000
	// regs[9] ... 8K ROM bank at CPU $C000
	// regs[10] .. ROM Swap flag

	switch(addr & 0xF000)
	{
	case 0x8000:
		{
			if(regs[10] & 0x02)
			{
				regs[9] = data;
				mmc_bankrom6(data);
			}
			else
			{
				regs[8] = data;
				mmc_bankrom4(data);
			}
		}
		break;

	case 0xA000:
		{
			mmc_bankrom5(data);
		}
		break;
	}

	switch(addr & 0xF00F)
	{
	case 0x9000:
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
				ppu_mirror(0,0,0,0);
			}
			else
			{
				ppu_mirror(1,1,1,1);
			}
		}
		break;

	case 0x9001:
	case 0x9004:
		{
			if((regs[10] & 0x02) != (data & 0x02))
			{
				uint8 swap = regs[8];
				regs[8] = regs[9];
				regs[9] = swap;
				mmc_bankrom4(regs[8]);
				mmc_bankrom6(regs[9]);
			}
			regs[10] = data;
		}
		break;

	case 0xB000:
		{
			regs[0] = (regs[0] & 0xF0) | (data & 0x0F);
			mmc_bankvrom0(regs[0]);
		}
		break;

	case 0xB001:
	case 0xB004:
		{
			regs[1] = (regs[1] & 0xF0) | (data & 0x0F);
			mmc_bankvrom1(regs[1]);
		}
		break;

	case 0xB002:
	case 0xB008:
		{
			regs[0] = (regs[0] & 0x0F) | ((data & 0x0F) << 4);
			mmc_bankvrom0(regs[0]);
		}
		break;

	case 0xB003:
	case 0xB00C:
		{
			regs[1] = (regs[1] & 0x0F) | ((data & 0x0F) << 4);
			mmc_bankvrom1(regs[1]);
		}
		break;

	case 0xC000:
		{
			regs[2] = (regs[2] & 0xF0) | (data & 0x0F);
			mmc_bankvrom2(regs[2]);
		}
		break;

	case 0xC001:
	case 0xC004:
		{
			regs[3] = (regs[3] & 0xF0) | (data & 0x0F);
			mmc_bankvrom3(regs[3]);
		}
		break;

	case 0xC002:
	case 0xC008:
		{
			regs[2] = (regs[2] & 0x0F) | ((data & 0x0F) << 4);
			mmc_bankvrom2(regs[2]);
		}
		break;

	case 0xC003:
	case 0xC00C:
		{
			regs[3] = (regs[3] & 0x0F) | ((data & 0x0F) << 4);
			mmc_bankvrom3(regs[3]);
		}
		break;

	case 0xD000:
		{
			regs[4] = (regs[4] & 0xF0) | (data & 0x0F);
			mmc_bankvrom4(regs[4]);
		}
		break;

	case 0xD001:
	case 0xD004:
		{
			regs[5] = (regs[5] & 0xF0) | (data & 0x0F);
			mmc_bankvrom5(regs[5]);
		}
		break;

	case 0xD002:
	case 0xD008:
		{
			regs[4] = (regs[4] & 0x0F) | ((data & 0x0F) << 4);
			mmc_bankvrom4(regs[4]);
		}
		break;

	case 0xD003:
	case 0xD00C:
		{
			regs[5] = (regs[5] & 0x0F) | ((data & 0x0F) << 4);
			mmc_bankvrom5(regs[5]);
		}
		break;

	case 0xE000:
		{
			regs[6] = (regs[6] & 0xF0) | (data & 0x0F);
			mmc_bankvrom6(regs[6]);
		}
		break;

	case 0xE001:
	case 0xE004:
		{
			regs[7] = (regs[7] & 0xF0) | (data & 0x0F);
			mmc_bankvrom7(regs[7]);
		}
		break;

	case 0xE002:
	case 0xE008:
		{
			regs[6] = (regs[6] & 0x0F) | ((data & 0x0F) << 4);
			mmc_bankvrom6(regs[6]);
		}
		break;

	case 0xE003:
	case 0xE00C:
		{
			regs[7] = (regs[7] & 0x0F) | ((data & 0x0F) << 4);
			mmc_bankvrom7(regs[7]);
		}
		break;

	case 0xF000:
		{
			irq_latch = (irq_latch & 0xF0) | (data & 0x0F);
		}
		break;

	case 0xF001:
	case 0xF004:
		{
			irq_enabled = data & 0x03;
			if(irq_enabled & 0x02)
			{
				irq_counter = irq_latch;
			}
		}
		break;

	case 0xF002:
	case 0xF008:
		{
			irq_latch = (irq_latch & 0x0F) | ((data & 0x0F) << 4);
		}
		break;

	case 0xF003:
	case 0xF00C:
		{
			irq_enabled = (irq_enabled & 0x01) * 3;
		}
		break;
	}
}

static void map25_HSync(uint32 scanline)
{
	if(irq_enabled & 0x02)
	{
		if(!patch && irq_counter == 0xFF)
		{
			irq_counter = irq_latch;
			//parent_NES->cpu->DoIRQ();
			doIRQ();
		}
		else if(patch && irq_counter == 0x00)
		{
			irq_counter = irq_latch;
			//parent_NES->cpu->DoIRQ();
			doIRQ();
		}
		else
		{
			irq_counter++;
		}
	}
}

#define MAP25_ROM(ptr)  (((ptr)-mmc_getinfo()->var.prg_beg)  >> 13)
#define MAP25_VROM(ptr) (((ptr)-mmc_getinfo()->var.chr_beg) >> 10)

static void map25_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper25.irqCounter = irq_counter;
	state->extraData.mapper25.irqCounterEnabled = irq_enabled;
	state->extraData.mapper25.irqLatchCounter = irq_latch;
	state->extraData.mapper25.last9001Write = regs[10];

}


static void map25_setstate(SnssMapperBlock *state)
{
	irq_counter = state->extraData.mapper25.irqCounter;
	irq_enabled = state->extraData.mapper25.irqCounterEnabled;
	irq_latch = state->extraData.mapper25.irqLatchCounter;
	regs[10] = state->extraData.mapper25.last9001Write;
	
	regs[0] = MAP25_VROM(mmc_getinfo()->PPUPageIndex[0]);
	regs[1] = MAP25_VROM(mmc_getinfo()->PPUPageIndex[1]);
	regs[2] = MAP25_VROM(mmc_getinfo()->PPUPageIndex[2]);
	regs[3] = MAP25_VROM(mmc_getinfo()->PPUPageIndex[3]);
	regs[4] = MAP25_VROM(mmc_getinfo()->PPUPageIndex[4]);
	regs[5] = MAP25_VROM(mmc_getinfo()->PPUPageIndex[5]);
	regs[6] = MAP25_VROM(mmc_getinfo()->PPUPageIndex[6]);
	regs[7] = MAP25_VROM(mmc_getinfo()->PPUPageIndex[7]);
	regs[8] = MAP25_ROM(mmc_getinfo()->CPUPageIndex[4]);
	regs[9] = MAP25_ROM(mmc_getinfo()->CPUPageIndex[6]);
}


mapintf_t map25_intf =
{
   25, /* mapper number */
   "map25", /* mapper name */
   map25_Reset, /* init routine */
   NULL, /* vblank callback */
   map25_HSync, /* hblank callback */
   map25_getstate, /* get state (snss) */
   map25_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map25_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

