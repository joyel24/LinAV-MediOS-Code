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
** map24.c
**
** mapper 24 interface
** $Id$
*/

#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"


static struct
{
   int counter, enabled;
   int latch, wait_state;
} irq;

//static uint8 patch;

static void map24_init(void)
{
   irq.counter = irq.enabled = 0;
   irq.latch = irq.wait_state = 0;
   
   //exsound apu_setexchip(1); //VRC6
}

static void map24_hblank(uint32 scanline) 
{
   if (irq.enabled)
   {
      if (256 == ++irq.counter)
      {
         irq.counter = irq.latch;
         #ifdef __asmcpu__
      	 cpu_irq();
  
      	 #else
      	 nes6502_irq();
	 #endif               
         //irq.enabled = false;
         irq.enabled = irq.wait_state;
      }
   }
}

static void map24_write(uint32 address, uint8 value)
{
   switch (address & 0xF003)
   {
   case 0x8000:
      mmc_bankrom(16, 0x8000, value);
      break;

   case 0x9003:
      /* ??? */
      break;
   
   case 0xB003:
      switch (value & 0x0C)
      {
      case 0x00:
         ppu_mirror(0, 1, 0, 1); /* vertical */
         break;
      
      case 0x04:
         ppu_mirror(0, 0, 1, 1); /* horizontal */
         break;
      
      case 0x08:
         ppu_mirror(0, 0, 0, 0);
         break;
      
      case 0x0C:
         ppu_mirror(1, 1, 1, 1);
         break;
      
      default:
         break;
      }
      break;
   

   case 0xC000:
      mmc_bankrom(8, 0xC000, value);
      break;
   
   case 0xD000:
      mmc_bankvrom(1, 0x0000, value);
      break;
   
   case 0xD001:
      mmc_bankvrom(1, 0x0400, value);
      break;
   
   case 0xD002:
      mmc_bankvrom(1, 0x0800, value);
      break;
   
   case 0xD003:
      mmc_bankvrom(1, 0x0C00, value);
      break;
   
   case 0xE000:
      mmc_bankvrom(1, 0x1000, value);
      break;
   
   case 0xE001:
      mmc_bankvrom(1, 0x1400, value);
      break;
   
   case 0xE002:
      mmc_bankvrom(1, 0x1800, value);
      break;
   
   case 0xE003:
      mmc_bankvrom(1, 0x1C00, value);
      break;
   
   case 0xF000:
      irq.latch = value;
      break;
   
   case 0xF001:
      irq.enabled = (value >> 1) & 0x01;
      irq.wait_state = value & 0x01;
      if (irq.enabled)
         irq.counter = irq.latch;
      break;
   
   case 0xF002:
      irq.enabled = irq.wait_state;
      break;
   }
   
   //exsound ex_write(address, value);
}

static void map24_getstate(SnssMapperBlock *state)
{
   state->extraData.mapper24.irqCounter = irq.counter;
   state->extraData.mapper24.irqCounterEnabled = irq.enabled;
   state->extraData.mapper24.irqLatchCounter = irq.latch;      
}

static void map24_setstate(SnssMapperBlock *state)
{
   irq.counter = state->extraData.mapper24.irqCounter;
   irq.enabled = state->extraData.mapper24.irqCounterEnabled;
   irq.latch = state->extraData.mapper24.irqLatchCounter;
}

mapintf_t map24_intf =
{
   24, /* mapper number */
   "Konami VRC6", /* mapper name */
   map24_init, /* init routine */
   NULL, /* vblank callback */
   map24_hblank, /* hblank callback */
   map24_getstate, /* get state (snss) */
   map24_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map24_write,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};
