#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

/////////////////////////////////////////////////////////////////////
// Mapper 89
static void map89_Reset()
{
	// set CPU bank pointers
	//mmc_bankroms(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
	// set PPU bank pointers
	//mmc_bankvroms(0,1,2,3,4,5,6,7);
}

static void map89_MemoryWrite(uint32 addr, uint8 data)
{
	if((addr & 0xFF00) == 0xC000)
	{
		uint8 prg_bank = (data & 0x70) >> 4;
		uint8 chr_bank = ((data & 0x80) >> 4) | (data & 0x07);

		mmc_bankrom(16,0x8000,prg_bank);		

		mmc_bankvrom(8,0x0000,chr_bank);		

		if (data & 0x08)
		{
			ppu_mirror(1,1,1,1);
		}
		else
		{
			ppu_mirror(0,0,0,0);
		}
	}
}

/////////////////////////////////////////////////////////////////////

static void map89_getstate(SnssMapperBlock *state)
{

}


static void map89_setstate(SnssMapperBlock *state)
{

}


mapintf_t map89_intf =
{
   89, /* mapper number */
   "Map89", /* mapper name */
   map89_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map89_getstate, /* get state (snss) */
   map89_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map89_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

