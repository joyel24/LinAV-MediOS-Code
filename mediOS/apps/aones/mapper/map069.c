#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 patch;
static uint8 regs[1];
static uint8 irq_enabled;
static uint32 irq_counter;
/////////////////////////////////////////////////////////////////////
// Mapper 69
static void map69_Reset()
{
	patch = 0;

	// for Dynamite Batman
	if(mmc_getinfo()->var.crc == 0xad28aef6) // Dynamite Batman
	{
		patch = 1;
	}

	// Init ExSound
	//parent_NES->apu->SelectExSound(32);
	//exsound apu_setexchip(32);

	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	/*if(num_1k_VROM_banks)
	{
		set_PPU_banks(0,1,2,3,4,5,6,7);
	}*/

	irq_enabled = 0;
	irq_counter = 0;
}

static void map69_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr & 0xE000)
	{
	case 0x8000:
		{
			regs[0] = data;
		}
		break;

	case 0xA000:
		{
			switch(regs[0] & 0x0F)
			{
			case 0x00:
				{
					//set_PPU_bank0(data);
					mmc_bankvrom(1,0x0000,data);
				}
				break;

			case 0x01:
				{
					//set_PPU_bank1(data);
					mmc_bankvrom(1,0x0400,data);
				}
				break;

			case 0x02:
				{
					//set_PPU_bank2(data);
					mmc_bankvrom(1,0x0800,data);
				}
				break;

			case 0x03:
				{
					//set_PPU_bank3(data);
					mmc_bankvrom(1,0x0C00,data);
				}
				break;

			case 0x04:
				{
					//set_PPU_bank4(data);
					mmc_bankvrom(1,0x1000,data);
				}
				break;

			case 0x05:
				{
					//set_PPU_bank5(data);
					mmc_bankvrom(1,0x1400,data);
				}
				break;

			case 0x06:
				{
					//set_PPU_bank6(data);
					mmc_bankvrom(1,0x1800,data);
				}
				break;

			case 0x07:
				{
					//set_PPU_bank7(data);
					mmc_bankvrom(1,0x1C00,data);
				}
				break;

			case 0x08:
				{
					if(!patch)
					{
						if(!(data & 0x40))
						{
							//set_CPU_bank3(data);
							mmc_getinfo()->CPUPageIndex[3]=mmc_getinfo()->var.prg_beg+((uint32)data<<13);
							#ifdef __asmcpu__   		
   							init_sram((uint8*)(mmc_getinfo()->CPUPageIndex[3]));
   							#else
   							//nes6502 uses directly CPUPageIndex...
   							// so jsut calling to have pfast_pc_update if needed
   							/*nes6502_getcontext(&_NEScontext);   		
   							nes6502_setcontext(&_NEScontext);   		*/
   							#endif
						}
					}
				}
				break;

			case 0x09:
				{
					//set_CPU_bank4(data);
					mmc_bankrom(8,0x8000,data);
				}
				break;

			case 0x0A:
				{
					//set_CPU_bank5(data);
					mmc_bankrom(8,0xA000,data);
				}
				break;

			case 0x0B:
				{
					//set_CPU_bank6(data);
					mmc_bankrom(8,0xC000,data);
				}
				break;

			case 0x0C:
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

			case 0x0D:
				{
					irq_enabled = data;
				}
				break;

			case 0x0E:
				{
					irq_counter = (irq_counter & 0xFF00) | data;
				}
				break;

			case 0x0F:
				{
					irq_counter = (irq_counter & 0x00FF) | (data << 8);
				}
				break;
			}
		}
		break;

	case 0xC000:
	case 0xE000:
		{
			//parent_NES->apu->ExWrite(addr, data);
			//exsound ex_write(addr,data);
		}
		break;
	}
}

static void map69_HSync(uint32 scanline)
{
	if(irq_enabled)
	{
		if(irq_counter <= 113)
		{
			//parent_NES->cpu->DoIRQ();
			doIRQ();
			irq_counter = 0;
		}
		else
		{
			irq_counter -= 113;
		}
	}
}
/////////////////////////////////////////////////////////////////////

static void map69_getstate(SnssMapperBlock *state)
{
   state->extraData.mapper69.irqCounterLowByte = irq_counter & 0x00FF;
   state->extraData.mapper69.irqCounterHighByte = (irq_counter & 0xFF00) >> 8;
   state->extraData.mapper69.irqCounterEnabled = irq_enabled;
   state->extraData.mapper69.last8000Write = regs[0];
}


static void map69_setstate(SnssMapperBlock *state)
{
   
   irq_counter = state->extraData.mapper69.irqCounterLowByte;
   irq_counter |= state->extraData.mapper69.irqCounterHighByte << 8;
   irq_enabled = state->extraData.mapper69.irqCounterEnabled;
   regs[0] = state->extraData.mapper69.last8000Write;
}


mapintf_t map69_intf =
{
   69, /* mapper number */
   "Sunsoft FME-7", /* mapper name */
   map69_Reset, /* init routine */
   NULL, /* vblank callback */
   map69_HSync, /* hblank callback */
   map69_getstate, /* get state (snss) */
   map69_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map69_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

