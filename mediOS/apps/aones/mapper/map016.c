
#include "datatypes.h"
#include "unes_mapper.h"
#include "unes_ppu.h"
#include "unes.h"


static uint8 patch, patch2;
static uint8 regs[3];

//static uint8 serial_out[0x2000];



static uint8 irq_enabled;
static uint32 irq_counter;
static uint32 irq_latch;

static void map16_write2(uint32 addr, uint8 data)
{
	switch(addr & 0x000F)
	{
	case 0x0000:
		{
			//set_PPU_bank0(data);
			mmc_bankvrom(1,0x0000,data);
		}
		break;

	case 0x0001:
		{
			//set_PPU_bank1(data);
			mmc_bankvrom(1,0x0400,data);
		}
		break;

	case 0x0002:
		{
			//set_PPU_bank2(data);
			mmc_bankvrom(1,0x0800,data);
		}
		break;

	case 0x0003:
		{
			//set_PPU_bank3(data);
			mmc_bankvrom(1,0x0C00,data);
		}
		break;

	case 0x0004:
		{
			//set_PPU_bank4(data);
			mmc_bankvrom(1,0x1000,data);
		}
		break;

	case 0x0005:
		{
			//set_PPU_bank5(data);
			mmc_bankvrom(1,0x1400,data);
		}
		break;

	case 0x0006:
		{
			//set_PPU_bank6(data);
			mmc_bankvrom(1,0x1800,data);
		}
		break;

	case 0x0007:
		{
			//set_PPU_bank7(data);
			mmc_bankvrom(1,0x1C00,data);
		}
		break;

	case 0x0008:
		{
			/*set_CPU_bank4(data*2+0);
			set_CPU_bank5(data*2+1);*/
			mmc_bankrom(16,0x8000,data);
		}
		break;

	case 0x0009:
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

	case 0x000A:
		{
			irq_enabled = data & 0x01;
			irq_counter = irq_latch;
		}
		break;

	case 0x000B:
		{
			irq_latch = (irq_latch & 0xFF00) | data;
		}
		break;

	case 0x000C:
		{
			irq_latch = ((uint32)data << 8) | (irq_latch & 0x00FF);
		}
		break;

	case 0x000D:
		//eeprom
		break;
	}
}

static void map16_write3(uint32 addr, uint8 data)
{
	switch(addr)
	{
	case 0x8000:
	case 0x8001:
	case 0x8002:
	case 0x8003:
		{
			regs[0] = data & 0x01;
			/*set_CPU_bank4(regs[0]*0x20+regs[2]*2+0);
			set_CPU_bank5(regs[0]*0x20+regs[2]*2+1);*/
			mmc_bankrom(16,0x8000,regs[0]*0x10+regs[2]);
		}
		break;

	case 0x8004:
	case 0x8005:
	case 0x8006:
	case 0x8007:
		{
			regs[1] = data & 0x01;
			/*set_CPU_bank6(regs[1]*0x20+0x1E);
			set_CPU_bank7(regs[1]*0x20+0x1F);*/
			mmc_bankrom(16,0xC000,regs[1]*0x10+0x0F);
		}
		break;

	case 0x8008:
		{
			regs[2] = data;
			/*set_CPU_bank4(regs[0]*0x20+regs[2]*2+0);
			set_CPU_bank5(regs[0]*0x20+regs[2]*2+1);
			set_CPU_bank6(regs[1]*0x20+0x1E);
			set_CPU_bank7(regs[1]*0x20+0x1F);*/
			mmc_bankrom(16,0x8000,regs[0]*0x10+regs[2]);
			mmc_bankrom(16,0xC000,regs[1]*0x10+0xF);
		}
		break;

	case 0x8009:
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

	case 0x800A:
		{
			irq_enabled = data & 0x01;
			irq_counter = irq_latch;
		}
		break;

	case 0x800B:
		{
			irq_latch = (irq_latch & 0xFF00) | data;
		}
		break;

	case 0x800C:
		{
			irq_latch = ((uint32)data << 8) | (irq_latch & 0x00FF);
		}
		break;

	case 0x800D:
		{
			//write protect
		}
		break;
	}
}

static void map16_write(uint32 addr, uint8 data)
{
	//LOG("W " << HEX(addr,4) << "  " << HEX(data,2)  << endl);
	if(patch)
	{
		map16_write3(addr, data);
	}
	else
	{
		map16_write2(addr, data);
	}
}

#if 0
static void map16_writeSaveRAM(uint32 addr, uint8 data)
{
	if(patch)
	{
		//map16_write3(addr, data);
	}
	else
	{
		map16_write2(addr, data);
	}
}
#endif

static void map16_hblank(uint32 scanline)
{	
	if(irq_enabled)
	{
		if(irq_counter <= (unsigned int)((patch2)?113:114))
		{
			//parent_NES->cpu->DoPendingIRQ();
			//PendingIRQ();			
			doIRQ();
			//    parent_NES->cpu->DoIRQ();
			irq_counter = 0xFFFF;
			irq_enabled = 0;
		}
		else
		{
			irq_counter -= (unsigned int)((patch2)?113:114);
		}
	}
}


static void map16_getstate(SnssMapperBlock *state)
{
   state->extraData.mapper16.irqCounterLowByte = irq_counter & 0x00FF;
   state->extraData.mapper16.irqCounterHighByte = (irq_counter & 0xFF00) >> 8;
   state->extraData.mapper16.irqCounterEnabled = irq_enabled;
   state->extraData.mapper16.irqLatchCounterLowByte = irq_latch & 0x00FF;
   state->extraData.mapper16.irqLatchCounterHighByte = (irq_latch & 0xFF00) >> 8;
}

static void map16_setstate(SnssMapperBlock *state)
{
   irq_counter = state->extraData.mapper16.irqCounterLowByte;   
   irq_counter |= state->extraData.mapper16.irqCounterHighByte << 8;
   irq_enabled = state->extraData.mapper16.irqCounterEnabled;
   irq_latch = state->extraData.mapper16.irqLatchCounterLowByte;
   irq_latch |= state->extraData.mapper16.irqLatchCounterHighByte << 8;
}


void map16_init(void)
{
	patch = patch2 = 0;

	if(mmc_getinfo()->var.crc == 0x3f15d20d) // Famicom Jump 2 - Saikyou no 7 Nin
	{
		patch2 =1;
		patch = 1;
	}
	if(mmc_getinfo()->var.crc == 0x09499f4d || // DragonBallZ3-RessenJinzouNingen(J)
	        mmc_getinfo()->var.crc == 0x170250de || // RokudenashiBlues(J)
	        mmc_getinfo()->var.crc == 0x73ac76db || // SD Gundam Gaiden 2
	        mmc_getinfo()->var.crc == 0x0be0a328 || // SD Gundam Wars
	        mmc_getinfo()->var.crc == 0x19e81461 ||  //
			mmc_getinfo()->var.crc == 0x6c6c2feb)		//DragonBall 3
	{
		patch2 = 1;
	}


	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);	

	regs[0] = 0;
	regs[1] = 0;
	regs[2] = 0;

	irq_enabled = 0;
	irq_counter = 0;
	irq_latch = 0;
}




mapintf_t map16_intf = 
{
   16, /* mapper number */
   "Bandai", /* mapper name */
   map16_init, /* init routine */
   NULL, /* vblank callback */
   map16_hblank, /* hblank callback */
   map16_getstate, /* get state (snss) */
   map16_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   /*map16_readSaveRAM*/NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   map16_write/*SaveRAM*/,  /*Write saveram*/
   map16_write,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};
