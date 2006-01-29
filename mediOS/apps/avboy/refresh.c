/*
*   apps/avboy/refresh.c
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
#include "lcd.h"

#define BUF (scan.buf)

#ifdef USE_ASM
#include "asm.h"
#endif

#ifdef AV3XX
extern int RotScreen;
#endif

#ifndef ASM_REFRESH_1
__IRAM_CODE void refresh_1(byte *dest, byte *src, byte *pal, int cnt)
{
#ifdef AV3XX
	if(RotScreen==2) {
           dest+=143;
           while(cnt--) {*dest = pal[*(src++)]; dest+=160;}
        }
        else if(RotScreen==1) {
           src+=cnt-1;
	   while(cnt--) {*dest = pal[*(src--)]; dest+=160;}
        }
        else {
#endif
           while(cnt--) *(dest++) = pal[*(src++)];
#ifdef AV3XX
        }
#endif
}
#endif

#ifndef ASM_REFRESH_2
void refresh_2(un16 *dest, byte *src, un16 *pal, int cnt)
{
	while (cnt--) *(dest++) = pal[*(src++)];
}
#endif

#ifndef ASM_REFRESH_3
void refresh_3(byte *dest, byte *src, un32 *pal, int cnt)
{
	un32 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c>>8;
		*(dest++) = c>>16;
	}
}
#endif

#ifndef ASM_REFRESH_4
void refresh_4(un32 *dest, byte *src, un32 *pal, int cnt)
{
	while (cnt--) *(dest++) = pal[*(src++)];
}
#endif




#ifndef ASM_REFRESH_1_2X
#ifdef AV3XX 
__IRAM_CODE 
#endif
void refresh_1_2x(byte *dest, byte *src, byte *pal, int cnt)
{
	byte c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

#ifndef ASM_REFRESH_2_2X
void refresh_2_2x(un16 *dest, byte *src, un16 *pal, int cnt)
{
	un16 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

#ifndef ASM_REFRESH_3_2X
void refresh_3_2x(byte *dest, byte *src, un32 *pal, int cnt)
{
	un32 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		dest[0] = dest[3] = c;
		dest[1] = dest[4] = c>>8;
		dest[2] = dest[5] = c>>16;
		dest += 6;
	}
}
#endif

#ifndef ASM_REFRESH_4_2X
void refresh_4_2x(un32 *dest, byte *src, un32 *pal, int cnt)
{
	un32 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

#ifndef ASM_REFRESH_2_3X
void refresh_2_3x(un16 *dest, byte *src, un16 *pal, int cnt)
{
	un16 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

#ifndef ASM_REFRESH_3_3X
void refresh_3_3x(byte *dest, byte *src, un32 *pal, int cnt)
{
	un32 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		dest[0] = dest[3] = dest[6] = c;
		dest[1] = dest[4] = dest[7] = c>>8;
		dest[2] = dest[5] = dest[8] = c>>16;
		dest += 9;
	}
}
#endif

#ifndef ASM_REFRESH_4_3X
void refresh_4_3x(un32 *dest, byte *src, un32 *pal, int cnt)
{
	un32 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

#ifndef ASM_REFRESH_3_4X
void refresh_3_4x(byte *dest, byte *src, un32 *pal, int cnt)
{
	un32 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		dest[0] = dest[3] = dest[6] = dest[9] = c;
		dest[1] = dest[4] = dest[7] = dest[10] = c>>8;
		dest[2] = dest[5] = dest[8] = dest[11] = c>>16;
		dest += 12;
	}
}
#endif

#ifndef ASM_REFRESH_4_4X
void refresh_4_4x(un32 *dest, byte *src, un32 *pal, int cnt)
{
	un32 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

