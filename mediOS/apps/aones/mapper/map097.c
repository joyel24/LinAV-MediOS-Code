#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

/////////////////////////////////////////////////////////////////////
// Mapper 97
static void map97_Reset()
{
	// set CPU bank pointers
	mmc_bankromALL4(num_8k_ROM_banks-2,num_8k_ROM_banks-1,0,1);

	// set PPU bank pointers ?
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/
}

static void map97_MemoryWrite(uint32 addr, uint8 data)
{
	if(addr < 0xC000)
	{
		uint8 prg_bank = data & 0x0F;

		/*set_CPU_bank6(prg_bank*2+0);
		set_CPU_bank7(prg_bank*2+1);*/
		mmc_bankrom(16,0xC000,prg_bank);
		

		if((data & 0x80) == 0)
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
/////////////////////////////////////////////////////////////////////
static void map97_getstate(SnssMapperBlock *state)
{

}


static void map97_setstate(SnssMapperBlock *state)
{

}


mapintf_t map97_intf =
{
   97, /* mapper number */
   "Map97", /* mapper name */
   map97_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map97_getstate, /* get state (snss) */
   map97_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map97_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

