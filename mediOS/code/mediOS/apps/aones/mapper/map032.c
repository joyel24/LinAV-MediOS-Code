#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 patch;
static uint8 regs[1];
/////////////////////////////////////////////////////////////////////
// Mapper 32
static void map32_Reset()
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/

	patch = 0;

	if(mmc_getinfo()->var.crc == 0xc0fed437) // Major League
	{
		ppu_mirror(0,0,0,0);
		patch = 1;
	}

	if(mmc_getinfo()->var.crc == 0xfd3fc292) // Ai Sensei no Oshiete - Watashi no Hoshi
	{
		mmc_bankromALL4(30,31,30,31);
	}

	regs[0] = 0;
}

static void map32_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr & 0xF000)
	{
	case 0x8000:
		{
			if(regs[0] & 0x02)
			{
				mmc_bankrom6(data);
			}
			else
			{
				mmc_bankrom4(data);
			}
		}
		break;

	case 0x9000:
		{
			if(data & 0x01)
			{
				ppu_mirror(0,0,1,1);
			}
			else
			{
				ppu_mirror(0,1,0,1);
			}
			regs[0] = data;
		}
		break;

	case 0xA000:
		{
			mmc_bankrom5(data);
		}
		break;
	}

	switch(addr & 0xF007)
	{
	case 0xB000:
		{
			mmc_bankvrom0(data);
		}
		break;

	case 0xB001:
		{
			mmc_bankvrom1(data);
		}
		break;

	case 0xB002:
		{
			mmc_bankvrom2(data);
		}
		break;

	case 0xB003:
		{
			mmc_bankvrom3(data);
		}
		break;

	case 0xB004:
		{
			mmc_bankvrom4(data);
		}
		break;

	case 0xB005:
		{
			mmc_bankvrom5(data);
		}
		break;

	case 0xB006:
		{
			if ((patch == 1) && (data & 0x40))
			{
				ppu_mirror(0,0,0,1);
			}
			mmc_bankvrom6(data);
		}
		break;

	case 0xB007:
		{
			if ((patch == 1) && (data & 0x40))
			{
				ppu_mirror(0,0,0,0);
			}
			mmc_bankvrom7(data);
		}
		break;
	}
}
/////////////////////////////////////////////////////////////////////


static void map32_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper32.last9000Write = regs[0];	

}


static void map32_setstate(SnssMapperBlock *state)
{
	regs[0] = state->extraData.mapper32.last9000Write;	
}


mapintf_t map32_intf =
{
   32, /* mapper number */
   "map32", /* mapper name */
   map32_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map32_getstate, /* get state (snss) */
   map32_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map32_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

