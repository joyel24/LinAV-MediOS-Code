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

__IRAM_DATA struct irq_data_s irq_data[NR_IRQS];
__IRAM_DATA int cli_var=0;
__IRAM_DATA int clf_var=0;

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


__IRAM_CODE void do_IRQ(int irq, struct pt_regs *regs)
{
    struct irq_data_s * desc;
    mask_ack_irq(irq); 
    irq_data[irq].nb_irq++;    
    desc=&irq_data[irq];
    if(desc->enable==1 && desc->action!=NULL)
    {
        desc->action(irq);
        unmask_irq(irq);
    }
}

/* Acknowlede the IRQ. */
#if 0
static inline void irq_ack(unsigned int irq, struct pt_regs * regs)
{
    outw((1<<INTC_IRQ_SHIFT(irq)), INTC_IRQ_STATUS(irq));
}

/* Acknowledge the FIQ. */

static inline void fiq_ack(unsigned int irq)
{
    outw((1<<INTC_FIQ_SHIFT(irq)), INTC_FIQ_STATUS(irq));
}

/* Mask the IRQ. */

static inline void mask_irq(unsigned int irq)
{
    unsigned int eint;
    int mask;

    eint = INTC_IRQ_ENABLE(irq);
    mask = inw(eint);
    mask &= ~(1<<INTC_IRQ_SHIFT(irq));
    outw(mask, eint);
}

/* Unmask the IRQ. */

static inline void unmask_irq(unsigned int irq)
{
    unsigned int eint;
    int mask;

    eint = INTC_IRQ_ENABLE(irq);
    mask = inw(eint);
    mask |= (1<<INTC_IRQ_SHIFT(irq));
    outw(mask, eint);
}

/* Mask the IRQ and acknowledge it. */

static inline void mask_ack_irq(unsigned int irq)
{
    unsigned int eint;
    int mask;

    eint = INTC_IRQ_ENABLE(irq);
    mask = inw(eint);
    mask &= ~(1<<INTC_IRQ_SHIFT(irq));
    outw(mask, eint);

    outw((1<<INTC_IRQ_SHIFT(irq)), INTC_IRQ_STATUS(irq));
}
#endif

void init_irq(void)
{
    int irq;
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
    for(irq=0;irq<NR_IRQS;irq++)
    {
        irq_data[irq].enable=0;
        irq_data[irq].action=NULL;
        irq_data[irq].name=NULL;
        irq_data[irq].nb_irq=0;
        
    }
    
    printk("[init] irq\n");    
}

__IRAM_CODE void add_irq_handler(int irq,void(*action)(int irqnr),char * name)
{
    if(irq>=0 && irq<NR_IRQS && action != NULL)
    {
        irq_data[irq].action=action;
        irq_data[irq].name=name;
    }
}

__IRAM_CODE void del_irq_handler(int irq)
{
    if(irq>=0 && irq<NR_IRQS)
    {
        irq_data[irq].action=NULL;
        irq_data[irq].name=NULL;
        irq_data[irq].enable=0;
    }
}

__IRAM_CODE void disable_irq(int irq)
{
    if(irq>=0 && irq<NR_IRQS)
    {
        irq_data[irq].enable=0;
        mask_irq(irq);
    }
}

__IRAM_CODE void enable_irq(int irq)
{
    if(irq>=0 && irq<NR_IRQS)
    {
        irq_data[irq].enable=1;
        unmask_irq(irq);
    }
}

__IRAM_CODE int irq_state(int irq)
{
    if(irq>=0 && irq<NR_IRQS)
        return irq_data[irq].enable;
    else
        return 0;
}


void print_irq(void)
{
    int irq,irqnr=0;
    printk("IRQ handler list:\n");
    for(irq=0;irq<NR_IRQS;irq++)
    {
        if(irq_data[irq].action!=NULL || irq_data[irq].nb_irq !=0)
        {            
            printk("%d: irq:%d %s, %s (%d irqs)\n",
                irqnr,irq,irq_data[irq].name!=NULL?irq_data[irq].name:"UNDEF",
                irq_data[irq].enable==1?"enable":"disable",
                irq_data[irq].nb_irq);
            irqnr++;
        }
    }
}
