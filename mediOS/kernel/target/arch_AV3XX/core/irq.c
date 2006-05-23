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
#include <kernel/mas.h>
#include <kernel/timer.h>
#include <kernel/uart.h>
#include <kernel/ata.h>
#include <kernel/ir_remote.h>


struct irq_data_s irq_table[] = {
#ifdef HAVE_MAS_SOUND
    {
        irq     : IRQ_MAS_DATA,
        action  : dsp_interrupt,
        name    : "MAS",
    },
#endif
    {
        irq     : IRQ_UART0,
        action  : uart_intAction,
        name    : "UART0 intr",
    },
    {
        irq     : IRQ_UART1,
        action  : uart_intAction,
        name    : "UART1 intr",
    },
    {
        irq     : IRQ_TMR_0,
        action  : tmr_intAction,
        name    : "Tick_timer",
    },
    {
        irq     : IRQ_TMR_1,
        action  : NULL,
        name    : "timer1",
    },
    {
        irq     : IRQ_TMR_2,
        action  : NULL,
        name    : "timer2",
    },
    {
        irq     : IRQ_TMR_3,
        action  : NULL,
        name    : "timer3",
    },
    {
        irq     : IRQ_IDE,
        action  : ide_intAction,
        name    : "IDE intr",
    },
#ifdef HAVE_DVR
    {
        irq     : IRQ_IR,
        action  : ir_remote_interrupt,
        name    : "IR remote",
    },
#endif
    {
        irq     : IRQ_OSD,
        action  : NULL,
        name    : "OSD VSync",
    },
    {
        irq     : IRQ_DSP,
        action  : NULL,
        name    : "DSP intr",
    },
    {
        irq     : -1,
        action  : NULL,
        name    : NULL,
    }
};


extern int irq_tbl_ptr;

void arch_irq_init(void)
{
    /* disable all ints */
    outw(0x0000, INTC_INT0_ENABLE);
    outw(0x0800, INTC_INT1_ENABLE);

    /* Set all to IRQ mode, not FIQ except WDT */
    outw(0x0000, INTC_FISEL0);
    outw(0x0800, INTC_FISEL1);

    /* clear all status */
    outw(0xffff, INTC_FIQ0_STATUS);
    outw(0xffff, INTC_FIQ1_STATUS);
    outw(0xffff, INTC_IRQ0_STATUS);
    outw(0xffff, INTC_IRQ1_STATUS);
 }
