/* 
*   kernel/irq.c
*
*   AMOS project
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


__IRAM_DATA int cli_var=0;
__IRAM_DATA int clf_var=0;

__IRAM_DATA struct irq_data_s irq_table[] = {
    {
        irq     : IRQ_MAS_DATA,
        action  : dsp_interrupt,
        name    : "MAS",
        nb_irq  : 0
    },
    {
        irq     : IRQ_TMR_0,
        action  : main_timer_action,
        name    : "Tick_timer",
        nb_irq  : 0
    },
    {
        irq     : IRQ_UART0,
        action  : uart_intr_action,
        name    : "UART0 intr",
        nb_irq  : 0
    },
    {
        irq     : IRQ_UART1,
        action  : uart_intr_action,
        name    : "UART1 intr",
        nb_irq  : 0
    },
    {
        irq     : -1,
        action  : NULL,
        name    : NULL,
        nb_irq  : 0
    }
};

__IRAM_CODE void sti(void)
{
    pv_sti();
}

__IRAM_CODE void cli(void)
{
    pv_cli();
}

__IRAM_CODE void stf(void)
{
    pv_stf();
}

__IRAM_CODE void clf(void)
{
    pv_clf();
}

__IRAM_CODE void __sti(void)
{
    __pv_sti();
}

__IRAM_CODE void __cli(void)
{
    __pv_cli();
}

__IRAM_CODE void __stf(void)
{
    __pv_stf();
}

__IRAM_CODE void __clf(void)
{
    __pv_clf();
}


__IRAM_CODE void do_IRQ(void)
{
    int i,irq;
    unsigned int mask;
    for(i=0;irq_table[i].irq!=-1;i++)
    {
        irq=irq_table[i].irq;
        mask=0x1 << INTC_IRQ_SHIFT(irq);
        
        if(((~inw(INTC_IRQ_STATUS(irq))) & mask) && (inw(INTC_IRQ_ENABLE(irq)) & mask))
        {
            irq_table[i].nb_irq++;
            irq_ack(irq);
            irq_table[i].action(irq); 
        }
    }
    
    
}

void init_irq(void)
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
    
    /* init the irq struct */
    
    
    printk("[init] irq\n");    
}

__IRAM_CODE void disable_irq(int irq)
{
    if(irq>=0 && irq<NR_IRQS)
        mask_irq(irq);
}

__IRAM_CODE void enable_irq(int irq)
{
    if(irq>=0 && irq<NR_IRQS)
    {
        unmask_irq(irq);
        irq_ack(irq);
    }
}

__IRAM_CODE int irq_state(int irq)
{
    if(irq>=0 && irq<NR_IRQS)
    {
        return (irq_enabled(irq)!=0);
    }
    return 0;
}


void print_irq(void)
{
    int i;
    printk("IRQ handler list:\n");
    for(i=0;irq_table[i].irq!=-1;i++)
    {
        printk("%d: irq:%d %s, %s (%d irqs)\n",
            i,irq_table[i].irq,irq_table[i].name!=NULL?irq_table[i].name:"UNDEF",
            irq_enabled(irq_table[i].irq)?"enable":"disable",
            irq_table[i].nb_irq);
    }
}
