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
** map3.c
**
** mapper 3 interface
** $Id$
*/

#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "unes_ppu.h"


extern uint32 num_1k_VROM_banks;
extern uint32 num_8k_ROM_banks;

void map3_Reset(void)
{
	// set CPU bank pointers
	if(num_8k_ROM_banks > 2)
	{
		mmc_bankromALL4(0,1,2,3);
	}
	else
	{
		mmc_bankromALL4(0,1,0,1);
	}

	// set VROM banks
	mmc_bankvromALL8(0,1,2,3,4,5,6,7);
}

void map3_MemoryWrite(uint32 addr, uint8 data)
{
	uint32 base;
	data &= (num_1k_VROM_banks>>1)-1;

	base = ((uint32)data) << 3;
//LOG("W4 " << HEX(base,4)<< endl);
	mmc_bankvromALL8(base+0,base+1,base+2,base+3,base+4,base+5,base+6,base+7);
}


mapintf_t map3_intf =
{
   3, /* mapper number */
   "CNROM", /* mapper name */
   map3_Reset, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   NULL, /* get state (snss) */
   NULL, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   NULL,  /*Write saveram*/
   map3_MemoryWrite,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};
