#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;


/////////////////////////////////////////////////////////////////////
// Mapper 78
static void map78_Reset()
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/
}

static void map78_MemoryWrite(uint32 addr, uint8 data)
{
	uint8 prg_bank = data & 0x0F;
	uint8 chr_bank = (data & 0xF0) >> 4;

	mmc_bankrom4(prg_bank*2+0);
	mmc_bankrom5(prg_bank*2+1);

	mmc_bankvrom(8,0x0000,chr_bank);	

	// added by rinao
	if((addr & 0xFE00) != 0xFE00)
	{
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

static void map78_getstate(SnssMapperBlock *state)
{

}


static void map78_setstate(SnssMapperBlock *state)
{

}


mapintf_t map78_intf =
{
   78, /* mapper number */
   "map78", /* mapper name */
   map78_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map78_getstate, /* get state (snss) */
   map78_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map78_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};


