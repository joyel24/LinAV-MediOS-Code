/* 
*   apps/avboy/hw.h
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


#ifndef __HW_H__
#define __HW_H__

#define PAD_RIGHT  0x01
#define PAD_LEFT   0x02
#define PAD_UP     0x04
#define PAD_DOWN   0x08
#define PAD_A      0x10
#define PAD_B      0x20
#define PAD_SELECT 0x40
#define PAD_START  0x80

#define IF_VBLANK 0x01
#define IF_STAT   0x02
#define IF_TIMER  0x04
#define IF_SERIAL 0x08
#define IF_PAD    0x10

struct hw
{
	byte ilines;
	byte pad;
	int cgb, gba;
	int hdma;
};


extern struct hw hw;

void hw_interrupt(byte i, byte mask);
void hw_reset(void);
void hw_hdma_cmd(byte c);
void hw_dma(byte b);
void hw_hdma(void);

void pad_release(byte k);
void pad_press(byte k);
void pad_refresh(void);

#endif


