#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"

extern int32 num_1k_VROM_banks;
extern int32 num_8k_ROM_banks;


static uint8  patch;
static uint8  regs[8];

//static uint8 wram_enabled;

static uint32 prg0,prg1;
static uint32 chr01,chr23,chr4,chr5,chr6,chr7;

static uint32 chr_swap() { return regs[0] & 0x80; }
static uint32 prg_swap() { return regs[0] & 0x40; }

static uint8 irq_enabled; // IRQs enabled
static uint8 irq_counter; // IRQ scanline counter, decreasing
static uint8 irq_latch;   // IRQ scanline counter latch
static uint8 irq_reset,latched;   // IRQ scanline counter latch

/////////////////////////////////////////////////////////////////////
// Mapper 4
// much of this is based on the DarcNES source. thanks, nyef :)



static void map4_MMC3_set_CPU_banks()
{
	if(prg_swap())
	{
		//set_CPU_banks(num_8k_ROM_banks-2,prg1,prg0,num_8k_ROM_banks-1);
		mmc_bankromALL4(num_8k_ROM_banks-2,prg1,prg0,num_8k_ROM_banks-1);
		
		
	}
	else
	{
		//set_CPU_banks(prg0,prg1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
		mmc_bankromALL4(prg0,prg1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);		
	}
}

static void map4_MMC3_set_PPU_banks()
{
	if(num_1k_VROM_banks)
	{
		if(chr_swap())
		{
			//set_PPU_banks(chr4,chr5,chr6,chr7,chr01,chr01+1,chr23,chr23+1);
			mmc_bankvromALL8(chr4,chr5,chr6,chr7,chr01,chr01+1,chr23,chr23+1);
		}
		else
		{
			//set_PPU_banks(chr01,chr01+1,chr23,chr23+1,chr4,chr5,chr6,chr7);
			mmc_bankvromALL8(chr01,chr01+1,chr23,chr23+1,chr4,chr5,chr6,chr7);			
		}
	}
	else
	{
		if(chr_swap())
		{
			mmc_VRAM_bank(0, chr4);
			mmc_VRAM_bank(1, chr5);
			mmc_VRAM_bank(2, chr6);
			mmc_VRAM_bank(3, chr7);
			mmc_VRAM_bank(4, chr01+0);
			mmc_VRAM_bank(5, chr01+1);
			mmc_VRAM_bank(6, chr23+0);
			mmc_VRAM_bank(7, chr23+1);
			
		}
		else
		{
			mmc_VRAM_bank(0, chr01+0);
			mmc_VRAM_bank(1, chr01+1);
			mmc_VRAM_bank(2, chr23+0);
			mmc_VRAM_bank(3, chr23+1);
			mmc_VRAM_bank(4, chr4);
			mmc_VRAM_bank(5, chr5);
			mmc_VRAM_bank(6, chr6);
			mmc_VRAM_bank(7, chr7);
		}
	}
}


static void map4_Reset(void)
{
	int i;
	patch = 0;

	if(mmc_getinfo()->var.crc == 0xdebea5a6  // Ninja Ryukenden 2 - Ankoku no Jashin Ken
	        ||mmc_getinfo()->var.crc == 0xc5fea9f2  // Dai2Ji - Super Robot Taisen	        
	        ||mmc_getinfo()->var.crc == 0xc43da8e2 //ninja gaiden 2 
	        )
	{
		patch = 1;
	}

	if(mmc_getinfo()->var.crc == 0xd7a97b38    // Chou Jinrou Senki - Warwolf	   
	)
	{
		patch = 2;
	}	
	/*
	  if(mmc_getinfo()->var.crc == 0xb42feeb4 ){
	    patch = 5;
	    parent_NES->frame_irq_disenabled = 1;
	  }
	*/
	ppu_mirror(0,1,2,3);
	
	irq_enabled = 0;
	irq_counter = 0;
	irq_latch = 0;
	
	irq_reset = 0;
	latched = 0;
	
	//wram_enabled = 0;
	mmc_getinfo()->var.sramEnabled = 0;

	// clear registers FIRST!!!
	for(i = 0; i < 8; i++) regs[i] = 0x00;

	// set CPU bank pointers
	prg0 = 0;
	prg1 = 1;
	map4_MMC3_set_CPU_banks();

	// set VROM banks
	if(num_1k_VROM_banks)
	{
		chr01 = 0;
		chr23 = 2;
		chr4  = 4;
		chr5  = 5;
		chr6  = 6;
		chr7  = 7;
		map4_MMC3_set_PPU_banks();
	}
	else
	{
		chr01 = chr23 = chr4 = chr5 = chr6 = chr7 = 0;
	}			
}

