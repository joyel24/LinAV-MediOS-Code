#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 vbank0,vbank1;
/////////////////////////////////////////////////////////////////////
// Mapper 96


static void map96_sync_PPU_banks(void)
{
	mmc_VRAM_bank(0, vbank0 * 16 + vbank1 * 4 + 0);
	mmc_VRAM_bank(1, vbank0 * 16 + vbank1 * 4 + 1);
	mmc_VRAM_bank(2, vbank0 * 16 + vbank1 * 4 + 2);
	mmc_VRAM_bank(3, vbank0 * 16 + vbank1 * 4 + 3);
	mmc_VRAM_bank(4, vbank0 * 16 + 12);
	mmc_VRAM_bank(5, vbank0 * 16 + 13);
	mmc_VRAM_bank(6, vbank0 * 16 + 14);
	mmc_VRAM_bank(7, vbank0 * 16 + 15);
}


static void map96_PPU_Latch_Address(uint32 addr)
{
	if((addr & 0xF000) == 0x2000)
	{
		vbank1 = (addr & 0x0300) >> 8;
		map96_sync_PPU_banks();
	}
}



static void map96_Reset()
{
	// set CPU bank pointers
	mmc_bankromALL4(0,1,2,3);

	// set PPU bank pointers
	vbank0 = vbank1 = 0;
	map96_sync_PPU_banks();
	//parent_NES->ppu->vram_size = 0x8000;
	mmc_getinfo()->vramsize = 0x8000;

	//set_mirroring(0,0,0,0);
	ppu_mirror(0,0,0,0);
	
	ppu_set_Latch_Address(map96_PPU_Latch_Address);
}


static void map96_MemoryWrite(uint32 addr, uint8 data)
{
	/*set_CPU_bank4((data & 0x03) * 4 + 0);
	set_CPU_bank5((data & 0x03) * 4 + 1);
	set_CPU_bank6((data & 0x03) * 4 + 2);
	set_CPU_bank7((data & 0x03) * 4 + 3);*/
	mmc_bankrom(32,0x8000,(data & 0x03));

	vbank0 = (data & 0x04) >> 2;
	map96_sync_PPU_banks();
}


/////////////////////////////////////////////////////////////////////

static void map96_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper96.wramBank = vbank0;
}


static void map96_setstate(SnssMapperBlock *state)
{
	vbank0 = state->extraData.mapper96.wramBank;
}


mapintf_t map96_intf =
{
   96, /* mapper number */
   "Map96", /* mapper name */
   map96_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map96_getstate, /* get state (snss) */
   map96_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map96_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   map96_PPU_Latch_Address,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

