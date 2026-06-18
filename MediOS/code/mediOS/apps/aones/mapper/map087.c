#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;


/////////////////////////////////////////////////////////////////////
// Mapper 87
static void map87_Reset()
{
	// set CPU bank pointers
	mmc_bankromALL4(0,1,2,3);

	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/
}

static void map87_MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
	if(addr == 0x6000)
	{
		uint8 chr_bank = (data & 0x02) >> 1;
		mmc_bankvrom(8,0x0000,chr_bank);		
	}
}
/////////////////////////////////////////////////////////////////////


static void map87_getstate(SnssMapperBlock *state)
{

}


static void map87_setstate(SnssMapperBlock *state)
{

}


mapintf_t map87_intf =
{
   87, /* mapper number */
   "Map87", /* mapper name */
   map87_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map87_getstate, /* get state (snss) */
   map87_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   map87_MemoryWriteSaveRAM,  /*Write saveram*/
   NULL,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

