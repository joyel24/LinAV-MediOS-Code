/*
*   kernel/target/arch_G4XX/timer.c
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

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/kernel.h>

void arch_init_timer(void)
{
    /*
    * System clock formula:
    *         freq = clock / (div * scale)
    *  freq need to be 100Hz
    */

    SET_TIMER_SEL(TMR_SEL_EXT,TMR0);

    /* prescale  */
    
    SET_TIMER_SCAL(9, TMR0);

    /* div  */
    SET_TIMER_DIV(((CONFIG_EXT_CLK/1000)-1), TMR0);

    /* freerun */
    SET_TIMER_MODE(TMR_MODE_FREERUN, TMR0);

    /* enable timer irq */
    enable_irq(IRQ_TMR_0);

    /*
    * Timer 3 is used for the watchdog
    * Sends FIQ at 2khz
    */

    SET_TIMER_SEL(TMR_SEL_ARM,TMR3);

    /* prescale  */
    SET_TIMER_SCAL(0, TMR3);

    /* div  */
    SET_TIMER_SEL(1,TMR3);
    SET_TIMER_DIV(50624, TMR3);

    /* freerun */
    SET_TIMER_MODE(TMR_MODE_FREERUN, TMR3);

}

