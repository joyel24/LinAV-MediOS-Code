#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"



static uint8 irq_enabled;
static uint32 irq_counter;
static uint32 irq_latch;


/////////////////////////////////////////////////////////////////////
// Mapper 17
static void map17_Reset(void)
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
	

	// set PPU bank pointers
/*	if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/

	irq_enabled = 0;
	irq_counter = 0;
	irq_latch = 0;
}

static void map17_MemoryWriteLow(uint32 addr, uint8 data)
{
	switch(addr)
	{
	case 0x42FE:
		{
			if((data & 0x10) == 0)
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
		break;

	case 0x42FF:
		{
			if((data & 0x10) == 0)
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
		break;

	case 0x4501:
		{
			irq_enabled = 0;
		}
		break;

	case 0x4502:
		{
			irq_latch = (irq_latch & 0xFF00) | data;
		}
		break;

	case 0x4503:
		{
			irq_latch = (irq_latch & 0x00FF) | ((uint32)data << 8);
			irq_counter = irq_latch;
			irq_enabled = 1;
		}
		break;

	case 0x4504:
		{
			//set_CPU_bank4(data);
			mmc_bankrom(8,0x8000,data);
		}
		break;

	case 0x4505:
		{
			//set_CPU_bank5(data);
			mmc_bankrom(8,0xA000,data);
		}
		break;

	case 0x4506:
		{
			//set_CPU_bank6(data);
			mmc_bankrom(8,0xC000,data);
		}
		break;

	case 0x4507:
		{
			//set_CPU_bank7(data);
			mmc_bankrom(8,0xE000,data);
		}
		break;

	case 0x4510:
		{
			//set_PPU_bank0(data);
			mmc_bankvrom(1,0x0000,data);
		}
		break;

	case 0x4511:
		{
			//set_PPU_bank1(data);
			mmc_bankvrom(1,0x0400,data);
		}
		break;

	case 0x4512:
		{
			//set_PPU_bank2(data);
			mmc_bankvrom(1,0x0800,data);
		}
		break;

	case 0x4513:
		{
			//set_PPU_bank3(data);
			mmc_bankvrom(1,0x0C00,data);
		}
		break;

	case 0x4514:
		{
			//set_PPU_bank4(data);
			mmc_bankvrom(1,0x1000,data);
		}
		break;

	case 0x4515:
		{
			//set_PPU_bank5(data);
			mmc_bankvrom(1,0x1400,data);
		}
		break;

	case 0x4516:
		{
			//set_PPU_bank6(data);
			mmc_bankvrom(1,0x1800,data);
		}
		break;

	case 0x4517:
		{
			//set_PPU_bank7(data);
			mmc_bankvrom(1,0x1C00,data);
		}
		break;
	}
}

static void map17_HSync(uint32 scanline)
{
	if ((scanline<240)&&(ppu_enabled()))	
	if(irq_enabled)
	{
		if(irq_counter >= 0xFFFF - 113)
		{
			//parent_NES->cpu->DoIRQ();
			doIRQ();
			irq_counter = 0;
			irq_enabled = 0;
		}
		else
		{
			irq_counter += 113;
		}
	}
}

static void map17_getstate(SnssMapperBlock *state)
{
   state->extraData.mapper17.irqCounterLowByte = irq_counter & 0x00FF;
   state->extraData.mapper17.irqCounterHighByte = (irq_counter & 0xFF00) >> 8;
   state->extraData.mapper17.irqCounterEnabled = irq_enabled;   
}

static void map17_setstate(SnssMapperBlock *state)
{
   irq_counter = state->extraData.mapper17.irqCounterLowByte;
   irq_counter |= state->extraData.mapper17.irqCounterHighByte << 8;
   irq_enabled = state->extraData.mapper17.irqCounterEnabled;   
}

mapintf_t map17_intf =
{
   17, /* mapper number */
   "FFE F8xxx", /* mapper name */
   map17_Reset, /* init routine */
   NULL, /* vblank callback */
   map17_HSync, /* hblank callback */
   map17_getstate, /* get state (snss) */
   map17_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   map17_MemoryWriteLow,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   NULL,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};
