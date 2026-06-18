#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

static uint8 prg_bank;
static uint8 chr_bank;

/////////////////////////////////////////////////////////////////////
// Mapper 13
static void map13_Reset(void)
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,2,3);
	mmc_bankrom(32,0x8000,0);

	// set PPU bank pointers
	/*set_VRAM_bank(0, 0);
	set_VRAM_bank(1, 1);
	set_VRAM_bank(2, 2);
	set_VRAM_bank(3, 3);
	set_VRAM_bank(4, 0);
	set_VRAM_bank(5, 1);
	set_VRAM_bank(6, 2);
	set_VRAM_bank(7, 3);*/
	mmc_VRAM_bank(0, 0);
	mmc_VRAM_bank(1, 1);
	mmc_VRAM_bank(2, 2);
	mmc_VRAM_bank(3, 3);
	mmc_VRAM_bank(4, 0);
	mmc_VRAM_bank(5, 1);
	mmc_VRAM_bank(6, 2);
	mmc_VRAM_bank(7, 3);

	//parent_NES->ppu->vram_size = 0x4000;

	prg_bank = chr_bank = 0;
}

static void map13_MemoryWrite(uint32 addr, uint8 data)
{
	prg_bank = (data & 0x30) >> 4;
	chr_bank = data & 0x03;

	/*set_CPU_bank4(prg_bank*4+0);
	set_CPU_bank5(prg_bank*4+1);
	set_CPU_bank6(prg_bank*4+2);
	set_CPU_bank7(prg_bank*4+3);*/
	mmc_bankrom(32,0x8000,prg_bank);

	/*set_VRAM_bank(4, chr_bank * 4 + 0);
	set_VRAM_bank(5, chr_bank * 4 + 1);
	set_VRAM_bank(6, chr_bank * 4 + 2);
	set_VRAM_bank(7, chr_bank * 4 + 3);*/
	mmc_VRAM_bank(4, chr_bank * 4 + 0);
	mmc_VRAM_bank(5, chr_bank * 4 + 1);
	mmc_VRAM_bank(6, chr_bank * 4 + 2);
	mmc_VRAM_bank(7, chr_bank * 4 + 3);
}


/////////////////////////////////////////////////////////////////////

static void map13_getstate(SnssMapperBlock *state)
{	
	state->extraData.mapper13.wramBank = chr_bank;
}
static void map13_setstate(SnssMapperBlock *state)
{
	chr_bank = state->extraData.mapper13.wramBank;	
	mmc_VRAM_bank(4, chr_bank * 4 + 0);
	mmc_VRAM_bank(5, chr_bank * 4 + 1);
	mmc_VRAM_bank(6, chr_bank * 4 + 2);
	mmc_VRAM_bank(7, chr_bank * 4 + 3);	
}

mapintf_t map13_intf =
{
   13, /* mapper number */
   "map13", /* mapper name */
   map13_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map13_getstate, /* get state (snss) */
   map13_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map13_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};

