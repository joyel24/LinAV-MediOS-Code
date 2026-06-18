#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 regs[1];
/////////////////////////////////////////////////////////////////////
// Mapper 76
static void map76_Reset()
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	/*if(num_1k_VROM_banks >= 8)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/
}

static void map76_MemoryWrite(uint32 addr, uint8 data)
{
	if(addr == 0x8000)
	{
		regs[0] = data;
	}
	else if(addr == 0x8001)
	{
		switch(regs[0] & 0x07)
		{
		case 0x02:
			{
				mmc_bankvrom0(data*2+0);
				mmc_bankvrom1(data*2+1);
			}
			break;

		case 0x03:
			{
				mmc_bankvrom2(data*2+0);
				mmc_bankvrom3(data*2+1);
			}
			break;

		case 0x04:
			{
				mmc_bankvrom4(data*2+0);
				mmc_bankvrom5(data*2+1);
			}
			break;

		case 0x05:
			{
				mmc_bankvrom6(data*2+0);
				mmc_bankvrom7(data*2+1);
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
}
/////////////////////////////////////////////////////////////////////
static void map76_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper76.last8000Write = regs[0];
}


static void map76_setstate(SnssMapperBlock *state)
{
	regs[0] = state->extraData.mapper76.last8000Write;
}


mapintf_t map76_intf =
{
   76, /* mapper number */
   "map76", /* mapper name */
   map76_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map76_getstate, /* get state (snss) */
   map76_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map76_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};


