#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"


static uint8 irq_enabled;
static uint32 irq_counter;

/////////////////////////////////////////////////////////////////////
// Mapper 6

static void map6_writelow(uint32 addr, uint8 data)
{
	switch(addr)
	{
	case 0x42FE:
		{
			if(data & 0x10)
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

	case 0x42FF:
		{
			if(data & 0x10)
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

	case 0x4501:
		{
			irq_enabled = 0;
		}
		break;

	case 0x4502:
		{
			irq_counter = (irq_counter & 0xFF00) | (uint32)data;
		}
		break;

	case 0x4503:
		{
			irq_counter = (irq_counter & 0x00FF) | ((uint32)data << 8);
			irq_enabled = 1;
		}
		break;
	}
}

static void map6_write(uint32 addr, uint8 data)
{
	//uint8 prg_bank = (data & 0x3C) >> 2;
	uint8 prg_bank = (data & 0x3C) >> 2;
	uint8 chr_bank = data & 0x03;

	/*set_CPU_bank4(prg_bank*2+0);
	set_CPU_bank5(prg_bank*2+1);*/
	mmc_bankrom(16,0x8000,prg_bank);
	
	

	/*set_VRAM_bank(0, chr_bank * 8 + 0);
	set_VRAM_bank(1, chr_bank * 8 + 1);
	set_VRAM_bank(2, chr_bank * 8 + 2);
	set_VRAM_bank(3, chr_bank * 8 + 3);
	set_VRAM_bank(4, chr_bank * 8 + 4);
	set_VRAM_bank(5, chr_bank * 8 + 5);
	set_VRAM_bank(6, chr_bank * 8 + 6);
	set_VRAM_bank(7, chr_bank * 8 + 7);*/	
	
	mmc_VRAM_bank(0, chr_bank * 8 + 0);
	mmc_VRAM_bank(1, chr_bank * 8 + 1);
	mmc_VRAM_bank(2, chr_bank * 8 + 2);
	mmc_VRAM_bank(3, chr_bank * 8 + 3);
	mmc_VRAM_bank(4, chr_bank * 8 + 4);
	mmc_VRAM_bank(5, chr_bank * 8 + 5);
	mmc_VRAM_bank(6, chr_bank * 8 + 6);
	mmc_VRAM_bank(7, chr_bank * 8 + 7);
	
}

static void map6_hblank(uint32 scanline)
{
	if(irq_enabled)
	{
		irq_counter += 133;
		if(irq_counter >= 0xFFFF)
		{
			//parent_NES->cpu->DoIRQ();
			doIRQ();
			irq_counter = 0;
		}
	}
}

static void map6_reset(void)
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,14,15);
	mmc_bankrom(16,0x8000,0);
	mmc_bankrom(16,0xC000,7);


	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}
	else
	{
		set_VRAM_bank(0, 0);
		set_VRAM_bank(0, 1);
		set_VRAM_bank(0, 2);
		set_VRAM_bank(0, 3);
		set_VRAM_bank(0, 4);
		set_VRAM_bank(0, 5);
		set_VRAM_bank(0, 6);
		set_VRAM_bank(0, 7);
	}*/
	mmc_getinfo()->vramsize = 0x8000;
}

static void map6_getstate(SnssMapperBlock *state)
{	
	state->extraData.mapper6.irqCounterLowByte = irq_counter & 0x00FF;
	state->extraData.mapper6.irqCounterHighByte = (irq_counter & 0xFF00) >> 8;
	state->extraData.mapper6.irqCounterEnabled = irq_enabled;
}

static void map6_setstate(SnssMapperBlock *state)
{
	irq_counter = state->extraData.mapper6.irqCounterLowByte;
	irq_counter |= state->extraData.mapper6.irqCounterHighByte << 8;
	irq_enabled = state->extraData.mapper6.irqCounterEnabled;
}

mapintf_t map6_intf =
{
   6, /* mapper number */
   "FFE F4xxx", /* mapper name */
   map6_reset, /* init routine */
   NULL, /* vblank callback */
   map6_hblank, /* hblank callback */
   map6_getstate, /* get state (snss) */
   map6_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   map6_writelow,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map6_write,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};
