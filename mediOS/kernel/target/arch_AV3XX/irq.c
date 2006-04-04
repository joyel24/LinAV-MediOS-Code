/*
*   kernel/target/arch_AV3XX/irq.c
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
#include <kernel/kernel.h>

/* irq driven driver includes */
#include <kernel/sound.h>
#include <kernel/timer.h>
#include <kernel/uart.h>
#include <kernel/ata.h>
#include <kernel/ir_remote.h>

__IRAM_DATA struct irq_data_s irq_table[] = {
    {
        irq     : IRQ_UART0,
        action  : uart_intAction,
        name    : "UART0 intr",
        nb_irq  : 0
    },
    {
        irq     : IRQ_UART1,
        action  : uart_intAction,
        name    : "UART1 intr",
        nb_irq  : 0
    },
#ifdef HAVE_MAS_SOUND
    {
        irq     : IRQ_MAS_DATA,
        action  : dsp_interrupt,
        name    : "MAS",
        nb_irq  : 0
    },
#endif
    {
        irq     : IRQ_TMR_0,
        action  : tmr_intAction,
        name    : "Tick_timer",
        nb_irq  : 0
    },
    {
        irq     : IRQ_TMR_1,
        action  : NULL,
        name    : "timer1",
        nb_irq  : 0
    },
    {
        irq     : IRQ_TMR_2,
        action  : NULL,
        name    : "timer2",
        nb_irq  : 0
    },
    {
        irq     : IRQ_TMR_3,
        action  : NULL,
        name    : "timer3",
        nb_irq  : 0
    },
    {
        irq     : IRQ_IDE,
        action  : ide_intAction,
        name    : "IDE intr",
        nb_irq  : 0
    },
#ifdef HAVE_DVR
    {
        irq     : IRQ_IR,
        action  : ir_remote_interrupt,
        name    : "IR remote",
        nb_irq  : 0
    },
#endif
    {
        irq     : -1,
        action  : NULL,
        name    : NULL,
        nb_irq  : 0
    }
};



void arch_irq_init(void)
{
    /* disable all irqs */
    outw(0x0000, INTC_IRQ0_ENABLE);
    outw(0x0800, INTC_IRQ1_ENABLE);

    /* Set all to IRQ mode, not FIQ except WDT */
    outw(0x0000, INTC_FISEL0);
    outw(0x0800, INTC_FISEL1);

    /* clear all status */
    outw(0xffff, INTC_FIQ0_STATUS);
    outw(0xffff, INTC_FIQ1_STATUS);
    outw(0xffff, INTC_IRQ0_STATUS);
    outw(0xffff, INTC_IRQ1_STATUS);
}
