

/////////////////////////////////////////////////////////////////////
// Mapper 101
#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static void map101_Reset()
{
	// set CPU bank pointers
	mmc_bankromALL4(0,1,2,3);
	mmc_bankvromALL8(0,1,2,3,4,5,6,7);
}

static void map101_MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
	data &= 0x03;
	mmc_bankvrom(8,0x0000,data);	
}

static void map101_MemoryWrite(uint32 addr, uint8 data)
{
	data &= 0x03;
	mmc_bankvrom(8,0x0000,data);		
}
/////////////////////////////////////////////////////////////////////


static void map101_getstate(SnssMapperBlock *state)
{

}


static void map101_setstate(SnssMapperBlock *state)
{
	
}


mapintf_t map101_intf =
{
   101, /* mapper number */
   "Map101", /* mapper name */
   map101_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map101_getstate, /* get state (snss) */
   map101_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   map101_MemoryWriteSaveRAM,  /*Write saveram*/
   map101_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

