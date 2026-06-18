
#include "datatypes.h"
#include "unes_mapper.h"
#include "unes_ppu.h"
//#include <log.h>

/* mapper 7: AOROM */
static void map7_write(uint32 address, uint8 value)
{
   int mirror;      
   //UNUSED(address);
   
   mmc_bankrom(32, 0x8000, value);
   mirror = (value >> 4) &0x1;
   ppu_mirror(mirror, mirror, mirror, mirror);
}

static void map7_init(void)
{
   mmc_bankrom(32, 0x8000, 0);
}

mapintf_t map7_intf =
{
   7, /* mapper number */
   "AOROM", /* mapper name */
   map7_init, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   NULL, /* get state (snss) */
   NULL, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map7_write,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};

