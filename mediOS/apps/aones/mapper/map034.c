#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

/////////////////////////////////////////////////////////////////////
// Mapper 34
static void map34_Reset()
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/
}

static void map34_MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
	switch(addr)
	{
	case 0x7FFD:
		{
			//set_CPU_banks(data*4,data*4+1,data*4+2,data*4+3);
			mmc_bankrom(32,0x8000,data);
		}
		break;

	case 0x7FFE:
		{
			/*set_PPU_bank0(data*4+0);
			set_PPU_bank1(data*4+1);
			set_PPU_bank2(data*4+2);
			set_PPU_bank3(data*4+3);*/
			mmc_bankvrom(4,0x0000,data);
		}
		break;

	case 0x7FFF:
		{
			/*set_PPU_bank4(data*4+0);
			set_PPU_bank5(data*4+1);
			set_PPU_bank6(data*4+2);
			set_PPU_bank7(data*4+3);*/
			mmc_bankvrom(4,0x1000,data);
		}
		break;
	}
}

static void map34_MemoryWrite(uint32 addr, uint8 data)
{
	//set_CPU_banks(data*4,data*4+1,data*4+2,data*4+3);
	mmc_bankrom(32,0x8000,data);
}
/////////////////////////////////////////////////////////////////////


static void map34_getstate(SnssMapperBlock *state)
{
}

static void map34_setstate(SnssMapperBlock *state)
{
}

mapintf_t map34_intf =
{
   34, /* mapper number */
   "Nina-1", /* mapper name */
   map34_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map34_getstate, /* get state (snss) */
   map34_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   map34_MemoryWriteSaveRAM,  /*Write saveram*/
   map34_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};

