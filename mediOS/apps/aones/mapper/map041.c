#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 regs[1];
/////////////////////////////////////////////////////////////////////
// Mapper 41
static void map41_Reset()
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,2,3);
	mmc_bankromALL4(0,1,2,3);

	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/
}

static void map41_MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
	if(addr < 0x6800)
	{
		regs[0] = (uint8)(addr & 0xFF);

		/*set_CPU_bank4((regs[0] & 0x07)*4+0);
		set_CPU_bank5((regs[0] & 0x07)*4+1);
		set_CPU_bank6((regs[0] & 0x07)*4+2);
		set_CPU_bank7((regs[0] & 0x07)*4+3);*/
		mmc_bankrom(32,0x8000,(regs[0] & 0x07));
		if(regs[0] & 0x20)
		{
			//set_mirroring(NES_PPU::MIRROR_HORIZ);
			ppu_mirror(0,0,1,1);
		}
		else
		{
			//set_mirroring(NES_PPU::MIRROR_VERT);
			ppu_mirror(0,1,0,1);
		}
	}
}

static void map41_MemoryWrite(uint32 addr, uint8 data)
{
	if(regs[0] & 0x04)
	{
		uint8 chr_bank = ((regs[0] & 0x18) >> 1) | (data & 0x03);

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
}
/////////////////////////////////////////////////////////////////////

static void map41_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper41.last6000Write = regs[0];
}

static void map41_setstate(SnssMapperBlock *state)
{
	regs[0] = state->extraData.mapper41.last6000Write;
}

mapintf_t map41_intf =
{
   41, /* mapper number */
   "map41", /* mapper name */
   map41_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map41_getstate, /* get state (snss) */
   map41_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   map41_MemoryWriteSaveRAM,  /*Write saveram*/
   map41_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};
