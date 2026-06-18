/*
*   kernel/driver/clkc_DSC25.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/clkc.h>

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/kernel.h>
#include <kernel/delay.h>

int clkc_dividers[]={1,2,4,8};
int clkc_dividersCount=sizeof(clkc_dividers)/sizeof(int);

int clkc_pllAddress[]={CLKC_PLL0,CLKC_PLL1,CLKC_PLL2};
int clkc_pllNum[]={0,1,2,1};

int clkc_divMask[]={CLKC_DIV_ARM_MASK,CLKC_DIV_DSP_MASK,CLKC_DIV_SDRAM_MASK,CLKC_DIV_DSP_MASK};
int clkc_divShift[]={CLKC_DIV_ARM_SHIFT,CLKC_DIV_DSP_SHIFT,CLKC_DIV_SDRAM_SHIFT,CLKC_DIV_DSP_SHIFT};

int clkc_realN[]={4,1,2,3};
int clkc_rawN[]={-1,1,2,3,0};
int clkc_realDiv[]={1,2,4,8};
int clkc_rawDiv[]={-1,0,1,-1,2,-1,-1,-1,3};

void clkc_getClockParameters(int clock,int * m,int * n,int * div){
    int addr;

    // divider
    *div=clkc_realDiv[((inw(CLKC_PLL_DIVIDER)&clkc_divMask[clock])>>clkc_divShift[clock])];

    // M & N
    addr=clkc_pllAddress[clkc_getPllNum(clock)];

    *m=((inw(addr)&CLKC_PLL_M_MASK)>>CLKC_PLL_M_SHIFT)+1;
    *n=clkc_realN[((inw(addr)&CLKC_PLL_N_MASK)>>CLKC_PLL_N_SHIFT)];
}

void clkc_setClockParameters(int clock,int m,int n,int div){
    int addr;

    // divider
    outw((inw(CLKC_PLL_DIVIDER)&~clkc_divMask[clock]) | ((clkc_rawDiv[div]<<clkc_divShift[clock])&clkc_divMask[clock]),CLKC_PLL_DIVIDER);

    // M & N
    addr=clkc_pllAddress[clkc_getPllNum(clock)];

    outw((((m-1)<<CLKC_PLL_M_SHIFT)&CLKC_PLL_M_MASK) | ((clkc_rawN[n]<<CLKC_PLL_N_SHIFT)&CLKC_PLL_N_MASK),addr);
}

int clkc_getPllNum(int clock){
    return clkc_pllNum[clock];
}

void clkc_setPllNum(int clock,int pll){
    // Can't be changed on DSC25
}

bool clkc_getPowerState(int device){
    //TODO
    return false;
}

void clkc_setPowerState(int device,bool state){
    //TODO
}
