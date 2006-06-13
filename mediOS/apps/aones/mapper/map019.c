#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"

static uint8 patch;

static uint8 regs[3];
static uint8 irq_enabled;
static uint32 irq_counter;
static uint32 irq_sn;

/////////////////////////////////////////////////////////////////////
// Mapper 19
static void map19_Reset(void)
{
	patch = 0;
	irq_sn=113;

	if(mmc_getinfo()->var.crc == 0x761ccfb5 )//|| // Digital Devil Story - Megami Tensei 2
		//	  mmc_getinfo()->var.crc == 0x3deac303)  // RollingThunder(J)
	{
		patch = 1;
		irq_sn = 112;
	}
	else if(mmc_getinfo()->var.crc == 0x96533999 || // Dokuganryuu Masamune
	        mmc_getinfo()->var.crc == 0x429fd177 || // Famista '90
	        mmc_getinfo()->var.crc == 0xdd454208 || // Hydlide 3 - Yami Kara no Houmonsha (J).nes
	        mmc_getinfo()->var.crc == 0xb1b9e187 || // Kaijuu Monogatari
	        mmc_getinfo()->var.crc == 0xaf15338f)   // Mindseeker
	{
		patch = 2;
	}
	if(mmc_getinfo()->var.crc == 0xb62a7b71) // Family Circuit '91(J)
	{
		irq_sn=100;
	}


	// Init ExSound
	//parent_NES->apu->SelectExSound(16);
	//exsound apu_setexchip(16);

	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);

	// set PPU bank pointers
	if(mmc_getinfo()->ROM_Header.chr_rom_pages_nb >= 8)
	{		
		/*set_PPU_bank0(num_1k_VROM_banks-8);
		set_PPU_bank1(num_1k_VROM_banks-7);
		set_PPU_bank2(num_1k_VROM_banks-6);
		set_PPU_bank3(num_1k_VROM_banks-5);
		set_PPU_bank4(num_1k_VROM_banks-4);
		set_PPU_bank5(num_1k_VROM_banks-3);
		set_PPU_bank6(num_1k_VROM_banks-2);
		set_PPU_bank7(num_1k_VROM_banks-1);*/
		mmc_bankvrom(1,0x0000,mmc_getinfo()->ROM_Header.chr_rom_pages_nb-8);
		mmc_bankvrom(1,0x0400,mmc_getinfo()->ROM_Header.chr_rom_pages_nb-7);
		mmc_bankvrom(1,0x0800,mmc_getinfo()->ROM_Header.chr_rom_pages_nb-6);
		mmc_bankvrom(1,0x0C00,mmc_getinfo()->ROM_Header.chr_rom_pages_nb-5);
		mmc_bankvrom(1,0x1000,mmc_getinfo()->ROM_Header.chr_rom_pages_nb-4);
		mmc_bankvrom(1,0x1400,mmc_getinfo()->ROM_Header.chr_rom_pages_nb-3);
		mmc_bankvrom(1,0x1800,mmc_getinfo()->ROM_Header.chr_rom_pages_nb-2);
		mmc_bankvrom(1,0x1C00,mmc_getinfo()->ROM_Header.chr_rom_pages_nb-1);
	}

	regs[0] = 0;
	regs[1] = 0;
	regs[2] = 0;
}

static uint8 map19_MemoryReadLow(uint32 addr)
{
	if(addr == 0x4800)
	{
		if(patch == 2)
		{
			//uint8 retval =  parent_NES->ReadSaveRAM(regs[2] & 0x7F);
			uint8 retval =  mmc_getinfo()->NESSRAM[regs[2] & 0x7F];
			if(regs[2] & 0x80) regs[2] = ((regs[2] & 0x7F)+1) | 0x80;
			return retval;
		}
		else
		{
			//return parent_NES->apu->ExRead(addr);
			return 0; //exsound ex_read(addr);
		}
	}
	else if((addr & 0xF800) == 0x5000) // addr $5000-$57FF
	{
		return (uint8)(irq_counter & 0x00FF);
	}
	else if ((addr & 0xF800) == 0x5800) // addr $5800-$5FFF
	{
		return (uint8)((irq_counter & 0x7F00) >> 8);
	}
	else
	{
		return (uint8)(addr >> 8);
	}
}

