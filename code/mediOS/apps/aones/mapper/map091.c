#include "datatypes.h"
#include "unes_mapper.h"
#include "unes_ppu.h"

static uint8 irq_counter;
static uint8 irq_enabled;

/////////////////////////////////////////////////////////////////////
// Mapper 91

void map91_writelow(uint32 addr, uint8 data)
{
	//LOG("W " << HEX(addr,4) << "  " << HEX(data,2) << endl);
	switch(addr & 0xF007)
	{
	case 0x6000:
		{
			/*set_PPU_bank0(data*2+0);
			set_PPU_bank1(data*2+1);*/
			mmc_bankvrom(2,0x0,data);
		}
		break;

	case 0x6001:
		{
			/*set_PPU_bank2(data*2+0);
			set_PPU_bank3(data*2+1);*/
			mmc_bankvrom(2,0x800,data);
		}
		break;

	case 0x6002:
		{
			/*set_PPU_bank4(data*2+0);
			set_PPU_bank5(data*2+1);*/
			mmc_bankvrom(2,0x1000,data);
		}
		break;

	case 0x6003:
		{
			/*set_PPU_bank6(data*2+0);
			set_PPU_bank7(data*2+1);*/
			mmc_bankvrom(2,0x1800,data);
		}
		break;

	case 0x7000:
		{
			//set_CPU_bank4(data);
			mmc_bankrom(8,0x8000,data);
		}
		break;

	case 0x7001:
		{
			//set_CPU_bank5(data);
			mmc_bankrom(8,0xA000,data);
		}
		break;

	case 0x7002:
		{
			irq_enabled = 0;
			irq_counter = 0;
		}
		break;

	case 0x7003:
		{
			irq_enabled = 1;
		}
		break;	
	}
}

void map91_hblank(uint32 scanline)
{	
  if (scanline>240) return;
  if(irq_enabled)  		
     if (ppu_enabled())	
          if(++irq_counter>=8) doIRQ();							
}
/////////////////////////////////////////////////////////////////////

static void map91_init()
{
	// set CPU bank pointers
	/*set_CPU_bank4(num_8k_ROM_banks-2);
	set_CPU_bank5(num_8k_ROM_banks-1);
	set_CPU_bank6(num_8k_ROM_banks-2);
	set_CPU_bank7(num_8k_ROM_banks-1);*/
	/*mmc_bankrom(16,0x8000,MMC_LASTBANK);
	mmc_bankrom(16,0xC000,MMC_LASTBANK);*/

	// set PPU bank pointers
	if(mmc_getinfo()->ROM_Header.chr_rom_pages_nb)
	{
		//set_PPU_banks(0,0,0,0,0,0,0,0);
		/*mmc_bankvrom(1,0,0);
		mmc_bankvrom(1,0x400,0);
		mmc_bankvrom(1,0x800,0);
		mmc_bankvrom(1,0xC00,0);
		mmc_bankvrom(1,0x1000,0);
		mmc_bankvrom(1,0x1400,0);
		mmc_bankvrom(1,0x1800,0);
		mmc_bankvrom(1,0x1C00,0);*/
	}
}

static void map91_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper91.irqCounter = irq_counter;
	state->extraData.mapper91.irqCounterEnabled = irq_enabled;
}
static void map91_setstate(SnssMapperBlock *state)
{
	irq_counter = state->extraData.mapper91.irqCounter;
	irq_enabled = state->extraData.mapper91.irqCounterEnabled;
}

mapintf_t map91_intf =
{
   91, /* mapper number */
   "HK-SF3", /* mapper name */
   map91_init, /* init routine */
   NULL, /* vblank callback */
   map91_hblank, /* hblank callback */
   map91_getstate, /* get state (snss) */
   map91_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   map91_writelow,  /*Write saveram*/
   NULL,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/      
};

