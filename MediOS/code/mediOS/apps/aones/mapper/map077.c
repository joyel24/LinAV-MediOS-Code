#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;


/////////////////////////////////////////////////////////////////////
// Mapper 77
static void map77_Reset()
{
	// set CPU bank pointers
	mmc_bankromALL4(0,1,2,3);

	// for Napoleon Senki
	//parent_NES->ppu->vram_write_protect = 0;
}

static void map77_MemoryWrite(uint32 addr, uint8 data)
{
	uint8 prg_bank = data & 0x07;
	uint8 chr_bank = (data & 0xF0) >> 4;

	mmc_bankrom(32,0x8000,prg_bank);	

	mmc_bankvrom0(chr_bank*2+0);
	mmc_bankvrom1(chr_bank*2+1);
}
/////////////////////////////////////////////////////////////////////

static void map77_getstate(SnssMapperBlock *state)
{

}


static void map77_setstate(SnssMapperBlock *state)
{

}


mapintf_t map77_intf =
{
   77, /* mapper number */
   "map77", /* mapper name */
   map77_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map77_getstate, /* get state (snss) */
   map77_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map77_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};


