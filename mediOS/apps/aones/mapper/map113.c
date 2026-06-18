#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

/////////////////////////////////////////////////////////////////////
// Mapper 113
static void map113_Reset()
{
	// set CPU bank pointers
	mmc_bankromALL4(0,1,2,3);

	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		mmc_bankvromALL8(0,1,2,3,4,5,6,7);
	}*/
}

static void map113_MemoryWriteLow(uint32 addr, uint8 data)
{
	switch(addr)
	{
	case 0x4100:
	case 0x4111:
	case 0x4120:
	case 0x4900:
		{
			uint8 prg_bank, chr_bank;

			prg_bank = data >> 3;
			if (num_8k_ROM_banks <= 8 && num_1k_VROM_banks == 8*16)
			{
				chr_bank = ((data >> 3) & 0x08) + (data & 0x07);
			}
			else
			{
				chr_bank = data & 0x07;
			}
			//uint8 prg_bank = (data & 0x03) | ((data & 0x80) >> 5);
			//uint8 chr_bank = (data & 0x70) >> 4;

			mmc_bankrom(32,0x8000,prg_bank);			

			mmc_bankvrom(8,0x0000,chr_bank);			
		}
		break;
	}
}

static void map113_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr)
	{
	case 0x8008:
	case 0x8009:
		{
			uint8 prg_bank, chr_bank;

			prg_bank = data >> 3;
			if (num_8k_ROM_banks <= 8 && num_1k_VROM_banks == 8*16)
			{
				chr_bank = ((data >> 3) & 0x08) + (data & 0x07);
			}
			else
			{
				chr_bank = data & 0x07;
			}
			//uint8 prg_bank = (data & 0x03) | ((data & 0x80) >> 5);
			//uint8 chr_bank = (data & 0x70) >> 4;

			mmc_bankrom(32,0x8000,prg_bank);			
			mmc_bankvrom(8,0x0000,chr_bank);			
		}
		break;
	}
}
/////////////////////////////////////////////////////////////////////

static void map113_getstate(SnssMapperBlock *state)
{
	
}


static void map113_setstate(SnssMapperBlock *state)
{
	
}


mapintf_t map113_intf =
{
   113, /* mapper number */
   "Map113", /* mapper name */
   map113_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map113_getstate, /* get state (snss) */
   map113_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   map113_MemoryWriteLow,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map113_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

