#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"


/* mapper 2: UNROM */

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static void map2_Reset(void)
{
//	mmc_bankromALL4(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
	int i=0;
	i++;
}

static void map2_MemoryWrite(uint32 addr, uint8 data)
{
	data&=(num_8k_ROM_banks-1);
    mmc_bankrom(16, 0x8000, data);
}
mapintf_t map2_intf =
{
   2, /* mapper number */
   "UNROM", /* mapper name */
   map2_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   NULL, /* get state (snss) */
   NULL, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map2_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

