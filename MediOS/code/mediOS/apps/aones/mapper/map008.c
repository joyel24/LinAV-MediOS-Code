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
** map8.c
**
** mapper 8 interface
** $Id$
*/

#include "datatypes.h"
#include "unes_mapper.h"

/* mapper 8: FFE F3xxx -- what the hell uses this? */
static void map8_write(uint32 address, uint8 value)
{
   //UNUSED(address);

   mmc_bankrom(16, 0x8000, value >> 3);
   mmc_bankvrom(8, 0x0000, value & 7);
}

static void map8_init(void)
{
   mmc_bankrom(16, 0x8000, 0);
   mmc_bankrom(16, 0xC000, 1);
   mmc_bankvrom(8, 0x0000, 0);
}

mapintf_t map8_intf =
{
   8, /* mapper number */
   "FFE F3xxx", /* mapper name */
   map8_init, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   NULL, /* get state (snss) */
   NULL, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map8_write,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};

