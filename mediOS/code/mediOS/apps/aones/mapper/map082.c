#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 regs[1];

/////////////////////////////////////////////////////////////////////
// Mapper 82
static void map82_Reset()
{
	// set CPU bank pointers
//	set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
/*	if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/

	// set Mirroring
	//set_mirroring(NES_PPU::MIRROR_VERT);
	ppu_mirror(0,1,0,1);

	regs[0] = 0;
}

static void map82_MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
	switch (addr)
	{
	case 0x7EF0:
		/* Switch 2k VROM at $0000 or $1000 */
		{
			if(regs[0])
			{
				mmc_bankvrom4((data & 0xFE)+0);
				mmc_bankvrom5((data & 0xFE)+1);
			}
			else
			{
				mmc_bankvrom0((data & 0xFE)+0);
				mmc_bankvrom1((data & 0xFE)+1);
			}
		}
		break;

	case 0x7EF1:
		{
			if(regs[0])
			{
				mmc_bankvrom6((data & 0xFE)+0);
				mmc_bankvrom7((data & 0xFE)+1);
			}
			else
			{
				mmc_bankvrom2((data & 0xFE)+0);
				mmc_bankvrom3((data & 0xFE)+1);
			}
		}
		break;

	case 0x7EF2:
		{
			if(!regs[0])
			{
				mmc_bankvrom4(data);
			}
			else
			{
				mmc_bankvrom0(data);
			}
		}
		break;

	case 0x7EF3:
		{
			if(!regs[0])
			{
				mmc_bankvrom5(data);
			}
			else
			{
				mmc_bankvrom1(data);
			}
		}
		break;

	case 0x7EF4:
		{
			if(!regs[0])
			{
				mmc_bankvrom6(data);
			}
			else
			{
				mmc_bankvrom2(data);
			}
		}
		break;

	case 0x7EF5:
		{
			if(!regs[0])
			{
				mmc_bankvrom7(data);
			}
			else
			{
				mmc_bankvrom3(data);
			}
		}
		break;

	case 0x7EF6:
		{
			regs[0] = data & 0x02;
			if(data & 0x01)
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

	case 0x7EFA:
		{
			mmc_bankrom4(data >> 2);
		}
		break;

	case 0x7EFB:
		{
			mmc_bankrom5(data >> 2);
		}
		break;

	case 0x7EFC:
		{
			mmc_bankrom6(data >> 2);
		}
		break;
	}
}
/////////////////////////////////////////////////////////////////////



static void map82_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper82.last7EF6Write = regs[0];
}


static void map82_setstate(SnssMapperBlock *state)
{
	regs[0] = state->extraData.mapper82.last7EF6Write & 0x02;
}


mapintf_t map82_intf =
{
   82, /* mapper number */
   "map82", /* mapper name */
   map82_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map82_getstate, /* get state (snss) */
   map82_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   map82_MemoryWriteSaveRAM,  /*Write saveram*/
   NULL,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};
