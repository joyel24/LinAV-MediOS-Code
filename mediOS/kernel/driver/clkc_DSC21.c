/*
*   kernel/driver/clkc_DSC21.c
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

int clkc_dividers[]={};
int clkc_dividersCount=sizeof(clkc_dividers)/sizeof(int);

void clkc_getClockParameters(int clock,int * m,int * n,int * div){
    //TODO
    *m=0;
    *n=0;
    *div=0;
}

void clkc_setClockParameters(int clock,int m,int n,int div){
    //TODO
}

int clkc_getPllNum(int clock){
    //TODO
    return -1;
}

void clkc_setPllNum(int clock,int pll){
    //TODO
}

bool clkc_getPowerState(int device){
    //TODO
    return false;
}

void clkc_setPowerState(int device,bool state){
    //TODO
}
