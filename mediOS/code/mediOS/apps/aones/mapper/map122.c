#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static int patch;

/////////////////////////////////////////////////////////////////////
// Mapper 122
static void map122_Reset()
{
	// set CPU bank pointers
	mmc_bankromALL4(0,1,2,3);
	patch=0;
	if(mmc_getinfo()->var.crc == 0x079eca27e)		//AtlantisNoNazo
		patch = 1;
}


static void map122_MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
	if(patch){
		int h = (data&0x20)>>2;
		int l = ((data&2)<<2) | (data&4);
		mmc_bankvromALL8(l, l+1, l+2, l+3, h, h+1, h+2, h+3);
		return;
	}
	if(addr == 0x6000)
	{
		uint8 chr_bank0 = data & 0x07;
		uint8 chr_bank1 = (data & 0x70) >> 4;

		//mmc_bankvrom0(4,0x0000,chr_bank0);		
		mmc_bankvrom0(chr_bank0*4);		
		mmc_bankvrom1(chr_bank0*4+1);
		mmc_bankvrom2(chr_bank0*4+2);
		mmc_bankvrom3(chr_bank0*4+3);
		//mmc_bankvrom4(4,0x1000,chr_bank1);		
		mmc_bankvrom4(chr_bank1*4+0);
		mmc_bankvrom5(chr_bank1*4+1);
		mmc_bankvrom6(chr_bank1*4+2);
		mmc_bankvrom7(chr_bank1*4+3);
	}
}

/////////////////////////////////////////////////////////////////////


static void map122_getstate(SnssMapperBlock *state)
{	
}


static void map122_setstate(SnssMapperBlock *state)
{
}


mapintf_t map122_intf =
{
   122, /* mapper number */
   "Map122", /* mapper name */
   map122_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map122_getstate, /* get state (snss) */
   map122_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   map122_MemoryWriteSaveRAM,  /*Write saveram*/
   NULL,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

