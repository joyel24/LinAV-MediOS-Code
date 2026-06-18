#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 irq_counter;
static uint8 irq_enabled;
/////////////////////////////////////////////////////////////////////
// Mapper 42
static void map42_Reset()
{
	// set CPU bank pointers
	/*set_CPU_bank3(0);
	set_CPU_bank4(num_8k_ROM_banks-4);
	set_CPU_bank5(num_8k_ROM_banks-3);
	set_CPU_bank6(num_8k_ROM_banks-2);
	set_CPU_bank7(num_8k_ROM_banks-1);*/
	mmc_bankromALL5(0,num_8k_ROM_banks-4,num_8k_ROM_banks-3,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	//set_PPU_banks(0,1,2,3,4,5,6,7);
}

static void map42_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr & 0xE003)
	{
	case 0x8000:
		//set_PPU_banks(((data&0x1f)<<3)+0,((data&0x1f)<<3)+1,((data&0x1f)<<3)+2,((data&0x1f)<<3)+3,((data&0x1f)<<3)+4,((data&0x1f)<<3)+5,((data&0x1f)<<3)+6,((data&0x1f)<<3)+7);
		mmc_bankvrom(8,0x0000,(data&0x1f));
		break;
	case 0xE000:
		{
			//set_CPU_bank3(data & 0x0F);
			mmc_bankrom3(data & 0x0F);
		}
		break;

	case 0xE001:
		{
			if(data & 0x08)
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
		break;

	case 0xE002:
		{
			if(data & 0x02)
			{
				irq_enabled = 1;
			}
			else
			{
				irq_enabled = 0;
				irq_counter = 0;
			}
		}
		break;
	}
//	LOG("W " << HEX(addr,4) << "  " << HEX(data,2) << endl);
}

static void map42_HSync(uint32 scanline)
{
	if(irq_enabled)
	{
		if(irq_counter < 215)
		{
			irq_counter++;
		}
		if(irq_counter == 215)
		{
			//parent_NES->cpu->DoIRQ();
			doIRQ();
			irq_enabled = 0;
		}
	}
}
/////////////////////////////////////////////////////////////////////

static void map42_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper42.irqCounter = irq_counter;
	state->extraData.mapper42.irqCounterEnabled = irq_enabled;
}

static void map42_setstate(SnssMapperBlock *state)
{
	irq_counter = state->extraData.mapper42.irqCounter;
	irq_enabled = state->extraData.mapper42.irqCounterEnabled;
}

mapintf_t map42_intf =
{
   42, /* mapper number */
   "map42", /* mapper name */
   map42_Reset, /* init routine */
   NULL, /* vblank callback */
   map42_HSync, /* hblank callback */
   map42_getstate, /* get state (snss) */
   map42_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map42_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};

