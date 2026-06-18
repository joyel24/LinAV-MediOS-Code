#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 irq_enabled;
static uint32 irq_counter;
/////////////////////////////////////////////////////////////////////
// Mapper 73
static void map73_Reset()
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	irq_counter = 0;
	irq_enabled = 0;
}

static void map73_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr)
	{
	case 0x8000:
		{
			irq_counter = (irq_counter & 0xFFF0) | (data & 0x0F);
		}
		break;

	case 0x9000:
		{
			irq_counter = (irq_counter & 0xFF0F) | ((data & 0x0F) << 4);
		}
		break;

	case 0xA000:
		{
			irq_counter = (irq_counter & 0xF0FF) | ((data & 0x0F) << 8);
		}
		break;

	case 0xB000:
		{
			irq_counter = (irq_counter & 0x0FFF) | ((data & 0x0F) << 12);
		}
		break;

	case 0xC000:
		{
			irq_enabled = data;
		}
		break;

	case 0xF000:
		{
			/*set_CPU_bank4(data*2+0);
			set_CPU_bank5(data*2+1);*/
			mmc_bankrom(16,0x8000,data);
		}
		break;
	}
}

static void map73_HSync(uint32 scanline)
{
	if(irq_enabled & 0x02)
	{
		if(irq_counter > 0xFFFF - 114)
		{
//			parent_NES->cpu->DoIRQ();
			doIRQ();
			irq_enabled = 0;
		}
		else
		{
			irq_counter += 114;
		}
	}
}
/////////////////////////////////////////////////////////////////////

static void map73_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper73.irqCounterLowByte = irq_counter & 0x00FF;
	state->extraData.mapper73.irqCounterHighByte = (irq_counter & 0xFF00) >> 8;
	state->extraData.mapper73.irqCounterEnabled = irq_enabled;
}


static void map73_setstate(SnssMapperBlock *state)
{
	irq_counter = state->extraData.mapper73.irqCounterLowByte;
	irq_counter |= state->extraData.mapper73.irqCounterHighByte << 8;
	irq_enabled = state->extraData.mapper73.irqCounterEnabled;
}


mapintf_t map73_intf =
{
   73, /* mapper number */
   "map73", /* mapper name */
   map73_Reset, /* init routine */
   NULL, /* vblank callback */
   map73_HSync, /* hblank callback */
   map73_getstate, /* get state (snss) */
   map73_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map73_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};


