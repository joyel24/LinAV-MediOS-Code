/* 
*   apps/avboy/cpu.h
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

#ifndef __CPU_H__
#define __CPU_H__

union reg
{
	byte b[2][2];
	word w[2];
	un32 d; /* padding for alignment, carry */
};

struct cpu
{
	union reg pc, sp, bc, de, hl, af;
	int ime, ima;
	int speed;
	int halt;
	int div, tim;
	int lcdc;
	int snd;
};

extern struct cpu cpu;

void cb_switch(byte cbop);
void cpu_reset(void);
int cpu_emulate(int cycles);

#endif


