/* 
*   kernel/target/arch_AV3XX/timer.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>
#include <kernel/irq.h>
#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/timer.h>
#include <kernel/kernel.h>

void arch_init_timer(void)
{
    /*
    * System clock formula:
    *         freq = clock / (div * scale)
    *  freq need to be 100Hz
    */

    outw(TMR_SEL_ARM, TIMER0_BASE+TIMER_SEL);

    /* prescale  */
    outw(9, TIMER0_BASE+TIMER_SCAL);

    /* div  */
    outw((CONFIG_ARM_CLK/1000)-1, TIMER0_BASE+TIMER_DIV);

    /* freerun */
    outw(TMR_MODE_FREERUN, TIMER0_BASE+TIMER_MODE);
       
    /* enable timer irq */
    enable_irq(IRQ_TMR_0);
}

