#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

/////////////////////////////////////////////////////////////////////
// Mapper 99
static void map99_Reset()
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

	// set VROM bank
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/
}

static void map99_WriteHighRegs(uint32 addr, uint8 data)
{
	if(addr == 0x4016)
	{
		if(data & 0x04)
		{
			mmc_bankvromALL8(8,9,10,11,12,13,14,15);
		}
		else
		{
			mmc_bankvromALL8(0,1,2,3,4,5,6,7);
		}
	}
}
/////////////////////////////////////////////////////////////////////

static void map99_getstate(SnssMapperBlock *state)
{

}


static void map99_setstate(SnssMapperBlock *state)
{

}


mapintf_t map99_intf =
{
   99, /* mapper number */
   "Map99", /* mapper name */
   map99_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map99_getstate, /* get state (snss) */
   map99_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   map99_WriteHighRegs,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   NULL,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

