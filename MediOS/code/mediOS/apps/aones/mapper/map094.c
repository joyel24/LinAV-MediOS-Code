#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

/////////////////////////////////////////////////////////////////////
// Mapper 94
static void map94_Reset()
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
}

static void map94_MemoryWrite(uint32 addr, uint8 data)
{
	if((addr & 0xFFF0) == 0xFF00)
	{
		uint8 bank = (data & 0x1C) >> 2;
		/*set_CPU_bank4(bank*2+0);
		set_CPU_bank5(bank*2+1);*/
		mmc_bankrom(16,0x8000,bank);
	}
	//  LOG("W " << HEX(addr,4) << HEX(data,2)<< endl);
}

/////////////////////////////////////////////////////////////////////


static void map94_getstate(SnssMapperBlock *state)
{

}


static void map94_setstate(SnssMapperBlock *state)
{

}


mapintf_t map94_intf =
{
   94, /* mapper number */
   "Map94", /* mapper name */
   map94_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map94_getstate, /* get state (snss) */
   map94_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map94_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

