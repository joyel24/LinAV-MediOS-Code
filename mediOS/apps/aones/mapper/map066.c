#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"


/////////////////////////////////////////////////////////////////////
// Mapper 66
static void map66_Reset()
{
	// set CPU bank pointers
	/*set_CPU_banks(0,1,2,3);
	set_PPU_banks(0,1,2,3,4,5,6,7);*/
	mmc_bankrom(32,0x8000,0);

	if(mmc_getinfo()->var.crc == 0xe30552db) // Paris-Dakar Rally Special
	{
		mmc_getinfo()->var.frame_irq_disenabled = 1;
	}
}

static void map66_MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
	uint8 chr_bank = data & 0x0F;
	uint8 prg_bank = (data & 0xF0) >> 4;

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
	mmc_bankvrom(8,0x0000,chr_bank);
}

static void map66_MemoryWrite(uint32 addr, uint8 data)
{
	uint8 chr_bank = data & 0x0F;
	uint8 prg_bank = (data & 0xF0) >> 4;

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
	mmc_bankvrom(8,0x0000,chr_bank);
}
/////////////////////////////////////////////////////////////////////

static void map66_getstate(SnssMapperBlock *state)
{
}
static void map66_setstate(SnssMapperBlock *state)
{
}

mapintf_t map66_intf =
{
   66, /* mapper number */
   "GNROM", /* mapper name */
   map66_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map66_getstate, /* get state (snss) */
   map66_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   map66_MemoryWriteSaveRAM,  /*Write saveram*/
   map66_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