static void map19_MemoryWriteLow(uint32 addr, uint8 data)
{
	switch(addr & 0xF800)
	{
	case 0x4800:
		{
			if(addr == 0x4800)
			{
				if(patch == 2)
				{
					//parent_NES->WriteSaveRAM(regs[2] & 0x7F, data);
					mmc_getinfo()->NESSRAM[regs[2] & 0x7F]=data;
					if(regs[2] & 0x80) regs[2] = ((regs[2] & 0x7F)+1) | 0x80;
				}
				else
				{
					//parent_NES->apu->ExWrite(addr, data);
					//exsound ex_write(addr,data);
				}
			}
		}
		break;

	case 0x5000: // addr $5000-$57FF
		{
			irq_counter = (irq_counter & 0xFF00) | data;
		}
		break;

	case 0x5800: // addr $5000-$57FF
		{
			irq_counter = (irq_counter & 0x00FF) | ((uint32)(data & 0x7F) << 8);
			irq_enabled = (data & 0x80) >> 7;
			if(patch)
			{
				irq_counter ++;
			}
		}
		break;
	}
}

static void map19_MemoryWrite(uint32 addr, uint8 data)
{
	switch(addr & 0xF800)
	{
	case 0x8000: // addr $8000-$87FF
		{
			if(data < 0xE0 || regs[0] == 1)
			{
				//set_PPU_bank0(data);
				mmc_bankvrom(1,0x000,data);
			}
			else
			{
				//set_VRAM_bank(0, 0);
				mmc_VRAM_bank(0,0);
			}
		}
		break;

	case 0x8800: // addr $8800-$8FFF
		{
			if(data < 0xE0 || regs[0] == 1)
			{
				//set_PPU_bank1(data);
				mmc_bankvrom(1,0x400,data);
			}
			else
			{
				//set_VRAM_bank(1, 1);
				mmc_VRAM_bank(1, 1);
			}
		}
		break;

	case 0x9000: // addr $9000-$97FF
		{
			if(data < 0xE0 || regs[0] == 1)
			{
				//set_PPU_bank2(data);
				mmc_bankvrom(1,0x800,data);
			}
			else
			{
				//set_VRAM_bank(2, 2);
				mmc_VRAM_bank(2, 2);
			}
		}
		break;

	case 0x9800: // addr $9800-$9FFF
		{
			if(data < 0xE0 || regs[0] == 1)
			{
				//set_PPU_bank3(data);
				mmc_bankvrom(1,0xC00,data);
			}
			else
			{
				//set_VRAM_bank(3, 3);
				mmc_VRAM_bank(3, 3);
			}
		}
		break;

	case 0xA000: // addr $A000-$A7FF
		{
			if(data < 0xE0 || regs[1] == 1)
			{
				//set_PPU_bank4(data);
				mmc_bankvrom(1,0x1000,data);
			}
			else
			{
				//set_VRAM_bank(4, 4);
				mmc_VRAM_bank(4, 4);
			}
		}
		break;

	case 0xA800: // addr $A800-$AFFF
		{
			if(data < 0xE0 || regs[1] == 1)
			{
				//set_PPU_bank5(data);
				mmc_bankvrom(1,0x1400,data);
			}
			else
			{
				//set_VRAM_bank(5, 5);
				mmc_VRAM_bank(5, 5);
			}
		}
		break;

	case 0xB000: // addr $B000-$B7FF
		{
			if(data < 0xE0 || regs[1] == 1)
			{
				//set_PPU_bank6(data);
				mmc_bankvrom(1,0x1800,data);
			}
			else
			{
				//set_VRAM_bank(6, 6);
				mmc_VRAM_bank(6, 6);
			}
		}
		break;

	case 0xB800: // addr $B800-$BFFF
		{
			if(data < 0xE0 || regs[1] == 1)
			{
				//set_PPU_bank7(data);
				mmc_bankvrom(1,0x1C00,data);
			}
			else
			{
				//set_VRAM_bank(7, 7);
				mmc_VRAM_bank(7, 7);
			}
		}
		break;

	case 0xC000: // addr $C000-$C7FF
		{
			if(data <= 0xDF)
			{
				//set_PPU_bank8(data);
				mmc_bankvrom(1,0x2000,data);
			}
			else
			{
				//set_VRAM_bank(8, data & 0x01);
				mmc_VRAM_bank(8, data & 0x01);
			}
		}
		break;

	case 0xC800: // addr $C800-$CFFF
		{
			if(data <= 0xDF)
			{
				//set_PPU_bank9(data);
				mmc_bankvrom(1,0x2400,data);
			}
			else
			{
				//set_VRAM_bank(9, data & 0x01);
				mmc_VRAM_bank(9, data & 0x01);
			}
		}
		break;

	case 0xD000: // addr $D000-$D7FF
		{
			if(data <= 0xDF)
			{
				//set_PPU_bank10(data);
				mmc_bankvrom(1,0x2800,data);
			}
			else
			{
				//set_VRAM_bank(10, data & 0x01);
				mmc_VRAM_bank(10, data & 0x01);
			}
		}
		break;

	case 0xD800: // addr $D800-$DFFF
		{
			if(data <= 0xDF)
			{
				//set_PPU_bank11(data);
				mmc_bankvrom(1,0x2C00,data);
			}
			else
			{
				//set_VRAM_bank(11, data & 0x01);
				mmc_VRAM_bank(11, data & 0x01);
			}
		}
		break;

	case 0xE000: // addr $E000-$E7FF
		{
			//set_CPU_bank4(data & 0x3F);
			mmc_bankrom(8,0x8000,data & 0x3F);
		}
		break;

	case 0xE800: // addr $E800-$EFFF
		{
			//set_CPU_bank5(data & 0x3F);
			mmc_bankrom(8,0xA000,data & 0x3F);
			regs[0] = (data & 0x40) >> 6;
			regs[1] = (data & 0x80) >> 7;
		}
		break;

	case 0xF000: // addr $F000-$F7FF
		{
			//set_CPU_bank6(data & 0x3F);
			mmc_bankrom(8,0xC000,data & 0x3F);
		}
		break;

	case 0xF800:
		{
			if(addr == 0xF800)
			{
				if(patch == 2)
				{
					regs[2] = data;
				}
				else
				{
					//parent_NES->apu->ExWrite(addr, data);
					//exsound ex_write(addr,data);
				}
			}
		}
		break;
	}
}

