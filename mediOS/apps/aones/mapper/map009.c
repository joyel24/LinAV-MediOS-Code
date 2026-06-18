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
** map9.c
**
** mapper 9 interface
** $Id$
*/

#include "datatypes.h"
#include "unes_mapper.h"
#include "unes_ppu.h"
#include "snss.h"

#ifdef __PALM__
//#include "string.h"
#endif

static uint8 latch[2];
static uint8 regs[4];

/* Used when tile $FD/$FE is accessed */
static void mmc9_latchfunc(uint32 address)
{	
      int reg;
      uint8 value=(address>>4);   
      
      if (address&0x1000)
      {
         latch[1] = value;
         reg = 2 + (value - 0xFD);
      }
      else
      {
         latch[0] = value;
         reg = value - 0xFD;
      }

      mmc_bankvrom(4, (address&0x1000), regs[reg]);        
}

/* mapper 9: MMC2 */
/* MMC2: Punch-Out! */
static void map9_write(uint32 address, uint8 value)
{
   switch ((address & 0xF000) >> 12)
   {
   case 0xA:
      mmc_bankrom(8, 0x8000, value);
      break;

   case 0xB:
      regs[0] = value;
      if (0xFD == latch[0])
         mmc_bankvrom(4, 0x0000, value);
      break;

   case 0xC:
      regs[1] = value;
      if (0xFE == latch[0])
         mmc_bankvrom(4, 0x0000, value);
      break;

   case 0xD:
      regs[2] = value;
      if (0xFD == latch[1])
         mmc_bankvrom(4, 0x1000, value);
      break;

   case 0xE:
      regs[3] = value;
      if (0xFE == latch[1])
         mmc_bankvrom(4, 0x1000, value);
      break;

   case 0xF:
      if (value & 1)
         ppu_mirror(0, 0, 1, 1); /* horizontal */
      else
         ppu_mirror(0, 1, 0, 1); /* vertical */
      break;

   default:
      break;
   }
}

static void map9_init(void)
{
   memset(regs, 0, sizeof(regs));

   mmc_bankrom(8, 0x8000, 0);
   mmc_bankrom(8, 0xA000, (mmc_getinfo()->ROM_Header.prg_rom_pages_nb * 2) - 3);
   mmc_bankrom(8, 0xC000, (mmc_getinfo()->ROM_Header.prg_rom_pages_nb * 2) - 2);
   mmc_bankrom(8, 0xE000, (mmc_getinfo()->ROM_Header.prg_rom_pages_nb * 2) - 1);

   latch[0] = 0xFE;
   latch[1] = 0xFE;

   ppu_set_Latch_FDFE( mmc9_latchfunc);
}

static void map9_getstate(SnssMapperBlock *state)
{
   state->extraData.mapper9.latch[0] = latch[0];
   state->extraData.mapper9.latch[1] = latch[1];
   state->extraData.mapper9.lastB000Write = regs[0];
   state->extraData.mapper9.lastC000Write = regs[1];
   state->extraData.mapper9.lastD000Write = regs[2];
   state->extraData.mapper9.lastE000Write = regs[3];
}

static void map9_setstate(SnssMapperBlock *state)
{
   latch[0] = state->extraData.mapper9.latch[0];
   latch[1] = state->extraData.mapper9.latch[1];
   regs[0] = state->extraData.mapper9.lastB000Write;
   regs[1] = state->extraData.mapper9.lastC000Write;
   regs[2] = state->extraData.mapper9.lastD000Write;
   regs[3] = state->extraData.mapper9.lastE000Write;
}

mapintf_t map9_intf =
{
   9, /* mapper number */
   "MMC2", /* mapper name */
   map9_init, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map9_getstate, /* get state (snss) */
   map9_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map9_write,  /*Write rom*/
   mmc9_latchfunc,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/         
};

