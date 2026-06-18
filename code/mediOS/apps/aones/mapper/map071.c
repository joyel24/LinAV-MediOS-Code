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
** map4.c
**
** mapper 4 interface
** $Id$
*/

#include "datatypes.h"
#include "unes_mapper.h"
#include "unes.h"
#include "snss.h"
#include "unes_ppu.h"

/////////////////////////////////////////////////////////////////////
// Mapper 71
void map71_init()
{
	// set CPU bank pointers
	//set_CPU_banks(0,1,num_8k_ROM_banks-2,num_8k_ROM_banks-1);
}

void map71_writeSaveRAM(uint32 addr, uint8 data)
{
	/*set_CPU_bank4(data*2+0);
	set_CPU_bank5(data*2+1);*/
	mmc_bankrom(16,0x8000,data);
}

void map71_write(uint32 addr, uint8 data)
{
	switch(addr & 0xF000)
	{
	case 0x9000:
		{
			if(data & 0x10)
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
		break;

	case 0xC000:
	case 0xD000:
	case 0xE000:
	case 0xF000:
		{
			/*set_CPU_bank4(data*2+0);
			set_CPU_bank5(data*2+1);*/
			mmc_bankrom(16,0x8000,data);
		}
		break;
	}
}
/////////////////////////////////////////////////////////////////////

static void map71_getstate(SnssMapperBlock *state)
{
   
}

static void map71_setstate(SnssMapperBlock *state)
{
   
}

mapintf_t map71_intf =
{
   71, /* mapper number */
   "Camerica", /* mapper name */
   map71_init, /* init routine */
   NULL, /* vblank callback */
   NULL, /* hblank callback */
   map71_getstate, /* get state (snss) */
   map71_setstate, /* set state (snss) */
   NULL,  /*Read low*/
   NULL,  /*Read saveram or rom*/
   NULL,  /*Write high regs 0x4000-0x4017*/
   NULL,  /*Write low   0x4018-0x5FFF*/
   map71_writeSaveRAM,  /*Write saveram*/
   map71_write,  /*Write rom*/
   NULL,   /*Latch FDFE*/
   NULL,   /*Latch renderscreen*/
   NULL,   /*Latch Address*/
   NULL    /*SetBarcodeValue*/   
};