static void map19_HSync(uint32 scanline)
{
	if(irq_enabled)
	{
		if(irq_counter >= (uint32)(0x7FFF - irq_sn))
		{
			irq_counter = 0x7FFF;
			//parent_NES->cpu->DoIRQ();
			doIRQ();
		}
		else
		{
			irq_counter += irq_sn;
		}
	}
}

static void map19_getstate(SnssMapperBlock *state)
{
   state->extraData.mapper19.irqCounterLowByte = irq_counter & 0x00FF;
   state->extraData.mapper19.irqCounterHighByte = (irq_counter & 0xFF00) >> 8;
   state->extraData.mapper19.irqCounterEnabled = irq_enabled;   
   state->extraData.mapper19.lastE800Write = (regs[0] & 0x01) << 6;
   state->extraData.mapper19.lastE800Write |= (regs[1] & 0x01) << 7;
   state->extraData.mapper19.lastF800Write = regs[2];
}

static void map19_setstate(SnssMapperBlock *state)
{
   irq_counter = state->extraData.mapper19.irqCounterLowByte;
   irq_counter |= state->extraData.mapper19.irqCounterHighByte << 8;
   irq_enabled = state->extraData.mapper19.irqCounterEnabled;   
   regs[0] = (state->extraData.mapper19.lastE800Write & 0x40) >> 6;
   regs[1] = (state->extraData.mapper19.lastE800Write & 0x80) >> 7;
   regs[2] = state->extraData.mapper19.lastF800Write;
}
   

/////////////////////////////////////////////////////////////////////

mapintf_t map19_intf =
{
   19, /* mapper number */
   "Namcot 106", /* mapper name */
   map19_Reset, /* init routine */
   NULL, /* vblank callback */
   map19_HSync, /* hblank callback */
   map19_getstate, /* get state (snss) */
   map19_setstate, /* set state (snss) */
   map19_MemoryReadLow,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   map19_MemoryWriteLow,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map19_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};
