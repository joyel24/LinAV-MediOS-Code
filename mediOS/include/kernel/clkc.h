/*
*   include/kernel/clkc.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __CLKC_H
#define __CLKC_H

#include <sys_def/stddef.h>
#include <sys_def/types.h>

#if defined(DM270) || defined(DM320)
    #include <kernel/clkc_DM270.h>
#elif defined(DSC25)
    #include <kernel/clkc_DSC25.h>
#elif defined(DSC21)
    #include <kernel/clkc_DSC21.h>
#else
    #error NO CHIP defined
#endif

#define CLK_ARM                        0
#define CLK_DSP                        1
#define CLK_SDRAM                      2
#define CLK_ACCEL                      3

void clkc_getClockParameters(int clock,int * m,int * n,int * div);
void clkc_setClockParameters(int clock,int m,int n,int div);

int clkc_getPllNum(int clock);
void clkc_setPllNum(int clock,int pll);

bool clkc_getPowerState(int device);
void clkc_setPowerState(int device,bool state);

int clkc_getClockFrequency(int clock);
int clkc_setClockFrequency(int clock,int freq);

void clkc_init();

#endif
