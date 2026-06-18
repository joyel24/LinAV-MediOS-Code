#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;


/////////////////////////////////////////////////////////////////////
// Mapper 72 (Created by rinao)
static void map72_Reset()
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/
}

static void map72_MemoryWrite(uint32 addr, uint8 data)
{
	uint8 bank = data & 0x0f;
	if (data & 0x80)
	{
		mmc_bankromALL4(bank*2, bank*2+1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
	}
	if (data & 0x40)
	{
		mmc_bankvrom(8,0x0000,bank);
	}
}
/////////////////////////////////////////////////////////////////////

static void map72_getstate(SnssMapperBlock *state)
{

}


static void map72_setstate(SnssMapperBlock *state)
{

}


mapintf_t map72_intf =
{
   72, /* mapper number */
   "map72", /* mapper name */
   map72_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map72_getstate, /* get state (snss) */
   map72_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map72_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};