static void map4_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr & 0xE001)
	{
	case 0x8000:
		{
			regs[0] = data;
			map4_MMC3_set_PPU_banks();
			map4_MMC3_set_CPU_banks();
		}
		break;

	case 0x8001:
		{
			uint32 bank_num;

			regs[1] = data;
			bank_num = regs[1];

			switch(regs[0] & 0x07)
			{
			case 0x00:
				{
					//if(num_1k_VROM_banks)
					{
						bank_num &= 0xfe;
						chr01 = bank_num;
						map4_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x01:
				{
					//if(num_1k_VROM_banks)
					{
						bank_num &= 0xfe;
						chr23 = bank_num;
						map4_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x02:
				{
					//if(num_1k_VROM_banks)
					{
						chr4 = bank_num;
						map4_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x03:
				{
					//if(num_1k_VROM_banks)
					{
						chr5 = bank_num;
						map4_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x04:
				{
					//if(num_1k_VROM_banks)
					{
						chr6 = bank_num;
						map4_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x05:
				{
					//if(num_1k_VROM_banks)
					{
						chr7 = bank_num;
						map4_MMC3_set_PPU_banks();
					}
				}
				break;

			case 0x06:
				{
					prg0 = bank_num;
					map4_MMC3_set_CPU_banks();
				}
				break;

			case 0x07:
				{
					prg1 = bank_num;
					map4_MMC3_set_CPU_banks();
				}
				break;
			}
		}
		break;

	case 0xA000:
		{
			regs[2] = data;

			if(data & 0x40)
			{
				//LOG("MAP4 MIRRORING: 0x40 ???" << endl);
			}

			//if(parent_NES->ROM->get_mirroring() != NES_PPU::MIRROR_FOUR_SCREEN)
			if(mmc_getinfo()->var.mirroring != 5)
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
		}
		break;

	case 0xA001:
		{
			regs[3] = data;

			if(data & 0x80)
			{
				// enable save RAM $6000-$7FFF
				//wram_enabled=1;
				mmc_getinfo()->var.sramEnabled = 1;
			}
			else
			{
				// disable save RAM $6000-$7FFF
				//wram_enabled=0;
				mmc_getinfo()->var.sramEnabled = 0;
			}
		}
		break;

	case 0xC000:
		regs[4] = data;
		irq_latch = data;
		latched = irq_reset^1;
		
		if (irq_reset) 
		  irq_counter = data;
		break;

	case 0xC001:
		regs[5] = data;
		irq_counter = irq_latch;
		break;

	case 0xE000:
		regs[6] = data;
		irq_enabled = 0;
		irq_reset = 1;
		break;

	case 0xE001:
		regs[7] = data;
		irq_enabled = 1;
		
		if (latched)
		  irq_counter=irq_latch;
		break;

	default:
		//LOG("MAP4: UNKNOWN: " << HEX(addr,4) << " = " << HEX(data) << endl);
		break;

	}
}

static void map4_HSync(uint32 scanline)
{
	if(irq_enabled)
	{
		if ((scanline < 240)&&(ppu_enabled()))
			{
				irq_reset=0;
				if(patch == 1)
				{
					if(--irq_counter<=0)
					{
						irq_counter = irq_latch;
						//parent_NES->cpu->DoIRQ();
						doIRQ();
					}
				}
				else if(patch == 2)
				{
					if(--irq_counter <= 0x01)
					{
						irq_counter = irq_latch;
						//parent_NES->cpu->DoIRQ();
						doIRQ();
					}
				}
				else
				{
					if(irq_counter--<=0)
					{
						irq_counter = irq_latch;
						//parent_NES->cpu->DoIRQ();
						doIRQ();
					}
				}
			}
		
	}
}

#define MAP4_ROM(ptr)  (((ptr)-mmc_getinfo()->var.prg_beg)  >> 13)
#define MAP4_VROM(ptr) (((ptr)-mmc_getinfo()->var.chr_beg) >> 10)
#define MAP4_VRAM(ptr) (((ptr)-mmc_getinfo()->PPU_patterntables) >> 10)

static void map4_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper4.irqCounter=irq_counter;
	state->extraData.mapper4.irqLatchCounter=regs[4];
	state->extraData.mapper4.irqCounterEnabled=irq_enabled;
	state->extraData.mapper4.last8000Write=regs[0];	
}

static void map4_setstate(SnssMapperBlock *state)
{
	irq_counter = state->extraData.mapper4.irqCounter;
	regs[4] = state->extraData.mapper4.irqLatchCounter;
	irq_enabled = state->extraData.mapper4.irqCounterEnabled;
	regs[0] = state->extraData.mapper4.last8000Write;	
		

	prg0 = MAP4_ROM(mmc_getinfo()->CPUPageIndex[prg_swap() ? 6 : 4]);
	prg1 = MAP4_ROM(mmc_getinfo()->CPUPageIndex[5]);
	if(num_1k_VROM_banks)
	{
		if(chr_swap())
		{
			chr01 = MAP4_VROM(mmc_getinfo()->PPUPageIndex[4]);
			chr23 = MAP4_VROM(mmc_getinfo()->PPUPageIndex[6]);
			chr4  = MAP4_VROM(mmc_getinfo()->PPUPageIndex[0]);
			chr5  = MAP4_VROM(mmc_getinfo()->PPUPageIndex[1]);
			chr6  = MAP4_VROM(mmc_getinfo()->PPUPageIndex[2]);
			chr7  = MAP4_VROM(mmc_getinfo()->PPUPageIndex[3]);
		}
		else
		{
			chr01 = MAP4_VROM(mmc_getinfo()->PPUPageIndex[0]);
			chr23 = MAP4_VROM(mmc_getinfo()->PPUPageIndex[2]);
			chr4  = MAP4_VROM(mmc_getinfo()->PPUPageIndex[4]);
			chr5  = MAP4_VROM(mmc_getinfo()->PPUPageIndex[5]);
			chr6  = MAP4_VROM(mmc_getinfo()->PPUPageIndex[6]);
			chr7  = MAP4_VROM(mmc_getinfo()->PPUPageIndex[7]);
		}
	}
	else
	{
		if(chr_swap())
		{
			chr01 = MAP4_VRAM(mmc_getinfo()->PPUPageIndex[4]);
			chr23 = MAP4_VRAM(mmc_getinfo()->PPUPageIndex[6]);
			chr4  = MAP4_VRAM(mmc_getinfo()->PPUPageIndex[0]);
			chr5  = MAP4_VRAM(mmc_getinfo()->PPUPageIndex[1]);
			chr6  = MAP4_VRAM(mmc_getinfo()->PPUPageIndex[2]);
			chr7  = MAP4_VRAM(mmc_getinfo()->PPUPageIndex[3]);
		}
		else
		{
			chr01 = MAP4_VRAM(mmc_getinfo()->PPUPageIndex[0]);
			chr23 = MAP4_VRAM(mmc_getinfo()->PPUPageIndex[2]);
			chr4  = MAP4_VRAM(mmc_getinfo()->PPUPageIndex[4]);
			chr5  = MAP4_VRAM(mmc_getinfo()->PPUPageIndex[5]);
			chr6  = MAP4_VRAM(mmc_getinfo()->PPUPageIndex[6]);
			chr7  = MAP4_VRAM(mmc_getinfo()->PPUPageIndex[7]);
		}
	}
}
/////////////////////////////////////////////////////////////////////



mapintf_t map4_intf =
{
   4, /* mapper number */
   "MMC3", /* mapper name */
   map4_Reset, /* init routine */
   NULL, /* vblank callback */
   map4_HSync, /* hblank callback */
   map4_getstate, /* get state (snss) */
   map4_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map4_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};

