/* 
*   apps/avboy/fastmem.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
* Gameboy / Color Gameboy emulator (port of gnuboy)
* 
*  Date:     18/10/2005
* Author:   GliGli

*  Modified by CjNr11 06/12/2005
*/

#ifndef __FASTMEM_H__
#define __FASTMEM_H__

#include "mem.h"

byte readb(int a);
void writeb(int a, byte b);
int readw(int a);
void writew(int a, int w);
byte readhi(int a);
void writehi(int a, byte b);

#if 0
byte readhi(int a)
{
	byte (*rd)() = hi_read[a];
	return rd ? rd(a) : (ram.hi[a] | himask[a]);
}

void writehi(int a, byte b)
{
	byte (*wr)() = hi_write[a];
	if (wr) wr(a, b);
	else ram.hi[a] = b & ~himask[a];
}
#endif


#endif
