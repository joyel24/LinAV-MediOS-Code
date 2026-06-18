/*
*   kernel/driver/clkc.c
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

// chip dependant
extern int clkc_dividers[];
extern int clkc_dividersCount;

int clkc_getClockFrequency(int clock){
    int m,n,div;

    clkc_getClockParameters(clock,&m,&n,&div);

    return CONFIG_EXT_CLK*m/n/div;
}

int clkc_setClockFrequency(int clock,int freq){
    int m=0;
    int n=0;
    int div=0;
    int delta=INT_MAX;
    int mi,ni,divi;
    int pllf,f;
    int pllnum;
    int i;

    // find best configuration
    for(divi=clkc_dividersCount-1;divi>=0;--divi){

        for(mi=CLKC_PLL_M_MAX;mi>=1;--mi){

            for(ni=CLKC_PLL_N_MAX;ni>=1;--ni){

                pllf=CONFIG_EXT_CLK*mi/ni;
                f=pllf/clkc_dividers[divi];

                if(ABS(f-freq)<ABS(delta) && pllf>=CLKC_PLL_FREQ_MIN && pllf<=CLKC_PLL_FREQ_MAX){
                    delta=f-freq;
                    m=mi;
                    n=ni;
                    div=clkc_dividers[divi];
                }
            }
        }
    }

    printk("[clkc] Wanted frequency=%d, Got frequency=%d (M=%d N=%d Divider=%d)\n",freq,freq+delta,m,n,div);

    // set all clocks that use the same pll as the clock we want to change
    //TODO: is this a good idea?
    pllnum=clkc_getPllNum(clock);
    for (i=0;i<4;++i){
        if(clkc_getPllNum(i)==pllnum) clkc_setClockParameters(i,m,n,div);
    }

    return freq+delta;
}

void clkc_init(){

}
