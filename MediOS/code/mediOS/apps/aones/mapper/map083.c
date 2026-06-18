#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 regs[3];
static uint32 irq_counter;
static uint8 irq_enabled;
/////////////////////////////////////////////////////////////////////
// Mapper 83
static void map83_Reset()
{
	regs[0] = regs[1] = regs[2] = 0;

	// set CPU bank pointers
	if(num_8k_ROM_banks >= 32)
	{
		//set_CPU_banks(0,1,30,31);
		mmc_bankromALL4(0,1,30,31);
		regs[1] = 0x30;
	}
	else
	{
		//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
	}

	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/

	irq_enabled = 0;
	irq_counter = 0;
}

static uint8 map83_MemoryReadLow(uint32 addr)
{
	if((addr & 0x5100) == 0x5100)
	{
		return regs[2];
	}
	else
	{
		return addr >> 8;
	}
}

static void map83_MemoryWriteLow(uint32 addr, uint8 data)
{
	switch(addr)
	{
	case 0x5101:
	case 0x5102:
	case 0x5103:
		{
			regs[2] = data;
		}
		break;
	}
}

static void map83_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr)
	{
	case 0x8000:
	case 0xB000:
	case 0xB0FF:
	case 0xB1FF:
		{
			regs[0] = data;
			/*set_CPU_bank4((data&0x3f)*2+0);
			set_CPU_bank5((data&0x3f)*2+1);
			set_CPU_bank6(((data&0x30)|0x0F)*2+0);
			set_CPU_bank7(((data&0x30)|0x0F)*2+1);*/
			mmc_bankrom(16,0x8000,(data&0x3f));
			mmc_bankrom(16,0xC000,((data&0x30)|0x0F));
		}
		break;

	case 0x8100:
		{
			/*        if(num_1k_VROM_banks <= 32*8)
			        {
			          regs[1] = data;
			        }
			*/
			if((data & 0x03) == 0x00)
			{
				//set_mirroring(NES_PPU::MIRROR_VERT);
				ppu_mirror(0,1,0,1);
			}
			else if((data & 0x03) == 0x01)
			{
				//set_mirroring(NES_PPU::MIRROR_HORIZ);
				ppu_mirror(0,0,1,1);
			}
			else if((data & 0x03) == 0x02)
			{
				//set_mirroring(0,0,0,0);
				ppu_mirror(0,0,0,0);
			}
			else
			{
				//set_mirroring(1,1,1,1);
				ppu_mirror(1,1,1,1);
			}
		}
		break;

	case 0x8200:
		{
			irq_counter = (irq_counter & 0xFF00) | (uint32)data;
		}
		break;

	case 0x8201:
		{
			irq_counter = (irq_counter & 0x00FF) | ((uint32)data << 8);
			irq_enabled = data;
		}
		break;

	case 0x8300:
		{
			//set_CPU_bank4(data);
			mmc_bankrom(8,0x8000,data);
		}
		break;

	case 0x8301:
		{
			//set_CPU_bank5(data);
			mmc_bankrom(8,0xA000,data);
		}
		break;

	case 0x8302:
		{
			//set_CPU_bank6(data);
			mmc_bankrom(8,0xC000,data);
		}
		break;

	case 0x8310:
		{
			//set_PPU_bank0((uint32)((regs[0]&0x30)<<4)|data);
			mmc_bankvrom(1,0x0000,(uint32)((regs[0]&0x30)<<4)|data);
			/*
			        if((regs[1] & 0x30) == 0x30)
			        {
			          set_PPU_bank0((uint32)((data & 0x30) << 4)^data);
			        }
			        else if((regs[1] & 0x30) == 0x10 || (regs[1] & 0x30) == 0x20)
			        {
			          set_PPU_bank0((((data & 0x30) << 4)^data)*2+0);
			          set_PPU_bank1((((data & 0x30) << 4)^data)*2+1);
			        }
				*/
		}
		break;

	case 0x8311:
		{
			//set_PPU_bank1((uint32)((regs[0]&0x30)<<4)|data);
			mmc_bankvrom(1,0x0400,(uint32)((regs[0]&0x30)<<4)|data);
			/*
			        if((regs[1] & 0x30) == 0x30)
			        {
			          set_PPU_bank1((uint32)((data & 0x30) << 4)^data);
			        }
			        else if((regs[1] & 0x30) == 0x10 || (regs[1] & 0x30) == 0x20)
			        {
			          set_PPU_bank2((((data & 0x30) << 4)^data)*2+0);
			          set_PPU_bank3((((data & 0x30) << 4)^data)*2+1);
			        }
				*/
		}
		break;

	case 0x8312:
		{
			//set_PPU_bank2((uint32)((regs[0]&0x30)<<4)|data);
			mmc_bankvrom(1,0x0800,(uint32)((regs[0]&0x30)<<4)|data);
			/*
			        if((regs[1] & 0x30) == 0x30)
			        {
			          set_PPU_bank2((uint32)((data & 0x30) << 4)^data);
			        }
			        else if((regs[1] & 0x30) == 0x10 || (regs[1] & 0x30) == 0x20)
			        {
			          set_PPU_bank4((((data & 0x30) << 4)^data)*2+0);
			          set_PPU_bank5((((data & 0x30) << 4)^data)*2+1);
			        }
				*/
		}
		break;

	case 0x8313:
		{
			//set_PPU_bank3((uint32)((regs[0]&0x30)<<4)|data);
			mmc_bankvrom(1,0x0C00,(uint32)((regs[0]&0x30)<<4)|data);
			/*
					if((regs[1] & 0x30) == 0x30)
			        {
			          set_PPU_bank3((uint32)((data & 0x30) << 4)^data);
			        }
			        else if((regs[1] & 0x30) == 0x10 || (regs[1] & 0x30) == 0x20)
			        {
			          set_PPU_bank6((((data & 0x30) << 4)^data)*2+0);
			          set_PPU_bank7((((data & 0x30) << 4)^data)*2+1);
			        }
				*/
		}
		break;

	case 0x8314:
		{
			//set_PPU_bank4((uint32)((regs[0]&0x30)<<4)|data);
			mmc_bankvrom(1,0x1000,(uint32)((regs[0]&0x30)<<4)|data);
			/*
			        if((regs[1] & 0x30) == 0x30)
			        {
			          set_PPU_bank4((uint32)((data & 0x30) << 4)^data);
			        }
			        else if((regs[1] & 0x30) == 0x10 || (regs[1] & 0x30) == 0x20)
			        {
			          set_PPU_bank4((((data & 0x30) << 4)^data)*2+0);
			          set_PPU_bank5((((data & 0x30) << 4)^data)*2+1);
			        }
				*/
		}
		break;

	case 0x8315:
		{
			//set_PPU_bank5((uint32)((regs[0]&0x30)<<4)|data);
			mmc_bankvrom(1,0x1400,(uint32)((regs[0]&0x30)<<4)|data);
			/*
					if((regs[1] & 0x30) == 0x30)
			        {
			          set_PPU_bank5((uint32)((data & 0x30) << 4)^data);
			        }
			        else if((regs[1] & 0x30) == 0x10 || (regs[1] & 0x30) == 0x20)
			        {
			          set_PPU_bank6((((data & 0x30) << 4)^data)*2+0);
			          set_PPU_bank7((((data & 0x30) << 4)^data)*2+1);
			        }
				*/
		}
		break;

	case 0x8316:
		{
			//set_PPU_bank6((uint32)((regs[0]&0x30)<<4)|data);
			mmc_bankvrom(1,0x1800,(uint32)((regs[0]&0x30)<<4)|data);
			/*
					if((regs[1] & 0x30) == 0x30)
			        {
			          set_PPU_bank6(((data & 0x30) << 4)^data);
			        }
			        else if((regs[1] & 0x30) == 0x10 || (regs[1] & 0x30) == 0x20)
			        {
			          set_PPU_bank4((((data & 0x30) << 4)^data)*2+0);
			          set_PPU_bank5((((data & 0x30) << 4)^data)*2+1);
			        }
					*/
		}
		break;

	case 0x8317:
		{
			//set_PPU_bank7((uint32)((regs[0]&0x30)<<4)|data);
			mmc_bankvrom(1,0x1C00,(uint32)((regs[0]&0x30)<<4)|data);
			/*
			        if((regs[1] & 0x30) == 0x30)
			        {
			          set_PPU_bank7(((data & 0x30) << 4)^data);
			        }
			        else if((regs[1] & 0x30) == 0x10 || (regs[1] & 0x30) == 0x20)
			        {
			          set_PPU_bank6((((data & 0x30) << 4)^data)*2+0);
			          set_PPU_bank7((((data & 0x30) << 4)^data)*2+1);
			        }
			*/
		}
		break;

	case 0x8318:
		{
			/*set_CPU_bank4(((regs[0]&0x30)|data)*2+0);
			set_CPU_bank5(((regs[0]&0x30)|data)*2+1);*/
			mmc_bankrom(16,0x8000,((regs[0]&0x30)|data));
		}
		break;
	}	
}

