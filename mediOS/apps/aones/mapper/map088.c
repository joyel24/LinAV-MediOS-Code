#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8  regs[2];
/////////////////////////////////////////////////////////////////////
// Mapper 88
static void map88_Reset()
{
	//mmc_bankromALL4(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	/*if(num_1k_VROM_banks >= 8)
	{
		mmc_bankvroms(0,1,2,3,4,5,6,7);
	}*/
}

static void map88_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr)
	{
	case 0x8000:
		{
			regs[0] = data;
		}
		break;

	case 0x8001:
		{
			switch(regs[0] & 0x07)
			{
			case 0x00:
				{
					mmc_bankvrom0(data & 0xFE);
					mmc_bankvrom1((data & 0xFE)+1);
				}
				break;

			case 0x01:
				{
					mmc_bankvrom2(data & 0xFE);
					mmc_bankvrom3((data & 0xFE)+1);
				}
				break;

			case 0x02:
				{
					mmc_bankvrom4(data|0x40);
				}
				break;

			case 0x03:
				{
					mmc_bankvrom5(data|0x40);
				}
				break;

			case 0x04:
				{
					mmc_bankvrom6(data|0x40);
				}
				break;

			case 0x05:
				{
					mmc_bankvrom7(data|0x40);
				}
				break;

			case 0x06:
				{
					mmc_bankrom4(data);
				}
				break;

			case 0x07:
				{
					mmc_bankrom5(data);
				}
				break;
			}
		}
		break;

	case 0xC000:
		{
			if(data&0x40)
			{
				ppu_mirror(1,1,1,1);
			}
			else
			{
				ppu_mirror(0,0,0,0);
			}
		}
		break;
	}
}
/////////////////////////////////////////////////////////////////////

static void map88_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper88.last8000Write = regs[0];
}


static void map88_setstate(SnssMapperBlock *state)
{
	regs[0] = state->extraData.mapper88.last8000Write;
}


mapintf_t map88_intf =
{
   88, /* mapper number */
   "Map88", /* mapper name */
   map88_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map88_getstate, /* get state (snss) */
   map88_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map88_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

