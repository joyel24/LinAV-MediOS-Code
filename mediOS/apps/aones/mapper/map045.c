#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;

static uint8 patch;

static uint8  regs[7];
static uint32 p[4],prg0,prg1,prg2,prg3;
static uint32 c[8],chr0,chr1,chr2,chr3,chr4,chr5,chr6,chr7;

static uint8 irq_enabled;
static uint8 irq_counter;
static uint8 irq_latch;
/////////////////////////////////////////////////////////////////////
// Mapper 45

static void MAP45_set_CPU_bank4(uint8 data)
{
	data &= (regs[3] & 0x3F) ^ 0xFF;
	data &= 0x3F;
	data |= regs[1];
	mmc_bankrom4(data);
	p[0] = data;
}

static void MAP45_set_CPU_bank5(uint8 data)
{
	data &= (regs[3] & 0x3F) ^ 0xFF;
	data &= 0x3F;
	data |= regs[1];
	mmc_bankrom5(data);
	p[1] = data;
}
static void MAP45_set_CPU_bank6(uint8 data)
{
	data &= (regs[3] & 0x3F) ^ 0xFF;
	data &= 0x3F;
	data |= regs[1];
	mmc_bankrom6(data);
	p[2] = data;
}
static void MAP45_set_CPU_bank7(uint8 data)
{
	data &= (regs[3] & 0x3F) ^ 0xFF;
	data &= 0x3F;
	data |= regs[1];
	mmc_bankrom7(data);
	p[3] = data;
}

static void MAP45_set_PPU_banks()
{
	uint8 i;
	uint8 table[16] =
	    {
	        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	        0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF
	    };
	c[0] = chr0;
	c[1] = chr1;
	c[2] = chr2;
	c[3] = chr3;
	c[4] = chr4;
	c[5] = chr5;
	c[6] = chr6;
	c[7] = chr7;
	for(i = 0; i < 8; i++)
	{
		c[i] &= table[regs[2] & 0x0F];
		c[i] |= regs[0] & ((patch == 0) ? 0xff : 0xC0);
		c[i] += (uint32)(regs[2] & ((patch == 0) ? 0x10 : 0x30)) << 4;
	}
	if(regs[6] & 0x80)
	{
//LOG("W4 " << HEX(c[4],2)<<" " << HEX(c[5],2)<<" " << HEX(c[6],2)<<" " << HEX(c[7],2)<<" " << HEX(c[0],2)<<" " << HEX(c[1],2)<<" " << HEX(c[2],2)<<" " << HEX(c[3],2) << endl);
		mmc_bankvromALL8(c[4],c[5],c[6],c[7],c[0],c[1],c[2],c[3]);
	}
	else
	{
//LOG("W4 " << " " << HEX(c[0],2)<<" " << HEX(c[1],2)<<" " << HEX(c[2],2)<<" " << HEX(c[3],2) << HEX(c[4],2)<<" " << HEX(c[5],2)<<" " << HEX(c[6],2)<<" " << HEX(c[7],2)<< endl);
		mmc_bankvromALL8(c[0],c[1],c[2],c[3],c[4],c[5],c[6],c[7]);
	}
}


static void map45_Reset()
{
	patch = 0;

	prg0 = 0;
	prg1 = 1;
	prg2 = num_8k_ROM_banks-2;
	prg3 = num_8k_ROM_banks-1;

	if(mmc_getinfo()->var.crc == 0x58bcacf6 || // Kunio 8-in-1 (Pirate Cart)
	        mmc_getinfo()->var.crc == 0x9103cfd6 || // HIK 7-in-1 (Pirate Cart)
	        mmc_getinfo()->var.crc == 0xc082e6d3)   // Super 8-in-1 (Pirate Cart)
	{
		patch = 1;
		prg2 = 62;
		prg3 = 63;
	}

	//set_CPU_banks(prg0,prg1,prg2,prg3);
	mmc_bankromALL4(prg0,prg1,prg2,prg3);
	p[0] = prg0;
	p[1] = prg1;
	p[2] = prg2;
	p[3] = prg3;

	// set PPU bank pointers
	//set_PPU_banks(0,1,2,3,4,5,6,7);
	chr0 = 0; chr1 = 1; chr2 = 2; chr3 = 3;
	chr4 = 4; chr5 = 5; chr6 = 6; chr7 = 7;
	c[0] = chr0; c[1] = chr1; c[2] = chr2; c[3] = chr3;
	c[4] = chr4; c[5] = chr5; c[6] = chr6; c[7] = chr7;

	irq_enabled = 0;
	irq_counter = 0;
	irq_latch = 0;

	regs[0] = regs[1] = regs[2] = regs[3] = 0;
	regs[4] = regs[5] = regs[6] = 0;
}

