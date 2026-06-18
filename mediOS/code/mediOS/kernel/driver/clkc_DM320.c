/*
*   kernel/driver/clkc_DM320.c
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

int clkc_dividers[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
int clkc_dividersCount=sizeof(clkc_dividers)/sizeof(int);

int clkc_pllAddress[]={CLKC_PLL0,CLKC_PLL1};
int clkc_pllSel[]={CLKC_SEL_ARM_PLL1,CLKC_SEL_DSP_PLL1,CLKC_SEL_SDRAM_PLL1,CLKC_SEL_ACCEL_PLL1};

int clkc_divAddress[]={CLKC_DIV_ARM_ADDRESS,CLKC_DIV_DSP_ADDRESS,CLKC_DIV_SDRAM_ADDRESS,CLKC_DIV_ACCEL_ADDRESS};
int clkc_divMask[]={CLKC_DIV_ARM_MASK,CLKC_DIV_DSP_MASK,CLKC_DIV_SDRAM_MASK,CLKC_DIV_ACCEL_MASK};
int clkc_divShift[]={CLKC_DIV_ARM_SHIFT,CLKC_DIV_DSP_SHIFT,CLKC_DIV_SDRAM_SHIFT,CLKC_DIV_ACCEL_SHIFT};

void clkc_getClockParameters(int clock,int * m,int * n,int * div){
    int addr;

    // divider
    *div=((inw(clkc_divAddress[clock])&clkc_divMask[clock])>>clkc_divShift[clock])+1;

    // M & N
    addr=clkc_pllAddress[clkc_getPllNum(clock)];

    *m=((inw(addr)&CLKC_PLL_M_MASK)>>CLKC_PLL_M_SHIFT)+1;
    *n=((inw(addr)&CLKC_PLL_N_MASK)>>CLKC_PLL_N_SHIFT)+1;
}

void clkc_setClockParameters(int clock,int m,int n,int div){
    int addr;

    // divider
    outw((inw(clkc_divAddress[clock])&~clkc_divMask[clock]) | (((div-1)<<clkc_divShift[clock])&clkc_divMask[clock]),clkc_divAddress[clock]);

    // M & N
    addr=clkc_pllAddress[clkc_getPllNum(clock)];

    outw((((m-1)<<CLKC_PLL_M_SHIFT)&CLKC_PLL_M_MASK) | (((n-1)<<CLKC_PLL_N_SHIFT)&CLKC_PLL_N_MASK),addr);
}

int clkc_getPllNum(int clock){
    return (inw(CLKC_PLL_SELECT)&clkc_pllSel[clock])?1:0;
}

void clkc_setPllNum(int clock,int pll){
    if(pll){
        outw(inw(CLKC_PLL_SELECT)|clkc_pllSel[clock],CLKC_PLL_SELECT);
    }else{
        outw(inw(CLKC_PLL_SELECT)&~clkc_pllSel[clock],CLKC_PLL_SELECT);
    }
}

bool clkc_getPowerState(int device){
    //TODO
    return false;
}

void clkc_setPowerState(int device,bool state){
    //TODO
}
