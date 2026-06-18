#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 irq_enabled;
static uint8 irq_counter;
static uint8 irq_latch;
static int patch;
/////////////////////////////////////////////////////////////////////
// Mapper 85
static void map85_Reset()
{
	// Init ExSound
	//parent_NES->apu->SelectExSound(2);
	//exsound apu_setexchip(2);

	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	if(num_1k_VROM_banks)
	{
		mmc_bankvromALL8(0,1,2,3,4,5,6,7);
	}
	else
	{
		mmc_VRAM_bank(0, 0);
		mmc_VRAM_bank(1, 0);
		mmc_VRAM_bank(2, 0);
		mmc_VRAM_bank(3, 0);
		mmc_VRAM_bank(4, 0);
		mmc_VRAM_bank(5, 0);
		mmc_VRAM_bank(6, 0);
		mmc_VRAM_bank(7, 0);
	}

	patch = 0;

	if(mmc_getinfo()->var.crc == 0x33CE3FF0)	// lagurange
		patch = 1;

	irq_enabled = 0;
	irq_counter = 0;
	irq_latch = 0;
}

static void map85_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr & 0xF038)
	{
	case 0x8000:
		{
			mmc_bankrom4(data);
		}
		break;

	case 0x8008:
	case 0x8010:
		{
			mmc_bankrom5(data);
		}
		break;

	case 0x9000:
		{
			mmc_bankrom6(data);
		}
		break;

	case 0x9010:
	case 0x9030:
		{
			//exsound ex_write(addr, data);
		}
		break;

	case 0xA000:
		{
			if(num_1k_VROM_banks)
			{
				mmc_bankvrom0(data);
			}
			else
			{
				mmc_VRAM_bank(0, data);
			}
		}
		break;

	case 0xA008:
	case 0xA010:
		{
			if(num_1k_VROM_banks)
			{
				mmc_bankvrom1(data);
			}
			else
			{
				mmc_VRAM_bank(1, data);
			}
		}
		break;

	case 0xB000:
		{
			if(num_1k_VROM_banks)
			{
				mmc_bankvrom2(data);
			}
			else
			{
				mmc_VRAM_bank(2, data);
			}
		}
		break;

	case 0xB008:
	case 0xB010:
		{
			if(num_1k_VROM_banks)
			{
				mmc_bankvrom3(data);
			}
			else
			{
				mmc_VRAM_bank(3, data);
			}
		}
		break;

	case 0xC000:
		{
			if(num_1k_VROM_banks)
			{
				mmc_bankvrom4(data);
			}
			else
			{
				mmc_VRAM_bank(4, data);
			}
		}
		break;

	case 0xC008:
	case 0xC010:
		{
			if(num_1k_VROM_banks)
			{
				mmc_bankvrom5(data);
			}
			else
			{
				mmc_VRAM_bank(5, data);
			}
		}
		break;

	case 0xD000:
		{
			if(num_1k_VROM_banks)
			{
				mmc_bankvrom6(data);
			}
			else
			{
				mmc_VRAM_bank(6, data);
			}
		}
		break;

	case 0xD008:
	case 0xD010:
		{
			if(num_1k_VROM_banks)
			{
				mmc_bankvrom7(data);
			}
			else
			{
				mmc_VRAM_bank(7, data);
			}
		}
		break;

	case 0xE000:
		{
			data &= 0x03;
			if(data == 0x00)
			{
				//set_mirroring(NES_PPU::MIRROR_VERT);
				ppu_mirror(0,1,0,1);
			}
			else if(data == 0x01)
			{
				//set_mirroring(NES_PPU::MIRROR_HORIZ);
				ppu_mirror(0,0,1,1);
			}
			else if(data == 0x02)
			{
				ppu_mirror(0,0,0,0);
			}
			else
			{
				ppu_mirror(1,1,1,1);
			}
		}
		break;

	case 0xE008:
	case 0xE010:
		{
			irq_latch = data;
		}
		break;

	case 0xF000:
		{
			irq_enabled = data & 0x03;
			if(irq_enabled & 0x02)
			{
				irq_counter = irq_latch;
			}
		}
		break;

	case 0xF008:
	case 0xF010:
		{
			irq_enabled = (irq_enabled & 0x01) * 3;
		}
		break;
	}
}

static void map85_HSync(uint32 scanline)
{
	if(irq_enabled & 0x02)
	{
		if(irq_counter == (patch?0:0xFF))
		{
			//parent_NES->cpu->DoIRQ();
			doIRQ();
			irq_counter = irq_latch;
		}
		else
		{
			irq_counter++;
		}
	}
}
/////////////////////////////////////////////////////////////////////


static void map85_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper85.irqCounter = irq_counter;
	state->extraData.mapper85.irqCounterEnabled = irq_enabled;
	state->extraData.mapper85.irqLatchCounter = irq_latch;
}


static void map85_setstate(SnssMapperBlock *state)
{
	irq_counter = state->extraData.mapper85.irqCounter;
	irq_enabled = state->extraData.mapper85.irqCounterEnabled;
	irq_latch = state->extraData.mapper85.irqLatchCounter;
}


mapintf_t map85_intf =
{
   85, /* mapper number */
   "map85", /* mapper name */
   map85_Reset, /* init routine */
   NULL, /* vblank callback */
   map85_HSync, /* hblank callback */
   map85_getstate, /* get state (snss) */
   map85_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map85_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};
