#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

/////////////////////////////////////////////////////////////////////
// Mapper 11
static void map11_Reset(void)
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,2,3);
	mmc_bankrom(32,0x8000,0);

	// set PPU bank pointers
	//set_PPU_banks(0,1,2,3,4,5,6,7);

	//set_mirroring(NES_PPU::MIRROR_VERT);
	ppu_mirror(0,1,0,1);
}

static void map11_MemoryWrite(uint32 addr, uint8 data)
{
	uint8 prg_bank = data & 0x01;
	uint8 chr_bank = (data & 0x70) >> 4;

	/*set_CPU_bank4(prg_bank*4+0);
	set_CPU_bank5(prg_bank*4+1);
	set_CPU_bank6(prg_bank*4+2);
	set_CPU_bank7(prg_bank*4+3);*/
	mmc_bankrom(32,0x8000,prg_bank);

	/*set_PPU_bank0(chr_bank*8+0);
	set_PPU_bank1(chr_bank*8+1);
	set_PPU_bank2(chr_bank*8+2);
	set_PPU_bank3(chr_bank*8+3);
	set_PPU_bank4(chr_bank*8+4);
	set_PPU_bank5(chr_bank*8+5);
	set_PPU_bank6(chr_bank*8+6);
	set_PPU_bank7(chr_bank*8+7);*/
	mmc_bankvrom(8,0,chr_bank);
}
/////////////////////////////////////////////////////////////////////

mapintf_t map11_intf =
{
   11, /* mapper number */
   "Color Dreams", /* mapper name */
   map11_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   NULL, /* get state (snss) */
   NULL, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map11_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};
