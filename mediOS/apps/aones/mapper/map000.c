#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static void map0_Reset(void)
{
// set CPU bank pointers
	if(num_8k_ROM_banks > 2)
	{
		mmc_bankromALL4(0,1,2,3);
	}
	else if(num_8k_ROM_banks > 1)
	{
		mmc_bankromALL4(0,1,0,1);
	}
	else
	{
		mmc_bankromALL4(0,0,0,0);
	}

	if(num_1k_VROM_banks)
	{
		mmc_bankvromALL8(0,1,2,3,4,5,6,7);
	}
}

mapintf_t map0_intf =
{
   0, /* mapper number */
   "None", /* mapper name */
   map0_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   NULL, /* get state (snss) */
   NULL, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   NULL,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};


