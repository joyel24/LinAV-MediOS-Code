#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 patch;
/////////////////////////////////////////////////////////////////////
// Mapper 80
static void map80_Reset()
{
	patch = 0;

	if(mmc_getinfo()->var.crc == 0x9832d15a)   // Fudou Myouou Den (J).nes
	{
		// for Hudoumyouou Den
		patch = 1;
	}

	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/
}

static void map80_MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
	switch(addr)
	{
	case 0x7EF0:
		{
			mmc_bankvrom0(data & 0x7F);
			mmc_bankvrom1((data & 0x7F)+1);

			if(patch)
			{
				if (data & 0x80)
				{
					mmc_VRAM_bank(0x8, 1);
					mmc_VRAM_bank(0x9, 1);
				}
				else
				{
					mmc_VRAM_bank(0x8, 0);
					mmc_VRAM_bank(0x9, 0);
				}
			}
		}
		break;

	case 0x7EF1:
		{
			mmc_bankvrom2(data & 0x7F);
			mmc_bankvrom3((data & 0x7F)+1);

			if(patch)
			{
				if (data & 0x80)
				{
					mmc_VRAM_bank(0xA, 1);
					mmc_VRAM_bank(0xB, 1);
				}
				else
				{
					mmc_VRAM_bank(0xA, 0);
					mmc_VRAM_bank(0xB, 0);
				}
			}
		}
		break;

	case 0x7EF2:
		{
			mmc_bankvrom4(data);
		}
		break;

	case 0x7EF3:
		{
			mmc_bankvrom5(data);
		}
		break;

	case 0x7EF4:
		{
			mmc_bankvrom6(data);
		}
		break;

	case 0x7EF5:
		{
			mmc_bankvrom7(data);
		}
		break;

	case 0x7EF6:
		{
			if(data & 0x01)
			{
				//set_mirroring(NES_PPU::MIRROR_VERT);
				ppu_mirror(0,1,0,1);
			}
			else
			{
				//set_mirroring(NES_PPU::MIRROR_HORIZ);
				ppu_mirror(0,0,1,1);
			}
		}
		break;

	case 0x7EFA:
	case 0x7EFB:
		{
			mmc_bankrom4(data);
		}
		break;

	case 0x7EFC:
	case 0x7EFD:
		{
			mmc_bankrom5(data);
		}
		break;

	case 0x7EFE:
	case 0x7EFF:
		{
			mmc_bankrom6(data);
		}
		break;
	}
}
/////////////////////////////////////////////////////////////////////


static void map80_getstate(SnssMapperBlock *state)
{

}


static void map80_setstate(SnssMapperBlock *state)
{

}


mapintf_t map80_intf =
{
   80, /* mapper number */
   "map80", /* mapper name */
   map80_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map80_getstate, /* get state (snss) */
   map80_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   map80_MemoryWriteSaveRAM,  /*Write saveram*/
   NULL,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};
