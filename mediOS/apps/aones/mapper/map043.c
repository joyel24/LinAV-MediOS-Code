#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 irq_enabled;
static uint32 irq_counter;
/////////////////////////////////////////////////////////////////////
// Mapper 43
static void map43_Reset()
{
	//set_CPU_banks(2,1,0,4,9);
	mmc_bankromALL5(2,1,0,4,9);
	//set_PPU_banks(0,1,2,3,4,5,6,7);

	//set_mirroring(NES_PPU::MIRROR_VERT);

	irq_enabled = 1;
	irq_counter = 0;
}

static uint8 map43_MemoryReadLow(uint32 addr)
{
	if(0x5000 <= addr && addr < 0x6000)
	{
		//return ROM_banks[0x2000*8 + 0x1000 + (addr - 0x5000)];
		return mmc_getinfo()->var.prg_beg[0x2000*8 + 0x1000 + (addr - 0x5000)];
	}
	return (uint8)(addr >> 8);
}

static void map43_MemoryWriteLow(uint32 addr, uint8 data)
{
	if((addr & 0xF0FF) == 0x4022)
	{
		switch(data & 0x07)
		{
		case 0x0:
		case 0x2:
		case 0x3:
		case 0x4:
			{
				//set_CPU_bank6(4);
				mmc_bankrom(8,0xC000,4);
			}
			break;

		case 0x1:
			{
				//set_CPU_bank6(3);
				mmc_bankrom(8,0xC000,3);
			}
			break;

		case 0x5:
			{
				//set_CPU_bank6(7);
				mmc_bankrom(8,0xC000,7);
			}
			break;

		case 0x6:
			{
				//set_CPU_bank6(5);
				mmc_bankrom(8,0xC000,5);
			}
			break;

		case 0x7:
			{
				//set_CPU_bank6(6);
				mmc_bankrom(8,0xC000,6);
			}
			break;
		}
	}
}

static void map43_MemoryWrite(uint32 addr, uint8 data)
{
	if(addr == 0x8122)
	{
		if(data & 3)
		{
			irq_enabled = 1;
		}
		else
		{
			irq_counter = 0;
			irq_enabled = 0;
		}
	}
}

static void map43_HSync(uint32 scanline)
{
	if(irq_enabled)
	{
		irq_counter += 114;
		if(irq_counter >= 4096)
		{
			irq_counter -= 4096;
			//parent_NES->cpu->DoIRQ();
			doIRQ();
		}
	}
}
/////////////////////////////////////////////////////////////////////

static void map43_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper43.irqCounterLowByte = irq_counter & 0x00FF;
	state->extraData.mapper43.irqCounterHighByte = (irq_counter & 0xFF00) >> 8;
	state->extraData.mapper43.irqCounterEnabled = irq_enabled;
}

static void map43_setstate(SnssMapperBlock *state)
{
	irq_counter = state->extraData.mapper43.irqCounterLowByte;
	irq_counter |= state->extraData.mapper43.irqCounterHighByte << 8;
	irq_enabled = state->extraData.mapper43.irqCounterEnabled;
}

mapintf_t map43_intf =
{
   43, /* mapper number */
   "map43", /* mapper name */
   map43_Reset, /* init routine */
   NULL, /* vblank callback */
   map43_HSync, /* hblank callback */
   map43_getstate, /* get state (snss) */
   map43_setstate, /* set state (snss) */
   map43_MemoryReadLow,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   map43_MemoryWriteLow,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map43_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};
