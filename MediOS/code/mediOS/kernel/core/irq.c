/*
*   kernel/core/irq.c
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


__IRAM_DATA int cli_var=0;
__IRAM_DATA int clf_var=0;

extern struct irq_data_s irq_table[];

void sti(void)
{
    pv_sti();
}

void cli(void)
{
    pv_cli();
}

void stf(void)
{
    pv_stf();
}

void clf(void)
{
    pv_clf();
}

void __sti(void)
{
    __pv_sti();
}

void __cli(void)
{
    __pv_cli();
}

void __stf(void)
{
    __pv_stf();
}

void __clf(void)
{
    __pv_clf();
}

extern int irq_tbl_ptr;

void irq_init(void)
{
    int i = 0;
    arch_irq_init();
    
    while(irq_table[i].irq!=-1)
    {
        irq_setHandler(irq_table[i].irq,irq_table[i].action);
        i++;
    }
    
#if defined(DSC25) || defined(DM270) || defined(DM320)
    irq_setRaw(0x0);
    fiq_setRaw(0x0);        
    int_setEabase(irq_tbl_ptr,0x2);
#elif defined(DSC21)
    dsc21_setRaw(0,0);
#else
#warning UKN CHIP
    printk("[init] irq FATAL ERROR: ukn chip\n");
#endif
    printk("[init] irq\n");
}

void irq_disable(int irq)
{
    if(irq>=0 && irq<NR_IRQS)
        irq_mask(irq);
}

void irq_enable(int irq)
{
    if(irq>=0 && irq<NR_IRQS)
    {
        irq_unmask(irq);
        irq_ack(irq);
    }
}

void irq_setHandler(int irq_num,void(*fct)(int,struct pt_regs *))
{
    struct irq_bloc_s * ptr=(struct irq_bloc_s *)irq_tbl_ptr;
    ptr[irq_num+1].irq=irq_num;
    ptr[irq_num+1].mask=1<<INTC_IRQ_SHIFT(irq_num);
    ptr[irq_num+1].reg=INTC_IRQ_STATUS(irq_num);
    ptr[irq_num+1].fct=(int)fct;
}

void irq_changeHandler(int irq_num,void(*fct)(int irq,struct pt_regs * regs))
{
    int i=0;
    int is_enable=0;
    while(irq_table[i].irq != irq_num && irq_table[i].irq != -1)
        i++;
    if(irq_table[i].irq == irq_num)
    {
        if(irq_state(irq_num))
        {
            irq_disable(irq_num);
            is_enable = 1;
        }
        printk("IRQ handler for %s (%d) changed\n",irq_table[i].name,irq_num);
        irq_table[i].action = fct;
        irq_setHandler(irq_num,fct);
        if(is_enable)
            irq_enable(irq_num);
    }
    else
    {
        printk("Can't change IRQ handler for %d\n",irq_num);
    }
}

void irq_print(void)
{
    int i;
    printk("IRQ handler list:\n");
    for(i=0;irq_table[i].irq!=-1;i++)
    {
        printk("%d: irq:%d %s, %s\n",
            i,irq_table[i].irq,irq_table[i].name!=NULL?irq_table[i].name:"UNDEF",
            irq_enabled(irq_table[i].irq)?"enable":"disable");
    }
}

int irq_globalEnabled(){
  int val;

  asm volatile(
      "mrs %0,cpsr \n"
      "and %0,%0,#0x80 \n"
      "mov %0,%0,lsr #7 \n"
      "eor %0,%0,#1 \n"
  :"=r" (val));

  return val;
}