static void map83_HSync(uint32 scanline)
{
	if(irq_enabled)
	{
		if(irq_counter <= 113)
		{
			//parent_NES->cpu->DoIRQ();
			doIRQ();
			irq_enabled = 0;
		}
		else
		{
			irq_counter -= 113;
		}
	}
}
/////////////////////////////////////////////////////////////////////

static void map83_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper83.irqCounterLowByte = irq_counter & 0x00FF;
	state->extraData.mapper83.irqCounterHighByte = (irq_counter & 0xFF00) >> 8;
	state->extraData.mapper83.irqCounterEnabled = irq_enabled;
	state->extraData.mapper83.last8000Write = regs[0];
	state->extraData.mapper83.last8100Write = regs[1];
	state->extraData.mapper83.last5101Write = regs[2];
}
static void map83_setstate(SnssMapperBlock *state)
{
	irq_counter = state->extraData.mapper83.irqCounterLowByte;
	irq_counter |= state->extraData.mapper83.irqCounterHighByte << 8;
	irq_enabled = state->extraData.mapper83.irqCounterEnabled;
	regs[0] = state->extraData.mapper83.last8000Write;
	regs[1] = state->extraData.mapper83.last8100Write;
	regs[2] = state->extraData.mapper83.last5101Write;
}

mapintf_t map83_intf =
{
   83, /* mapper number */
   "map83", /* mapper name */
   map83_Reset, /* init routine */
   NULL, /* vblank callback */
   map83_HSync, /* hblank callback */
   map83_getstate, /* get state (snss) */
   map83_setstate, /* set state (snss) */   
   map83_MemoryReadLow,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   map83_MemoryWriteLow,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map83_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

