#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8  write_count;
static uint8  bits;
static uint8  regs[4];

static uint8  irq_enabled;
static uint32 irq_counter;
static uint8  init_state;
/////////////////////////////////////////////////////////////////////
// Mapper 105
static void map105_Reset()
{
	mmc_bankromALL4(0,1,2,3);

	regs[0] = 0x0C;
	regs[1] = 0x00;
	regs[2] = 0x00;
	regs[3] = 0x10;

	bits = 0;
	write_count = 0;

	irq_counter = 0;
	irq_enabled = 0;
	init_state = 0;
}

static void map105_MemoryWrite(uint32 addr, uint8 data)
{
	uint32 reg_num = (addr & 0x7FFF) >> 13;

	if (data & 0x80)
	{
		bits = write_count = 0;
		if (reg_num == 0)
		{
			regs[reg_num] |= 0x0C;
		}
	}
	else
	{
		bits |= (data & 1) << write_count++;
		if (write_count == 5)
		{
			regs[reg_num] = bits & 0x1F;
			bits = write_count = 0;
		}
	}

	if(regs[0] & 0x02)
	{
		if(regs[0] & 0x01)
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
	else
	{
		if(regs[0] & 0x01)
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

	switch (init_state)
	{
	case 0:
	case 1:
		{
			init_state++;
		}
		break;

	case 2:
		{
			if(regs[1] & 0x08)
			{
				if (regs[0] & 0x08)
				{
					if (regs[0] & 0x04)
					{
						mmc_bankrom4((regs[3] & 0x07) * 2 + 16);
						mmc_bankrom5((regs[3] & 0x07) * 2 + 17);
						mmc_bankrom6(30);
						mmc_bankrom7(31);
					}
					else
					{
						mmc_bankrom4(16);
						mmc_bankrom5(17);
						mmc_bankrom6((regs[3] & 0x07) * 2 + 16);
						mmc_bankrom7((regs[3] & 0x07) * 2 + 17);
					}
				}
				else
				{
					mmc_bankrom4((regs[3] & 0x06) * 2 + 16);
					mmc_bankrom5((regs[3] & 0x06) * 2 + 17);
					mmc_bankrom6((regs[3] & 0x06) * 2 + 18);
					mmc_bankrom7((regs[3] & 0x06) * 2 + 19);
				}
			}
			else
			{
				mmc_bankrom4((regs[1] & 0x06) * 2 + 0);
				mmc_bankrom5((regs[1] & 0x06) * 2 + 1);
				mmc_bankrom6((regs[1] & 0x06) * 2 + 2);
				mmc_bankrom7((regs[1] & 0x06) * 2 + 3);
			}

			if(regs[1] & 0x10)
			{
				irq_counter = 0;
				irq_enabled = 0;
			}
			else
			{
				irq_enabled = 1;
			}
		}
		break;
	}
}

static void map105_HSync(uint32 scanline)
{
	if(!scanline)
	{
		if(irq_enabled)
		{
			irq_counter += 29781;
		}
		if(((irq_counter | 0x21FFFFFF) & 0x3E000000) == 0x3E000000)
		{
			//parent_NES->cpu->DoIRQ();
			doIRQ();
		}
	}
}

static void map105_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper105.irqCounter[0] = (irq_counter &0x000000ff) >> 0;
	state->extraData.mapper105.irqCounter[1] = (irq_counter &0x0000ff00) >> 8;
	state->extraData.mapper105.irqCounter[2] = (irq_counter &0x00ff0000) >> 16;
	state->extraData.mapper105.irqCounter[3] = (irq_counter &0xff000000) >> 24;
	state->extraData.mapper105.irqCounterEnabled = irq_enabled;
	state->extraData.mapper105.InitialCounter = init_state;
	state->extraData.mapper105.WriteCounter = write_count;
	state->extraData.mapper105.Bits = bits;
	state->extraData.mapper105.registers[0] = regs[0];
	state->extraData.mapper105.registers[1] = regs[1];
	state->extraData.mapper105.registers[2] = regs[2];
	state->extraData.mapper105.registers[3] = regs[3];
}


static void map105_setstate(SnssMapperBlock *state)
{
	irq_counter = state->extraData.mapper105.irqCounter[0];
	irq_counter |= state->extraData.mapper105.irqCounter[1] << 8;
	irq_counter |= state->extraData.mapper105.irqCounter[2] << 16;
	irq_counter |= state->extraData.mapper105.irqCounter[3] << 24;
	irq_enabled = state->extraData.mapper105.irqCounterEnabled;
	init_state = state->extraData.mapper105.InitialCounter;
	write_count = state->extraData.mapper105.WriteCounter;
	bits = state->extraData.mapper105.Bits;
	regs[0] = state->extraData.mapper105.registers[0];
	regs[1] = state->extraData.mapper105.registers[1];
	regs[2] = state->extraData.mapper105.registers[2];
	regs[3] = state->extraData.mapper105.registers[3];
}


mapintf_t map105_intf =
{
   105, /* mapper number */
   "Map105", /* mapper name */
   map105_Reset, /* init routine */
   NULL, /* vblank callback */
   map105_HSync, /* hblank callback */
   map105_getstate, /* get state (snss) */
   map105_setstate, /* set state (snss) */   
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map105_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

