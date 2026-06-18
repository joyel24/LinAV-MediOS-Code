#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 regs[4];
/////////////////////////////////////////////////////////////////////
// Mapper 68

static void map68_SyncMirror()
{
	if(regs[0])
	{
		if (regs[1] == 0)
		{
			/*set_PPU_bank8(regs[2] + 0x80);   // + 0x20000
			set_PPU_bank9(regs[3] + 0x80);
			set_PPU_bank10(regs[2] + 0x80);
			set_PPU_bank11(regs[3] + 0x80);*/
			mmc_bankvrom8(regs[2] + 0x80);
			mmc_bankvrom9(regs[3] + 0x80);
			mmc_bankvrom10(regs[2] + 0x80);
			mmc_bankvrom11(regs[3] + 0x80);
		}
		else if (regs[1] == 1)
		{
			/*set_PPU_bank8(regs[2] + 0x80);
			set_PPU_bank9(regs[2] + 0x80);
			set_PPU_bank10(regs[3] + 0x80);
			set_PPU_bank11(regs[3] + 0x80);*/
			mmc_bankvrom8(regs[2] + 0x80);
			mmc_bankvrom9(regs[2] + 0x80);
			mmc_bankvrom10(regs[3] + 0x80);
			mmc_bankvrom11(regs[3] + 0x80);
		}
		else if (regs[1] == 2)
		{
			/*set_PPU_bank8(regs[2] + 0x80);
			set_PPU_bank9(regs[2] + 0x80);
			set_PPU_bank10(regs[2] + 0x80);
			set_PPU_bank11(regs[2] + 0x80);*/
			mmc_bankvrom8(regs[2] + 0x80);
			mmc_bankvrom9(regs[2] + 0x80);
			mmc_bankvrom10(regs[2] + 0x80);
			mmc_bankvrom11(regs[2] + 0x80);
		}
		else if (regs[1] == 3)
		{
			/*set_PPU_bank8(regs[3] + 0x80);
			set_PPU_bank9(regs[3] + 0x80);
			set_PPU_bank10(regs[3] + 0x80);
			set_PPU_bank11(regs[3] + 0x80);*/
			mmc_bankvrom8(regs[3] + 0x80);
			mmc_bankvrom9(regs[3] + 0x80);
			mmc_bankvrom10(regs[3] + 0x80);
			mmc_bankvrom11(regs[3] + 0x80);
		}
	}
	else
	{
		if (regs[1] == 0)
		{
			//set_mirroring(NES_PPU::MIRROR_VERT);
			ppu_mirror(0,1,0,1);
		}
		else if (regs[1] == 1)
		{
			//set_mirroring(NES_PPU::MIRROR_HORIZ);
			ppu_mirror(0,0,1,1);
		}
		else if (regs[1] == 2)
		{
			//set_mirroring(0,0,0,0);
			ppu_mirror(0,0,0,0);
		}
		else if (regs[1] == 3)
		{
			//set_mirroring(1,1,1,1);
			ppu_mirror(1,1,1,1);
		}
	}
}


static void map68_Reset()
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	regs[0] = 0;
	regs[1] = 0;
	regs[2] = 0;
	regs[3] = 0;
}

static void map68_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr & 0xF000)
	{
	case 0x8000:
		{
			/*set_PPU_bank0(data*2+0);
			set_PPU_bank1(data*2+1);*/
			mmc_bankvrom(2,0x0000,data);
		}
		break;

	case 0x9000:
		{
			/*set_PPU_bank2(data*2+0);
			set_PPU_bank3(data*2+1);*/
			mmc_bankvrom(2,0x0800,data);
		}
		break;

	case 0xA000:
		{
			/*set_PPU_bank4(data*2+0);
			set_PPU_bank5(data*2+1);*/
			mmc_bankvrom(2,0x1000,data);
		}
		break;

	case 0xB000:
		{
			/*set_PPU_bank6(data*2+0);
			set_PPU_bank7(data*2+1);*/
			mmc_bankvrom(2,0x1800,data);
		}
		break;

	case 0xC000:
		{
			regs[2] = data;
			map68_SyncMirror();
		}
		break;

	case 0xD000:
		{
			regs[3] = data;
			map68_SyncMirror();
		}
		break;

	case 0xE000:
		{
			regs[0] = (data & 0x10) >> 4;
			regs[1] = data & 0x03;
			map68_SyncMirror();
		}
		break;

	case 0xF000:
		{
			/*set_CPU_bank4(data*2);
			set_CPU_bank5(data*2+1);*/
			mmc_bankrom(16,0x8000,data);
		}
		break;
	}
}


static void map68_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper68.lastC000Write = regs[2];
	state->extraData.mapper68.lastD000Write = regs[3];
	state->extraData.mapper68.lastE000Write = (regs[0] << 4) | regs[1];
}
static void map68_setstate(SnssMapperBlock *state)
{
	regs[0] = (state->extraData.mapper68.lastE000Write & 0x10) >> 4;
	regs[1] = state->extraData.mapper68.lastE000Write & 0x03;
	regs[2] = state->extraData.mapper68.lastC000Write;
	regs[3] = state->extraData.mapper68.lastD000Write;
	
	map68_SyncMirror();
}
/////////////////////////////////////////////////////////////////////

mapintf_t map68_intf =
{
   68, /* mapper number */
   "Sunsoft Mapper #4", /* mapper name */
   map68_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map68_getstate, /* get state (snss) */
   map68_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map68_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};

