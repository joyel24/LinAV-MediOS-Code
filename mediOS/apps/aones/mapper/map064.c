/*
** Nofrendo (c) 1998-2000 Matthew Conte (matt@conte.com)
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of version 2 of the GNU Library General 
** Public License as published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** map64.c
**
** mapper 64 interface
** $Id$
*/

#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
//#include <log.h>
#include "unes_ppu.h"

static struct
{
   int counter, latch;
   bool enabled;
} irq;

static uint8 regs[3];

static void map64_hblank(uint32 scanline)
{   
   if (irq.enabled)
   { 
   	if((scanline >= 0) && (scanline <= 239))
   	{
	  if (ppu_enabled())
   	  {
      		if (!(--irq.counter))
      		{
         		irq.counter = irq.latch;       		
            		#ifndef __asmcpu__
               		nes6502_irq();
               		#else
               		cpu_irq();
               		#endif
         	}
          }
        }
   } 
}

/* mapper 64: Tengen RAMBO-1 */
static void map64_write(uint32 addr, uint8 data)
{
	switch(addr & 0xF003)
	{
	case 0x8000:
		{
			regs[0] = data & 0x0F;
			regs[1] = data & 0x40;
			regs[2] = data & 0x80;
		}
		break;

	case 0x8001:
		{
			switch(regs[0])
			{
			case 0x00:
				{
					if(regs[2])
					{
						/*set_PPU_bank4(data+0);
						set_PPU_bank5(data+1);*/
						mmc_bankvrom(2, 0x1000, data>>1);         					
					}
					else
					{
						/*set_PPU_bank0(data+0);
						set_PPU_bank1(data+1);*/
						mmc_bankvrom(2, 0x0000 , data>>1);         					
					}
				}
				break;

			case 0x01:
				{
					if(regs[2])
					{
						/*set_PPU_bank6(data+0);
						set_PPU_bank7(data+1);*/
						mmc_bankvrom(2, 0x1800 , data>>1);						
					}
					else
					{
						/*set_PPU_bank2(data+0);
						set_PPU_bank3(data+1);*/
						mmc_bankvrom(2, 0x0800 , data>>1);						
					}
				}
				break;

			case 0x02:
				{
					if(regs[2])
					{
						//set_PPU_bank0(data);
						mmc_bankvrom(1, 0x0000 , data);
					}
					else
					{
						//set_PPU_bank4(data);
						mmc_bankvrom(1, 0x1000 , data);
					}
				}
				break;

			case 0x03:
				{
					if(regs[2])
					{
						//set_PPU_bank1(data);
						mmc_bankvrom(1, 0x0400 , data);
					}
					else
					{
						//set_PPU_bank5(data);
						mmc_bankvrom(1, 0x1400 , data);
					}
				}
				break;

			case 0x04:
				{
					if(regs[2])
					{
						//set_PPU_bank2(data);
						mmc_bankvrom(1, 0x0800 , data);
					}
					else
					{
						//set_PPU_bank6(data);
						mmc_bankvrom(1, 0x1800 , data);
					}
				}
				break;

			case 0x05:
				{
					if(regs[2])
					{
						//set_PPU_bank3(data);
						mmc_bankvrom(1, 0x0C00 , data);
					}
					else
					{
						//set_PPU_bank7(data);
						mmc_bankvrom(1, 0x1C00 , data);
					}
				}
				break;

			case 0x06:
				{
					if(regs[1])
					{
						//set_CPU_bank5(data);
						mmc_bankrom(8, 0xA000 , data);
					}
					else
					{
						//set_CPU_bank4(data);
						mmc_bankrom(8, 0x8000 , data);
					}
				}
				break;

			case 0x07:
				{
					if(regs[1])
					{
						//set_CPU_bank6(data);
						mmc_bankrom(8, 0xC000 , data);
					}
					else
					{
						//set_CPU_bank5(data);
						mmc_bankrom(8, 0xA000 , data);
					}
				}
				break;

			case 0x08:
				{
					//if(regs[2])
					//{
					//  set_PPU_bank5(data);
					//}
					//else
					{
						//set_PPU_bank1(data);
						mmc_bankvrom(1, 0x0400 , data);
					}
				}
				break;

			case 0x09:
				{
					//if(regs[2])
					//{
					//  set_PPU_bank7(data);
					//}
					//else
					{
						//set_PPU_bank3(data);
						mmc_bankvrom(1, 0x0C00 , data);
					}
				}
				break;

			case 0x0F:
				{
					if(regs[1])
					{
						//set_CPU_bank4(data);
						mmc_bankrom(8, 0x8000 , data);
					}
					else
					{
						//set_CPU_bank6(data);
						mmc_bankrom(8, 0xC000 , data);
					}
				}
				break;
			}
		}
		break;

	case 0xA000:
		{
			if(!(data & 0x01))
			{
				//set_mirroring(NES_PPU::MIRROR_VERT);
				ppu_mirror(0, 1, 0, 1);
			}
			else
			{
				ppu_mirror(0, 0, 1, 1);
			}
		}
		break;

	case 0xC000:
		{
			irq.latch = data;
			irq.counter = irq.latch;
		}
		break;

	case 0xC001:
		{
			irq.counter = irq.latch;
		}
		break;

	case 0xE000:
		{
			irq.enabled = 0;
			irq.counter = irq.latch;
		}
		break;

	case 0xE001:
		{
			irq.enabled = 1;
			irq.counter = irq.latch;
		}
		break;
	}
	   
}

static void map64_init(void)
{
   mmc_bankrom(8, 0x8000, MMC_LASTBANK);
   mmc_bankrom(8, 0xA000, MMC_LASTBANK);
   mmc_bankrom(8, 0xC000, MMC_LASTBANK);
   mmc_bankrom(8, 0xE000, MMC_LASTBANK);

   irq.counter = irq.latch = 0;
   irq.enabled = false;
   
   regs[0] = 0;
   regs[1] = 0;
   regs[2] = 0;
}

static void map64_getstate(SnssMapperBlock *state)
{
	state->extraData.mapper64.irqCounter = irq.counter;
	state->extraData.mapper64.irqCounterEnabled = irq.enabled;
	state->extraData.mapper64.irqLatchCounter = irq.latch;
	state->extraData.mapper64.last8000Write = regs[0] | regs[1] | regs[2];
}

static void map64_setstate(SnssMapperBlock *state)
{
	irq.counter = state->extraData.mapper64.irqCounter;
	irq.enabled = state->extraData.mapper64.irqCounterEnabled;
	irq.latch = state->extraData.mapper64.irqLatchCounter;
	regs[0] = state->extraData.mapper64.last8000Write & 0x0F;
	regs[1] = state->extraData.mapper64.last8000Write & 0x40;
	regs[2] = state->extraData.mapper64.last8000Write & 0x80;
}

mapintf_t map64_intf =
{
   64, /* mapper number */
   "Tengen RAMBO-1", /* mapper name */
   map64_init, /* init routine */
   NULL, /* vblank callback */
   map64_hblank, /* hblank callback */
   map64_getstate, /* get state (snss) */
   map64_setstate, /* set state (snss) */
   NULL,  /*Read low*/   
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map64_write,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};

