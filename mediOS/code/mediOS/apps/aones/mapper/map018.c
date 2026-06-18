#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"


static uint8 patch;
static uint8 regs[11];
static uint8 irq_enabled;
static uint32 irq_latch;
static uint32 irq_counter;
	
/////////////////////////////////////////////////////////////////////
// Mapper 18
static void map18_Reset(void)
{
	if(mmc_getinfo()->var.crc == 0x424ec0a6)   // Jajamaru Gekimaden - Maboroshi no Kinmajou (J)
	{
		patch = 1;
	}

	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	regs[0] = 0;
	regs[1] = 1;
	regs[2] = mmc_getinfo()->ROM_Header.prg_rom_pages_nb*2-2; // num_8k_ROM_banks-2;
	regs[3] = mmc_getinfo()->ROM_Header.prg_rom_pages_nb*2-1; // num_8k_ROM_banks-1;
	regs[4] = 0;
	regs[5] = 0;
	regs[6] = 0;
	regs[7] = 0;
	regs[8] = 0;
	regs[9] = 0;
	regs[10] = 0;

	irq_enabled = 0;
	irq_latch = 0;
	irq_counter = 0;
}

static void map18_MemoryWrite(uint32 addr, uint8 data)
{
	// regs[ 0] ... 8K PROM bank at CPU $8000
	// regs[ 1] ... 8K PROM bank at CPU $A000
	// regs[ 2] ... 8K PROM bank at CPU $C000
	// regs[ 3] ... 1K VROM bank at PPU $0000
	// regs[ 4] ... 1K VROM bank at PPU $0400
	// regs[ 5] ... 1K VROM bank at PPU $0800
	// regs[ 6] ... 1K VROM bank at PPU $0C00
	// regs[ 7] ... 1K VROM bank at PPU $1000
	// regs[ 8] ... 1K VROM bank at PPU $1400
	// regs[ 9] ... 1K VROM bank at PPU $1800
	// regs[10] ... 1K VROM bank at PPU $1C00

	switch(addr)
	{
	case 0x8000:
		{
			regs[0] = (regs[0] & 0xF0) | (data & 0x0F);
			//set_CPU_bank4(regs[0]);
			mmc_bankrom(8,0x8000,regs[0]);
		}
		break;

	case 0x8001:
		{
			regs[0] = (regs[0] & 0x0F) | ((data & 0x0F) << 4);
			//set_CPU_bank4(regs[0]);
			mmc_bankrom(8,0x8000,regs[0]);
		}
		break;

	case 0x8002:
		{
			regs[1] = (regs[1] & 0xF0) | (data & 0x0F);
			//set_CPU_bank5(regs[1]);
			mmc_bankrom(8,0xA000,regs[1]);
		}
		break;

	case 0x8003:
		{
			regs[1] = (regs[1] & 0x0F) | ((data & 0x0F) << 4);
			//set_CPU_bank5(regs[1]);
			mmc_bankrom(8,0xA000,regs[1]);
		}
		break;

	case 0x9000:
		{
			regs[2] = (regs[2] & 0xF0) | (data & 0x0F);
			//set_CPU_bank6(regs[2]);
			mmc_bankrom(8,0xC000,regs[2]);
		}
		break;

	case 0x9001:
		{
			regs[2] = (regs[2] & 0x0F) | ((data & 0x0F) << 4);
			//set_CPU_bank6(regs[2]);
			mmc_bankrom(8,0xC000,regs[2]);
		}
		break;

	case 0xA000:
		{
			regs[3] = (regs[3] & 0xF0) | (data & 0x0F);
			//set_PPU_bank0(regs[3]);
			mmc_bankvrom(1,0x0000,regs[3]);
		}
		break;

	case 0xA001:
		{
			regs[3] = (regs[3] & 0x0F) | ((data & 0x0F) << 4);
			//set_PPU_bank0(regs[3]);
			mmc_bankvrom(1,0x0000,regs[3]);
		}
		break;

	case 0xA002:
		{
			regs[4] = (regs[4] & 0xF0) | (data & 0x0F);
			//set_PPU_bank1(regs[4]);
			mmc_bankvrom(1,0x0400,regs[4]);
		}
		break;

	case 0xA003:
		{
			regs[4] = (regs[4] & 0x0F) | ((data & 0x0F) << 4);
			//set_PPU_bank1(regs[4]);
			mmc_bankvrom(1,0x0400,regs[4]);
		}
		break;

	case 0xB000:
		{
			regs[5] = (regs[5] & 0xF0) | (data & 0x0F);
			//set_PPU_bank2(regs[5]);
			mmc_bankvrom(1,0x0800,regs[5]);
		}
		break;

	case 0xB001:
		{
			regs[5] = (regs[5] & 0x0F) | ((data & 0x0F) << 4);
			//set_PPU_bank2(regs[5]);
			mmc_bankvrom(1,0x0800,regs[5]);
		}
		break;

	case 0xB002:
		{
			regs[6] = (regs[6] & 0xF0) | (data & 0x0F);
			//set_PPU_bank3(regs[6]);
			mmc_bankvrom(1,0x0C00,regs[6]);
		}
		break;

	case 0xB003:
		{
			regs[6] = (regs[6] & 0x0F) | ((data & 0x0F) << 4);
			//set_PPU_bank3(regs[6]);
			mmc_bankvrom(1,0x0C00,regs[6]);
		}
		break;

	case 0xC000:
		{
			regs[7] = (regs[7] & 0xF0) | (data & 0x0F);
			//set_PPU_bank4(regs[7]);
			mmc_bankvrom(1,0x1000,regs[7]);
		}
		break;

	case 0xC001:
		{
			regs[7] = (regs[7] & 0x0F) | ((data & 0x0F) << 4);
			//set_PPU_bank4(regs[7]);
			mmc_bankvrom(1,0x1000,regs[7]);
		}
		break;

	case 0xC002:
		{
			regs[8] = (regs[8] & 0xF0) | (data & 0x0F);
			//set_PPU_bank5(regs[8]);
			mmc_bankvrom(1,0x1400,regs[8]);
		}
		break;

	case 0xC003:
		{
			regs[8] = (regs[8] & 0x0F) | ((data & 0x0F) << 4);
			//set_PPU_bank5(regs[8]);
			mmc_bankvrom(1,0x1400,regs[8]);
		}
		break;

	case 0xD000:
		{
			regs[9] = (regs[9] & 0xF0) | (data & 0x0F);
			//set_PPU_bank6(regs[9]);
			mmc_bankvrom(1,0x1800,regs[9]);
		}
		break;

	case 0xD001:
		{
			regs[9] = (regs[9] & 0x0F) | ((data & 0x0F) << 4);
			//set_PPU_bank6(regs[9]);
			mmc_bankvrom(1,0x1800,regs[9]);
		}
		break;

	case 0xD002:
		{
			regs[10] = (regs[10] & 0xF0) | (data & 0x0F);
			//set_PPU_bank7(regs[10]);
			mmc_bankvrom(1,0x1C00,regs[10]);
		}
		break;

	case 0xD003:
		{
			regs[10] = (regs[10] & 0x0F) | ((data & 0x0F) << 4);
			//set_PPU_bank7(regs[10]);
			mmc_bankvrom(1,0x1C00,regs[10]);
		}
		break;

	case 0xE000:
		{
			irq_latch = (irq_latch & 0xFFF0) | (data & 0x0F);
		}
		break;

	case 0xE001:
		{
			irq_latch = (irq_latch & 0xFF0F) | ((data & 0x0F) << 4);
		}
		break;

	case 0xE002:
		{
			irq_latch = (irq_latch & 0xF0FF) | ((data & 0x0F) << 8);
		}
		break;

	case 0xE003:
		{
			irq_latch = (irq_latch & 0x0FFF) | ((data & 0x0F) << 12);
		}
		break;

	case 0xF000:
		{
			irq_counter = irq_latch;
		}
		break;

	case 0xF001:
		{
			irq_enabled = data & 0x01;
		}
		break;

	case 0xF002:
		{
			data &= 0x03;
			if(data == 0)
			{
				//set_mirroring(NES_PPU::MIRROR_HORIZ);
				ppu_mirror(0,0,1,1);
			}
			else if(data == 1)
			{
				//set_mirroring(NES_PPU::MIRROR_VERT);
				ppu_mirror(0,1,0,1);
			}
			else
			{
				//set_mirroring(0,0,0,0);
				ppu_mirror(0,0,0,0);
			}
		}
		break;
	}
}

