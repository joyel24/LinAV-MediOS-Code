#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;


/////////////////////////////////////////////////////////////////////
// Mapper 86
static void map86_Reset()
{
	// set CPU bank pointers
	mmc_bankromALL4(0,1,2,3);
	mmc_bankvromALL8(0,1,2,3,4,5,6,7);
}

static void map86_MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
	if(addr == 0x6000)
	{
		uint8 chr_bank = (data & 0x03) | (data & 0x40) >> 4;
		uint8 prg_bank = (data & 0x30) >> 4;

		mmc_bankrom(32,0x8000,prg_bank);
		

		mmc_bankvrom(8,0x0000,chr_bank);
		
	}
}
/////////////////////////////////////////////////////////////////////


static void map86_getstate(SnssMapperBlock *state)
{

}


static void map86_setstate(SnssMapperBlock *state)
{

}


mapintf_t map86_intf =
{
   86, /* mapper number */
   "map86", /* mapper name */
   map86_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map86_getstate, /* get state (snss) */
   map86_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   map86_MemoryWriteSaveRAM,  /*Write saveram*/
   NULL,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};
