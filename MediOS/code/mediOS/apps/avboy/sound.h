/* 
*   apps/avboy/sound.h
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

#ifndef __SOUND_H__
#define __SOUND_H__


struct sndchan
{
	int on;
	unsigned pos;
	int cnt, encnt, swcnt;
	int len, enlen, swlen;
	int swfreq;
	int freq;
	int envol, endir;
};


struct snd
{
	int rate;
	struct sndchan ch[4];
	byte wave[16];
};


extern struct snd snd;


void sound_reset(void);
void sound_dirty(void);
byte sound_read(byte r);
void sound_write(byte r, byte b);


#endif


