#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 irq_enabled;
static uint32 lines_to_irq;
/////////////////////////////////////////////////////////////////////
// Mapper 40 (smb2j)
static void map40_Reset()
{
	irq_enabled = 0;
	lines_to_irq = 0;

	// set CPU bank pointers
	//set_CPU_banks(6,4,5,0,7);
	mmc_bankromALL5(6,4,5,0,7);

	// set VROM banks
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/
}

static void map40_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr & 0xE000)
	{
	case 0x8000:
		irq_enabled = 0;
		//      LOG("MAP40: [$8000] = " << HEX(data,2) << endl);
		break;

	case 0xA000:
		irq_enabled = 1;
		lines_to_irq = 37;

		//      LOG("MAP40: [$A000] = " << HEX(data,2) << endl);
		break;

	case 0xC000:
		//      LOG("MAP40: [$C000] = " << HEX(data,2) << endl);
		//      LOG("MAP40: INVALID WRITE TO $C000" << endl);
		break;

	case 0xE000:
		//      LOG("MAP40: [$E000] = " << HEX(data,2) << endl);

		//set_CPU_bank6(data & 0x07);
		mmc_bankrom(8,0xC000,data & 0x07);
		break;

	}
}

static void map40_HSync(uint32 scanline)
{
	if(irq_enabled)
	{
		if((--lines_to_irq) <= 0)
		{
			//parent_NES->cpu->DoIRQ();
			doIRQ();
		}
	}
}
/////////////////////////////////////////////////////////////////////

static void map40_getstate(SnssMapperBlock *state)
{
	// IRQ counter
	state->extraData.mapper40.irqCounter = lines_to_irq;

	// IRQ enabled
	state->extraData.mapper40.irqCounterEnabled = irq_enabled;
}

static void map40_setstate(SnssMapperBlock *state)
{
	// IRQ counter
	lines_to_irq = state->extraData.mapper40.irqCounter;

	// IRQ enabled
	irq_enabled = state->extraData.mapper40.irqCounterEnabled;
}

mapintf_t map40_intf =
{
   40, /* mapper number */
   "map40", /* mapper name */
   map40_Reset, /* init routine */
   NULL, /* vblank callback */
   map40_HSync, /* hblank callback */
   map40_getstate, /* get state (snss) */
   map40_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map40_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};

