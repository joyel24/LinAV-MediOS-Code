#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

/////////////////////////////////////////////////////////////////////
// Mapper 93
static void map93_Reset()
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
	// set PPU bank pointers
	//set_PPU_banks(0,1,2,3,4,5,6,7);
}

static void map93_MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
	if(addr == 0x6000)
	{
		/*set_CPU_bank4(data*2+0);
		set_CPU_bank5(data*2+1);*/
		mmc_bankrom(16,0x8000,data);
	}
}

/////////////////////////////////////////////////////////////////////

static void map93_getstate(SnssMapperBlock *state)
{

}


static void map93_setstate(SnssMapperBlock *state)
{

}


mapintf_t map93_intf =
{
   93, /* mapper number */
   "Map93", /* mapper name */
   map93_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map93_getstate, /* get state (snss) */
   map93_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   map93_MemoryWriteSaveRAM,  /*Write saveram*/
   NULL,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

