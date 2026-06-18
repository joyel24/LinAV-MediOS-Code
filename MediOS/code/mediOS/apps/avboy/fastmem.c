/* 
*   apps/avboy/fastmem.c
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

#include "defs.h"
#include "fastmem.h"


#define D 0 /* direct */
#define C 1 /* direct cgb-only */
#define R 2 /* io register */
#define S 3 /* sound register */
#define W 4 /* wave pattern */

#define F 0xFF /* fail */

const byte himask[256];

const byte hi_rmap[256] =
{
	0, 0, R, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S,
	S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, C, 0, C,
	0, C, C, C, C, C, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, C, C, C, C, 0, 0, 0, 0,
	C, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const byte hi_wmap[256] =
{
	R, R, R, R, R, R, R, R, R, R, R, R, R, R, R, R,
	S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S,
	S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S,
	S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S,
	R, R, R, R, R, R, R, R, R, R, R, R, 0, R, 0, R,
	0, C, C, C, C, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, R, R, R, R, 0, 0, 0, 0,
	R, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, R
};


__IRAM_CODE byte readb(int a)
{
	byte *p = mbc.rmap[a>>12];
	if (p) return p[a];
	else return mem_read(a);
}

__IRAM_CODE void writeb(int a, byte b)
{
	byte *p = mbc.wmap[a>>12];
	if (p) p[a] = b;
	else mem_write(a, b);
}

__IRAM_CODE int readw(int a)
{
	if ((a+1) & 0xfff)
	{
		byte *p = mbc.rmap[a>>12];
		if (p)
		{
			if (a&1) return p[a] | (p[a+1]<<8);
			return *(word *)(p+a);
//			return p[a] | (p[a+1]<<8);
		}
	}
	return mem_read(a) | (mem_read(a+1)<<8);
}

__IRAM_CODE void writew(int a, int w)
{
	if ((a+1) & 0xfff)
	{
		byte *p = mbc.wmap[a>>12];
		if (p)
		{
			if (a&1)
			{
				p[a] = w;
				p[a+1] = w >> 8;
				return;
			}
			*(word *)(p+a) = w;
			return;
//			p[a] = w;
//    	p[a+1] = w >> 8;
//			return;
//#endif
		}
	}
	mem_write(a, w);
	mem_write(a+1, w>>8);
}

__IRAM_CODE byte readhi(int a)
{
	return readb(a | 0xff00);
}

__IRAM_CODE void writehi(int a, byte b)
{
	writeb(a | 0xff00, b);
}

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
