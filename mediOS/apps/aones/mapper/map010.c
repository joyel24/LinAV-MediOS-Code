#include "datatypes.h"
#include "unes_mapper.h"
#include "unes_ppu.h"

static uint8 regs[6];
static uint8 latch_0000;
static uint8 latch_1000;

/////////////////////////////////////////////////////////////////////
// Mapper 10

static void map10_set_VROM_0000()
{
	int bank_num = (latch_0000 == 0xFD) ? regs[1] : regs[2];

	/*bank_num <<= 2;

	set_PPU_bank0(bank_num+0);
	set_PPU_bank1(bank_num+1);
	set_PPU_bank2(bank_num+2);
	set_PPU_bank3(bank_num+3);*/
	mmc_bankvrom(4,0,bank_num);
}

static void map10_set_VROM_1000()
{
	int bank_num = (latch_1000 == 0xFD) ? regs[3] : regs[4];

	/*bank_num <<= 2;

	set_PPU_bank4(bank_num+0);
	set_PPU_bank5(bank_num+1);
	set_PPU_bank6(bank_num+2);
	set_PPU_bank7(bank_num+3);*/
	mmc_bankvrom(4,0x1000,bank_num);
}

static void map10_write(uint32 addr, uint8 data)
{
	switch(addr & 0xF000)
	{
	case 0xA000:
		{
     		uint8 bank_num;
			regs[0] = data;

			// 16K ROM bank at $8000
			bank_num = regs[0];
			/*set_CPU_bank4(bank_num*2);
			set_CPU_bank5(bank_num*2+1);*/
			mmc_bankrom(16,0x8000,bank_num);
		}
		break;

	case 0xB000:
		{
			// B000-BFFF: select 4k VROM for (0000) $FD latch
			regs[1] = data;
			map10_set_VROM_0000();
		}
		break;

	case 0xC000:
		{
			// C000-CFFF: select 4k VROM for (0000) $FE latch
			regs[2] = data;
			map10_set_VROM_0000();
		}
		break;

	case 0xD000:
		{
			// D000-DFFF: select 4k VROM for (1000) $FD latch
			regs[3] = data;
			map10_set_VROM_1000();
		}
		break;

	case 0xE000:
		{
			// E000-EFFF: select 4k VROM for (1000) $FE latch
			regs[4] = data;
			map10_set_VROM_1000();
		}
		break;

	case 0xF000:
		{
			regs[5] = data;

			if(regs[5] & 0x01)
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

static void map10_latchfunc(uint32 addr)
{
	if(addr & 0x1000)
	{
		latch_1000 = (addr >> 4) & 0xFF;
		map10_set_VROM_1000();
	}
	else
	{
		latch_0000 = (addr >> 4) & 0xFF;
		map10_set_VROM_0000();
	}   
}


static void map10_reset(void)
{
	int i;
	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	for(i = 0; i < sizeof(regs)/sizeof(regs[0]); i++)
		regs[i] = 0;

	regs[2] = 4;

	latch_0000 = 0xFE;
	latch_1000 = 0xFE;

	map10_set_VROM_0000();
	map10_set_VROM_1000();
	
	ppu_set_Latch_FDFE(map10_latchfunc);
}



static void map10_getstate(SnssMapperBlock *state)
{
	// 2 latch registers
	state->extraData.mapper10.latch[0] = latch_0000;
	state->extraData.mapper10.latch[1] = latch_1000;

	// regs (B/C/D/E000)
	state->extraData.mapper10.lastB000Write = regs[1];
	state->extraData.mapper10.lastC000Write = regs[2];
	state->extraData.mapper10.lastD000Write = regs[3];
	state->extraData.mapper10.lastE000Write = regs[4];
}	

static void map10_setstate(SnssMapperBlock *state)
{
	latch_0000 = state->extraData.mapper10.latch[0];
	latch_1000 = state->extraData.mapper10.latch[1];

			// regs (B/C/D/E000)
	regs[1] = state->extraData.mapper10.lastB000Write;
	regs[2] = state->extraData.mapper10.lastC000Write;
	regs[3] = state->extraData.mapper10.lastD000Write;
	regs[4] = state->extraData.mapper10.lastE000Write;
	
	map10_set_VROM_0000();
	map10_set_VROM_1000();
}

/*
static map_memwrite map10_memwrite[] =
{
   { 0x8000, 0xFFFF, map10_write },
   {     -1,     -1, NULL }
};
*/

mapintf_t map10_intf =
{
   10, /* mapper number */
   "MMC4", /* mapper name */
   map10_reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map10_getstate, /* get state (snss) */
   map10_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map10_write,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};