static void map18_HSync(uint32 scanline)
{
	if(irq_enabled)
	{
		if(irq_counter <= 113)
		{
			//parent_NES->cpu->DoIRQ();
			doIRQ();
			irq_counter = (patch == 1) ? 114 : 0;
			irq_enabled = 0;
		}
		else
		{
			irq_counter -= 113;
		}
	}
}

#define MAP18_ROM(ptr)  (((ptr)-mmc_getinfo()->var.prg_beg)  >> 13)
#define MAP18_VROM(ptr) (((ptr)-mmc_getinfo()->var.chr_beg) >> 10)




static void map18_getstate(SnssMapperBlock *state)
{
   state->extraData.mapper18.irqCounterLowByte = irq_counter & 0x00FF;
   state->extraData.mapper18.irqCounterHighByte = (irq_counter & 0xFF00) >> 8;
   state->extraData.mapper18.irqCounterEnabled = irq_enabled;   
}

static void map18_setstate(SnssMapperBlock *state)
{
   irq_counter = state->extraData.mapper18.irqCounterLowByte;
   irq_counter |= state->extraData.mapper18.irqCounterHighByte << 8;
   irq_enabled = state->extraData.mapper18.irqCounterEnabled;   
   
   regs[0] = MAP18_ROM(mmc_getinfo()->CPUPageIndex[4]);
   regs[1] = MAP18_ROM(mmc_getinfo()->CPUPageIndex[5]);
   regs[2] = MAP18_ROM(mmc_getinfo()->CPUPageIndex[6]);
   regs[3] = MAP18_VROM(mmc_getinfo()->PPUPageIndex[0]);
   regs[4] = MAP18_VROM(mmc_getinfo()->PPUPageIndex[1]);
   regs[5] = MAP18_VROM(mmc_getinfo()->PPUPageIndex[2]);
   regs[6] = MAP18_VROM(mmc_getinfo()->PPUPageIndex[3]);
   regs[7] = MAP18_VROM(mmc_getinfo()->PPUPageIndex[4]);
   regs[8] = MAP18_VROM(mmc_getinfo()->PPUPageIndex[5]);
   regs[9] = MAP18_VROM(mmc_getinfo()->PPUPageIndex[6]);
   regs[10] = MAP18_VROM(mmc_getinfo()->PPUPageIndex[7]);
}

mapintf_t map18_intf =
{
   18, /* mapper number */
   "Jaleco SS8806", /* mapper name */
   map18_Reset, /* init routine */
   NULL, /* vblank callback */
   map18_HSync, /* hblank callback */
   map18_getstate, /* get state (snss) */
   map18_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map18_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};

