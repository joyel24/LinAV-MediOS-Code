#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 patch;
/////////////////////////////////////////////////////////////////////
// Mapper 70
static void map70_Reset()
{
	patch = 0;

	if(mmc_getinfo()->var.crc == 0xa59ca2ef || // Kamen Rider Club
	        mmc_getinfo()->var.crc == 0x10bb8f9a)   // Family Trainer - Manhattan Police
	{
		patch = 1;
	}

	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
}

static void map70_MemoryWrite(uint32 addr, uint8 data)
{
	uint8 chr_bank = data & 0x0F;
	uint8 prg_bank = (data & 0x70) >> 4;

	/*set_CPU_bank4(prg_bank*2+0);
	set_CPU_bank5(prg_bank*2+1);*/
	mmc_bankrom(16,0x8000,prg_bank);

	/*set_PPU_bank0(chr_bank*8+0);
	set_PPU_bank1(chr_bank*8+1);
	set_PPU_bank2(chr_bank*8+2);
	set_PPU_bank3(chr_bank*8+3);
	set_PPU_bank4(chr_bank*8+4);
	set_PPU_bank5(chr_bank*8+5);
	set_PPU_bank6(chr_bank*8+6);
	set_PPU_bank7(chr_bank*8+7);*/
	mmc_bankvrom(8,0x0000,chr_bank);

	if(patch)
	{
		if(data & 0x80)
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
	else
	{
		if(data & 0x80)
		{
			//set_mirroring(1,1,1,1);
			ppu_mirror(1,1,1,1);
		}
		else
		{
			//set_mirroring(0,0,0,0);
			ppu_mirror(0,0,0,0);
		}
	}
}
/////////////////////////////////////////////////////////////////////

static void map70_getstate(SnssMapperBlock *state)
{
   
}


static void map70_setstate(SnssMapperBlock *state)
{
   
}


mapintf_t map70_intf =
{
   70, /* mapper number */
   "map70", /* mapper name */
   map70_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map70_getstate, /* get state (snss) */
   map70_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map70_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

