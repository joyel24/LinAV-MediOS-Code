#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 regs[2];

/////////////////////////////////////////////////////////////////////
// Mapper 75
static void map75_Reset()
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/

	regs[0] = 0;
	regs[1] = 1;
}

static void map75_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr & 0xF000)
	{
	case 0x8000:
		{
			mmc_bankrom4(data);
		}
		break;

	case 0x9000:
		{
			if(data & 0x01)
			{
				//set_mirroring(NES_PPU::MIRROR_HORIZ);
				ppu_mirror(0,0,1,1);
			}
			else
			{
				//set_mirroring(NES_PPU::MIRROR_VERT);
				ppu_mirror(0,1,0,1);
			}
			regs[0] = (regs[0] & 0x0F) | ((data & 0x02) << 3);
			mmc_bankvrom(4,0x0000,regs[0]);			
			regs[1] = (regs[1] & 0x0F) | ((data & 0x04) << 2);
			mmc_bankvrom(4,0x1000,regs[1]);			
		}
		break;

	case 0xA000:
		{
			mmc_bankrom5(data);
		}
		break;

	case 0xC000:
		{
			mmc_bankrom6(data);
		}
		break;

	case 0xE000:
		{
			regs[0] = (regs[0] & 0x10) | (data & 0x0F);
			mmc_bankvrom(4,0x0000,regs[0]);			
		}
		break;

	case 0xF000:
		{
			regs[1] = (regs[1] & 0x10) | (data & 0x0F);
			mmc_bankvrom(4,0x1000,regs[1]);			
		}
		break;
	}
}
/////////////////////////////////////////////////////////////////////

static void map75_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper75.lastE000Write = regs[0];
	state->extraData.mapper75.lastF000Write = regs[1];
}


static void map75_setstate(SnssMapperBlock *state)
{
	regs[0] = state->extraData.mapper75.lastE000Write;
	regs[1] = state->extraData.mapper75.lastF000Write;
}


mapintf_t map75_intf =
{
   72, /* mapper number */
   "map75", /* mapper name */
   map75_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map75_getstate, /* get state (snss) */
   map75_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map75_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};


