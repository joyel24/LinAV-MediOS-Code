#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "snss.h"
#include "unes_ppu.h"

/////////////////////////////////////////////////////////////////////
// Mapper 15
static void map15_Reset(void)
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,2,3);
	//mmc_bankrom(16,0xC000,1);
	mmc_bankrom(32,0x8000,0);
}

void map15_write(uint32 addr, uint8 data)
{
	switch(addr)
	{
	case 0x8000:
		{
			if(data & 0x80)
			{
				/*set_CPU_bank4((data & 0x3F)*2+1);
				set_CPU_bank5((data & 0x3F)*2+0);
				set_CPU_bank6((data & 0x3F)*2+3);
				set_CPU_bank7((data & 0x3F)*2+2);*/
				mmc_bankrom(8,0x8000,(data & 0x3F)*2+1);
				mmc_bankrom(8,0xA000,(data & 0x3F)*2+0);
				mmc_bankrom(8,0xC000,(data & 0x3F)*2+2);
				mmc_bankrom(8,0xE000,(data & 0x3F)*2+1);
			}
			else
			{
				/*set_CPU_bank4((data & 0x3F)*2+0);
				set_CPU_bank5((data & 0x3F)*2+1);
				set_CPU_bank6((data & 0x3F)*2+2);
				set_CPU_bank7((data & 0x3F)*2+3);*/
				mmc_bankrom(16,0x8000,(data & 0x3F));
				mmc_bankrom(16,0xC000,(data & 0x3F)+1);
			}
			if(data & 0x40)
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
		break;

	case 0x8001:
		{
			mmc_bankrom(16,0x8000,data & 0x3F);
			mmc_bankrom(16,0xC000,MMC_LASTBANK);//(data & 0x3F)*2+0);
			ppu_mirror(0,1,0,1);
			/*if(data & 0x80)
			{
				//set_CPU_bank6((data & 0x3F)*2+1);
				//set_CPU_bank7((data & 0x3F)*2+0);
				
			}
			else
			{
				//set_CPU_bank6((data & 0x3F)*2+0);
				//set_CPU_bank7((data & 0x3F)*2+1);
				mmc_bankrom(16,0xC000,(data & 0x3F));				
			}*/
		}
		break;

	case 0x8002:
		{
			if(data & 0x80)
			{
				/*set_CPU_bank4((data & 0x3F)*2+1);
				set_CPU_bank5((data & 0x3F)*2+1);
				set_CPU_bank6((data & 0x3F)*2+1);
				set_CPU_bank7((data & 0x3F)*2+1);*/
				mmc_bankrom(8,0x8000,(data & 0x3F)*2+1);
				mmc_bankrom(8,0xA000,(data & 0x3F)*2+1);
				mmc_bankrom(8,0xC000,(data & 0x3F)*2+1);
				mmc_bankrom(8,0xE000,(data & 0x3F)*2+1);
			}
			else
			{
				/*set_CPU_bank4((data & 0x3F)*2);
				set_CPU_bank5((data & 0x3F)*2);
				set_CPU_bank6((data & 0x3F)*2);
				set_CPU_bank7((data & 0x3F)*2);*/
				mmc_bankrom(8,0x8000,(data & 0x3F)*2);
				mmc_bankrom(8,0xA000,(data & 0x3F)*2);
				mmc_bankrom(8,0xC000,(data & 0x3F)*2);
				mmc_bankrom(8,0xE000,(data & 0x3F)*2);
			}
		}
		break;

	case 0x8003:
		{
			if(data & 0x80)
			{
				/*set_CPU_bank6((data & 0x3F)*2+1);
				set_CPU_bank7((data & 0x3F)*2+0);*/
				mmc_bankrom(8,0xC000,(data & 0x3F)*2+1);
				mmc_bankrom(8,0xE000,(data & 0x3F)*2+0);
			}
			else
			{
				/*set_CPU_bank6((data & 0x3F)*2+0);
				set_CPU_bank7((data & 0x3F)*2+1);*/
				mmc_bankrom(16,0xC000,(data & 0x3F));
			}
			if(data & 0x40)
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
		break;
	}
}
/////////////////////////////////////////////////////////////////////

static void map15_getstate(SnssMapperBlock *state)
{
   
}

static void map15_setstate(SnssMapperBlock *state)
{
   
}

mapintf_t map15_intf =
{
   15, /* mapper number */
   "100-in-1", /* mapper name */
   map15_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map15_getstate, /* get state (snss) */
   map15_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map15_write,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};

