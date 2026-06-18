#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"
extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

/////////////////////////////////////////////////////////////////////
// Mapper 22
static void map22_Reset()
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
	//set_PPU_banks(0,1,2,3,4,5,6,7);
}

static void map22_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr)
	{
	case 0x8000:
		{
			//set_CPU_bank4(data);
			mmc_bankrom(8,0x8000,data);
		}
		break;

	case 0x9000:
		{
			data &= 0x03;
			if(data == 0)
			{
				//set_mirroring(NES_PPU::MIRROR_VERT);
				ppu_mirror(0,1,0,1);
			}
			else if(data == 1)
			{
				//set_mirroring(NES_PPU::MIRROR_HORIZ);
				ppu_mirror(0,0,1,1);
			}
			else if(data == 2)
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
		break;

	case 0xA000:
		{
			//set_CPU_bank5(data);
			mmc_bankrom(8,0xA000,data);
		}
		break;

	case 0xB000:
		{
			//set_PPU_bank0(data >> 1);
			mmc_bankvrom(1,0x0000,data>>1);
		}
		break;

	case 0xB001:
		{
			//set_PPU_bank1(data >> 1);
			mmc_bankvrom(1,0x0400,data>>1);
		}
		break;

	case 0xC000:
		{
			//set_PPU_bank2(data >> 1);
			mmc_bankvrom(1,0x0800,data>>1);
		}
		break;

	case 0xC001:
		{
			//set_PPU_bank3(data >> 1);
			mmc_bankvrom(1,0x0C00,data>>1);
		}
		break;

	case 0xD000:
		{
			//set_PPU_bank4(data >> 1);
			mmc_bankvrom(1,0x1000,data>>1);
		}
		break;

	case 0xD001:
		{
			//set_PPU_bank5(data >> 1);
			mmc_bankvrom(1,0x1400,data>>1);
		}
		break;

	case 0xE000:
		{
			//set_PPU_bank6(data >> 1);
			mmc_bankvrom(1,0x1800,data>>1);
		}
		break;

	case 0xE001:
		{
			//set_PPU_bank7(data >> 1);
			mmc_bankvrom(1,0x1C00,data>>1);
		}
		break;
	}
}

static void map22_getstate(SnssMapperBlock *state)
{
}
static void map22_setstate(SnssMapperBlock *state)
{
}
/////////////////////////////////////////////////////////////////////

mapintf_t map22_intf =
{
   22, /* mapper number */
   "Konami VRC2 type A", /* mapper name */
   map22_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map22_getstate, /* get state (snss) */
   map22_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map22_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};

