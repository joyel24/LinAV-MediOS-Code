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
#include <kernel/ata.h>


__IRAM_DATA int cli_var=0;
__IRAM_DATA int clf_var=0;

extern struct irq_data_s irq_table[];

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
        irq = irq_table[i].irq;
        mask = 0x1 << INTC_IRQ_SHIFT(irq);

        if(((~inw(INTC_IRQ_STATUS(irq))) & mask) && (inw(INTC_IRQ_ENABLE(irq)) & mask))
        {              
            irq_table[i].nb_irq++;
            irq_ack(irq);
            irq_table[i].action(irq);
            break;
        }
    }
}

void init_irq(void)
{
    
    arch_init_irq();
    
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

/*
int irq_state(int irq)
{
    if(irq>=0 && irq<NR_IRQS)
    {
        return (irq_enabled(irq)!=0);
    }
    return 0;
}
*/

__IRAM_CODE void chg_irq_handler(int irq_num,void(*fct)(int irq))
{
    int i=0;
    int is_enable=0;
    while(irq_table[i].irq != irq_num && irq_table[i].irq != -1)
        i++;
    if(irq_table[i].irq == irq_num)
    {
        if(irq_state(irq_num))
        {
            disable_irq(irq_num);
            is_enable =1;
        }
        printk("IRQ handler for %s (%d) changed\n",irq_table[i].name,irq_num);
        irq_table[i].action = fct;
        if(is_enable)
            enable_irq(irq_num);        
    }
    else
    {
        printk("Can't change IRQ handler for %d\n",irq_num);
    }
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
