/* 
*   apps/avboy/include/cpuregs.h
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

#ifndef __CPUREGS_H__
#define __CPUREGS_H__

#include "cpu.h"

#define LB(r) ((r).b[LO][LO])
#define HB(r) ((r).b[LO][HI])
#define W(r) ((r).w[LO])
#define DW(r) ((r).d)

#define A HB(cpu.af)
#define F LB(cpu.af)
#define B HB(cpu.bc)
#define C LB(cpu.bc)
#define D HB(cpu.de)
#define E LB(cpu.de)
#define H HB(cpu.hl)
#define L LB(cpu.hl)

#define AF W(cpu.af)
#define BC W(cpu.bc)
#define DE W(cpu.de)
#define HL W(cpu.hl)

#define PC W(cpu.pc)
#define SP W(cpu.sp)

#define xAF DW(cpu.af)
#define xBC DW(cpu.bc)
#define xDE DW(cpu.de)
#define xHL DW(cpu.hl)

#define xPC DW(cpu.pc)
#define xSP DW(cpu.sp)

#define IMA cpu.ima
#define IME cpu.ime
#define IF R_IF
#define IE R_IE

#define FZ 0x80
#define FN 0x40
#define FH 0x20
#define FC 0x10
#define FL 0x0F /* low unused portion of flags */


#endif /* __CPUREGS_H__ */


