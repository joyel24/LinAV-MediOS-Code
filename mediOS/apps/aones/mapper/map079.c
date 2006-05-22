#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;


/////////////////////////////////////////////////////////////////////
// Mapper 79
static void map79_Reset()
{
	// set CPU bank pointers
	mmc_bankromALL4(0,1,2,3);

	// set PPU bank pointers
	if(num_1k_VROM_banks)
	{
		mmc_bankvromALL8(0,1,2,3,4,5,6,7);
	}
}

static void map79_MemoryWriteLow(uint32 addr, uint8 data)
{
	if(addr & 0x0100)
	{
		uint8 prg_bank = (data & 0x08) >> 3;
		uint8 chr_bank = data & 0x07;

		mmc_bankrom(32,0x8000,prg_bank);		

		mmc_bankvrom(8,0x0000,chr_bank);		
	}
}
/////////////////////////////////////////////////////////////////////

static void map79_getstate(SnssMapperBlock *state)
{

}


static void map79_setstate(SnssMapperBlock *state)
{

}


mapintf_t map79_intf =
{
   79, /* mapper number */
   "map79", /* mapper name */
   map79_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map79_getstate, /* get state (snss) */
   map79_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   map79_MemoryWriteLow,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   NULL,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};



