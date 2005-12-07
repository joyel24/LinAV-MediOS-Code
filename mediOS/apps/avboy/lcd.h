/* 
*   apps/avboy/include/lcd.h
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

#ifndef __LCD_H__
#define __LCD_H__


struct vissprite
{
	byte *buf;
	int x;
	byte pal, pri, pad[6];
};

struct scan
{
	int bg[64];
	int wnd[64];
	byte buf[256];
	byte pal1[128];
	un16 pal2[64];
	un32 pal4[64];
	byte pri[256];
	struct vissprite vs[16];
	int ns, l, x, y, s, t, u, v, wx, wy, wt, wv;
};

struct obj
{
	byte y;
	byte x;
	byte pat;
	byte flags;
};

struct lcd
{
	union
	{
		byte mem[256];
		struct obj obj[40];
	} oam;
	byte pal[128];
};

byte lcd_vbank[2][8192];

extern struct lcd lcd;
extern struct scan scan;

void lcd_reset(void);
void lcd_begin(void);
void lcd_refreshline(void);

void pal_dirty(void);
void pal_write_dmg(int i, int mapnum, byte d);
void pal_write(int i, byte b);

void vram_dirty(void);
void vram_write(int a, byte b);

#endif



