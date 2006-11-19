/*
*   kernel/core/kdelay.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/delay.h>
#include <kernel/timer.h>
#include <kernel/irq.h>

void mdelay(unsigned long msecs)
{
#if 1
    if(irq_globalEnabled()){
        unsigned int end=tmr_getMicroTick()+msecs*100;

        while (tmr_getMicroTick()<end) /*nothing*/;
    }else{  // irqs are disabled, so no tick timer, use udelay() as a fallback
        while (msecs--) udelay(1000);
    }
#else
while (msecs--) udelay(1000);
#endif
}