static void map45_MemoryWriteSaveRAM(uint32 addr, uint8 data)
{
	if(addr == 0x6000)
	{
		regs[regs[5]] = data;
		regs[5]= (regs[5]+1) & 0x03;
		MAP45_set_CPU_bank4(prg0);
		MAP45_set_CPU_bank5(prg1);
		MAP45_set_CPU_bank6(prg2);
		MAP45_set_CPU_bank7(prg3);
		MAP45_set_PPU_banks();
	}
}

static void map45_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr & 0xE001)
	{
	case 0x8000:
		{
			uint32 swap;
			if((data & 0x40) != (regs[6] & 0x40))
			{
				swap = prg0; prg0 = prg2; prg2 = swap;
				swap = p[0]; p[0] = p[2]; p[2] = swap;
				mmc_bankrom4(p[0]);
				mmc_bankrom6(p[2]);
			}
			if(num_8k_ROM_banks)
			{
				if((data & 0x80) != (regs[6] & 0x80))
				{
					swap = chr4; chr4 = chr0; chr0 = swap;
					swap = chr5; chr5 = chr1; chr1 = swap;
					swap = chr6; chr6 = chr2; chr2 = swap;
					swap = chr7; chr7 = chr3; chr3 = swap;
					swap = c[4]; c[4] = c[0]; c[0] = swap;
					swap = c[5]; c[5] = c[1]; c[1] = swap;
					swap = c[6]; c[6] = c[2]; c[2] = swap;
					swap = c[7]; c[7] = c[3]; c[3] = swap;
					mmc_bankvromALL8(c[0],c[1],c[2],c[3],c[4],c[5],c[6],c[7]);
				}
			}
			regs[6] = data;
		}
		break;

	case 0x8001:
		{
			switch(regs[6] & 0x07)
			{
			case 0x00:
				{
					chr0 = (data & 0xFE)+0;
					chr1 = (data & 0xFE)+1;
					MAP45_set_PPU_banks();
				}
				break;

			case 0x01:
				{
					chr2 = (data & 0xFE)+0;
					chr3 = (data & 0xFE)+1;
					MAP45_set_PPU_banks();
				}
				break;

			case 0x02:
				{
					chr4 = data;
					MAP45_set_PPU_banks();
				}
				break;

			case 0x03:
				{
					chr5 = data;
					MAP45_set_PPU_banks();
				}
				break;

			case 0x04:
				{
					chr6 = data;
					MAP45_set_PPU_banks();
				}
				break;

			case 0x05:
				{
					chr7 = data;
					MAP45_set_PPU_banks();
				}
				break;

			case 0x06:
				{
					if(regs[6] & 0x40)
					{
						prg2 = data & 0x3F;
						MAP45_set_CPU_bank6(data);
					}
					else
					{
						prg0 = data & 0x3F;
						MAP45_set_CPU_bank4(data);
					}
				}
				break;

			case 0x07:
				{
					prg1 = data & 0x3F;
					MAP45_set_CPU_bank5(data);
				}
				break;
			}
		}
		break;

	case 0xA000:
		{
			if(data & 0x01)
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

	case 0xC000:
		{
			irq_counter = data;
		}
		break;

	case 0xC001:
		{
			irq_latch = data;
		}
		break;

	case 0xE000:
		{
			irq_enabled = 0;
		}
		break;

	case 0xE001:
		{
			irq_enabled = 1;
		}
		break;
	}
}

static void map45_HSync(uint32 scanline)
{
	if(irq_enabled)
	{
		if((scanline >= 0) && (scanline <= 239))
		{
			//if(parent_NES->ppu->spr_enabled() || parent_NES->ppu->bg_enabled())
			if (ppu_enabled())
			{
				if(!(irq_counter--))
				{
					irq_counter = irq_latch;
					//parent_NES->cpu->DoIRQ();
					doIRQ();
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////

static void map45_getstate(SnssMapperBlock *state)
{
}

static void map45_setstate(SnssMapperBlock *state)
{
}


mapintf_t map45_intf =
{
   45, /* mapper number */
   "map45", /* mapper name */
   map45_Reset, /* init routine */
   NULL, /* vblank callback */
   map45_HSync, /* hblank callback */
   map45_getstate, /* get state (snss) */
   map45_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   map45_MemoryWriteSaveRAM,  /*Write saveram*/
   map45_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};
