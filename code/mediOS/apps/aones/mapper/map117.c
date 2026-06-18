#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 irq_line;
static uint8 irq_enabled1;
static uint8 irq_enabled2;
/////////////////////////////////////////////////////////////////////
// Mapper 117
static void map117_Reset()
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/

	irq_line = 0;
	irq_enabled1 = 0;
	irq_enabled2 = 1;
}

static void map117_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr)
	{
	case 0x8000:
		{
			mmc_bankrom4(data);
		}
		break;

	case 0x8001:
		{
			mmc_bankrom5(data);
		}
		break;

	case 0x8002:
		{
			mmc_bankrom6(data);
		}
		break;

	case 0xA000:
		{
			mmc_bankvrom0(data);
		}
		break;

	case 0xA001:
		{
			mmc_bankvrom1(data);
		}
		break;

	case 0xA002:
		{
			mmc_bankvrom2(data);
		}
		break;

	case 0xA003:
		{
			mmc_bankvrom3(data);
		}
		break;

	case 0xA004:
		{
			mmc_bankvrom4(data);
		}
		break;

	case 0xA005:
		{
			mmc_bankvrom5(data);
		}
		break;

	case 0xA006:
		{
			mmc_bankvrom6(data);
		}
		break;

	case 0xA007:
		{
			mmc_bankvrom7(data);
		}
		break;

	case 0xA008:
	case 0xA009:
	case 0xA00a:
	case 0xA00b:
	case 0xA00c:
	case 0xA00d:
	case 0xA00e:
	case 0xA00f:
		break;

	case 0xC001:
	case 0xC002:
	case 0xC003:
		{
			irq_enabled1 = irq_line = data;
		}
		break;

	case 0xE000:
		{
			irq_enabled2 = data & 1;
		}
		break;
	}
}

static void map117_HSync(uint32 scanline)
{
	if(irq_enabled1 && irq_enabled2 && irq_line == scanline)
	{
		irq_enabled1 = 0;
		//parent_NES->cpu->DoIRQ();
		doIRQ();
	}
}
/////////////////////////////////////////////////////////////////////

static void map117_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper117.irqLine = irq_line;
	state->extraData.mapper117.irqEnabled1 = irq_enabled1;
	state->extraData.mapper117.irqEnabled2 = irq_enabled2;
}


static void map117_setstate(SnssMapperBlock *state)
{
	irq_line = state->extraData.mapper117.irqLine;
	irq_enabled1 = state->extraData.mapper117.irqEnabled1;
	irq_enabled2 = state->extraData.mapper117.irqEnabled2;
}


mapintf_t map117_intf =
{
   117, /* mapper number */
   "map117", /* mapper name */
   map117_Reset, /* init routine */
   NULL, /* vblank callback */
   map117_HSync, /* hblank callback */
   map117_getstate, /* get state (snss) */
   map117_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map117_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

