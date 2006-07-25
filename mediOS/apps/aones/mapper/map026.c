#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;


static uint8 irq_enabled;
static uint8 irq_counter;
static uint8 irq_latch;
/////////////////////////////////////////////////////////////////////
// Mapper 26
static void map26_Reset()
{
	// Init ExSound
	//parent_NES->apu->SelectExSound(1);
	//exsound apu_setexchip(1);


	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/

	irq_enabled = 0;
	irq_counter = 0;
}

static void map26_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr)
	{
	case 0x8000:
		{
			mmc_bankrom4(data*2+0);
			mmc_bankrom5(data*2+1);
		}
		break;

	case 0xB003:
		{
			data = data & 0x7F;
			if(data == 0x08 || data == 0x2C)
			{
				ppu_mirror(1,1,1,1);
			}
			else if(data == 0x20)
			{
				//set_mirroring(NES_PPU::MIRROR_VERT);
				ppu_mirror(0,1,0,1);
			}
			else if(data == 0x24)
			{
				//set_mirroring(NES_PPU::MIRROR_HORIZ);
				ppu_mirror(0,0,1,1);
			}
			else if(data == 0x28)
			{
				ppu_mirror(0,0,0,0);
			}
		}
		break;

	case 0xC000:
		{
			mmc_bankrom6(data);
		}
		break;

	case 0xD000:
		{
			mmc_bankvrom0(data);
		}
		break;

	case 0xD001:
		{
			mmc_bankvrom2(data);
		}
		break;

	case 0xD002:
		{
			mmc_bankvrom1(data);
		}
		break;

	case 0xD003:
		{
			mmc_bankvrom3(data);
		}
		break;

	case 0xE000:
		{
			mmc_bankvrom4(data);
		}
		break;

	case 0xE001:
		{
			mmc_bankvrom6(data);
		}
		break;

	case 0xE002:
		{
			mmc_bankvrom5(data);
		}
		break;

	case 0xE003:
		{
			mmc_bankvrom7(data);
		}
		break;

	case 0xF000:
		{
			irq_latch = data;
		}
		break;

	case 0xF001:
		{
			irq_enabled = data & 0x01;
		}
		break;

	case 0xF002:
		{
			irq_enabled = data & 0x03;
			if(irq_enabled & 0x02)
			{
				irq_counter = irq_latch;
			}
		}
		break;
	}

	addr = (addr & 0xfffc) | ((addr & 1) << 1) | ((addr & 2) >> 1);
	//parent_NES->apu->ExWrite(addr, data);
	//exsound ex_write(addr,data);
}

static void map26_HSync(uint32 scanline)
{
	if(irq_enabled & 0x03)
	{
		if(irq_counter >= 0xFE)
		{
			//parent_NES->cpu->DoIRQ();
			doIRQ();
			irq_counter = irq_latch;
			irq_enabled = 0;
		}
		else
		{
			irq_counter++;
		}
	}
}
/////////////////////////////////////////////////////////////////////
static void map26_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper26.irqCounter = irq_counter;
	state->extraData.mapper26.irqCounterEnabled = irq_enabled;
	state->extraData.mapper26.irqLatchCounter = irq_latch;
}


static void map26_setstate(SnssMapperBlock *state)
{
	irq_counter = state->extraData.mapper26.irqCounter;
	irq_enabled = state->extraData.mapper26.irqCounterEnabled;
	irq_latch = state->extraData.mapper26.irqLatchCounter;
}


mapintf_t map26_intf =
{
   26, /* mapper number */
   "Map26", /* mapper name */
   map26_Reset, /* init routine */
   NULL, /* vblank callback */
   map26_HSync, /* hblank callback */
   map26_getstate, /* get state (snss) */
   map26_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map26_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

