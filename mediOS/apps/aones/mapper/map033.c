#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 patch;
static uint8 patch2;
static uint8 irq_enabled;
static uint8 irq_counter;
/////////////////////////////////////////////////////////////////////
// Mapper 33
static void map33_Reset()
{
	patch = patch2 = 0;

	if(mmc_getinfo()->var.crc == 0x5e9bc161 || // Akira
	        mmc_getinfo()->var.crc == 0xecdbafa4 || // Bakushou!! Jinsei Gekijou
	        mmc_getinfo()->var.crc == 0x59cd0c31 || // Don Doko Don
	        mmc_getinfo()->var.crc == 0x837c1342 || // Golf Ko Open
	        mmc_getinfo()->var.crc == 0x42d893e4 || // Operation Wolf
	        mmc_getinfo()->var.crc == 0x07ee6d8f || // Power Blazer
	        mmc_getinfo()->var.crc == 0x5193fb54 )  // Takeshi no Sengoku Fuuunji
	{
		patch = 1;
	}
	if(mmc_getinfo()->var.crc == 0x63bb86b5)   // Jetsons-Cogswell'sCaper!,The(J)
	{
		patch2=1;
	}

	// set CPU bank pointers
	mmc_bankromALL4(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
/*	if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/

	irq_enabled = 0;
	irq_counter = 0;
}

static void map33_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr)
	{
	case 0x8000:
		{
			if(patch) // old #33 games
			{
				if(data & 0x40)
				{
					ppu_mirror(0,0,1,1);
				}
				else
				{
					ppu_mirror(0,1,0,1);
				}
				mmc_bankrom4(data & 0x1F);
			}
			else
			{
				mmc_bankrom4(data);
			}
		}
		break;

	case 0x8001:
		{
			if(patch) // old #33 games
			{
				mmc_bankrom5(data & 0x1F);
			}
			else
			{
				mmc_bankrom5(data);
			}
		}
		break;

	case 0x8002:
		{
			mmc_bankvrom0(data*2+0);
			mmc_bankvrom1(data*2+1);
		}
		break;

	case 0x8003:
		{
			mmc_bankvrom2(data*2+0);
			mmc_bankvrom3(data*2+1);
		}
		break;

	case 0xA000:
		{
			mmc_bankvrom4(data);
		}
		break;

	case 0xA001:
		{
			mmc_bankvrom5(data);
		}
		break;

	case 0xA002:
		{
			mmc_bankvrom6(data);
		}
		break;

	case 0xA003:
		{
			mmc_bankvrom7(data);
		}
		break;

	case 0xC000:
		{
			irq_counter = data;
		}
		break;

	case 0xC001:
	case 0xC002:
	case 0xE001:
	case 0xE002:
		{
			irq_enabled = data;
		}
		break;

	case 0xE000:
		{
			if (data & 0x40)
			{
				ppu_mirror(0,0,1,1);
			}
			else
			{
				ppu_mirror(0,1,0,1);
			}
		}
		break;
	}
	//  LOG("MAP33: "<< HEX(addr,4) << "  " << HEX(data,2) << endl);
}

static void map33_HSync(uint32 scanline)
{
	if(irq_enabled)
	{
		//if(parent_NES->ppu->spr_enabled() || parent_NES->ppu->bg_enabled())
		if (ppu_enabled())
		{
			if (scanline >= 0 && scanline <= 239)
			{
				if(irq_counter == 0)
				{
					if(patch2)
					{
						//parent_NES->cpu->DoIRQ();
						doIRQ();
					}
					else
					{
						//parent_NES->cpu->DoPendingIRQ(); //DoIRQ()
						PendingIRQ();
					}
					irq_counter = 0;
					irq_enabled = 0;
				}
				else
				{
					irq_counter++;
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////


static void map33_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper33.irqCounter = irq_counter;
	state->extraData.mapper33.irqCounterEnabled = irq_enabled;

}


static void map33_setstate(SnssMapperBlock *state)
{
	irq_counter = state->extraData.mapper33.irqCounter;
	irq_enabled = state->extraData.mapper33.irqCounterEnabled;
}


mapintf_t map33_intf =
{
   33, /* mapper number */
   "map33", /* mapper name */
   map33_Reset, /* init routine */
   NULL, /* vblank callback */
   map33_HSync, /* hblank callback */
   map33_getstate, /* get state (snss) */
   map33_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map33_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

