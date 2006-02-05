/* 
*   include/kernel/irq.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __IRQ_H
#define __IRQ_H

#include <kernel/irqs.h>
#include <kernel/hardware.h>
#include <sys_def/types.h>

#define MAX_IRQ_CNT  10

extern int cli_var;
extern int clf_var;

void sti(void);
void cli(void);
void stf(void);
void clf(void);

void __sti(void);
void __cli(void);
void __stf(void);
void __clf(void);

/*
 * Enable IRQs
 */
/*********************************** std enable IRQ */
#define pv_sti()                                  \
    ({                                         \
        unsigned long temp;                    \
        __asm__ __volatile__(                  \
            "cmp %0, #0 \n"                    \
            "beq 1f \n"                        \
            "sub %0, %0, #1 \n"                \
            "cmp %0, #0 \n"                    \
            "bne 1f \n"                        \
            "mrs %1, cpsr \n"                  \
            "bic %1, %1, #0x80 \n"             \
            "msr cpsr_c, %1 \n"                \
            "1:"                               \
        : "=r" (cli_var), "=r" (temp)          \
        : "0" (cli_var)                        \
        : "memory");                           \
    })
/*********************************** force enable IRQ */        
#define __pv_sti()                                \
    ({                                         \
        unsigned long temp;                    \
        __asm__ __volatile__(                  \
            "mrs %1, cpsr \n"                  \
            "bic %1, %1, #0x80 \n"             \
            "msr cpsr_c, %1 \n"                \
            "mov %0, #0"                       \
        : "=r" (cli_var), "=r" (temp)          \
        : "0" (cli_var)                        \
        : "memory");                           \
    })

/*
 * Disable IRQs
 */
 /*********************************** std disable IRQ */
#define pv_cli()                                  \
    ({                                         \
        unsigned long temp;                    \
        __asm__ __volatile__(                  \
            "cmp %0, #0 \n"                    \
            "bne 2f \n"                        \
            "mrs %1, cpsr \n"                  \
            "orr %1, %1, #0x80 \n"             \
            "msr cpsr_c, %1 \n"                \
            "2: \n"                            \
            "add %0, %0, #1"                   \
        : "=r" (cli_var), "=r" (temp)          \
        : "0" (cli_var)                        \
        : "memory");                           \
    })
/*********************************** force disable IRQ */
#define __pv_cli()                                \
    ({                                         \
        unsigned long temp;                    \
        __asm__ __volatile__(                  \
            "mrs %1, cpsr \n"                  \
            "orr %1, %1, #0x80 \n"             \
            "msr cpsr_c, %1 \n"                \
            "mov %0, #1"                       \
        : "=r" (cli_var), "=r" (temp)          \
        : "0" (cli_var)                        \
        : "memory");                           \
    })

/*
 * Enable FIQs
 */
 /*********************************** std enable FIQ */
#define pv_stf()                                  \
    ({                                         \
        unsigned long temp;                    \
        __asm__ __volatile__(                  \
            "cmp %0, #0 \n"                    \
            "beq 3f \n"                        \
            "sub %0, %0, #1 \n"                \
            "cmp %0, #0 \n"                    \
            "bne 3f \n"                        \
            "mrs %1, cpsr \n"                  \
            "bic %1, %1, #0x40 \n"             \
            "msr cpsr_c, %1 \n"                \
            "3:"                               \
        : "=r" (clf_var), "=r" (temp)          \
        : "0" (clf_var)                        \
        : "memory");                           \
    })
/*********************************** force enable FIQ */ 
#define __pv_stf()                                \
    ({                                         \
        unsigned long temp;                    \
        __asm__ __volatile__(                  \
            "mrs %1, cpsr \n"                  \
            "bic %1, %1, #0x40 \n"             \
            "msr cpsr_c, %1 \n"                \
            "mov %0, #0"                       \
        : "=r" (clf_var), "=r" (temp)          \
        : "0" (clf_var)                        \
        : "memory");                           \
    })

/*
 * Disable FIQs
 */
/*********************************** std disable IRQ */
#define pv_clf()                                  \
    ({                                         \
        unsigned long temp;                    \
        __asm__ __volatile__(                  \
            "cmp %0, #0 \n"                    \
            "bne 4f \n"                        \
            "mrs %1, cpsr \n"                  \
            "orr %1, %1, #0x40 \n"             \
            "msr cpsr_c, %1 \n"                \
            "4:"                               \
            "add %0, %0, #1 \n"                \
        : "=r" (clf_var), "=r" (temp)          \
        : "0" (clf_var)                        \
        : "memory");                           \
    })
/*********************************** force disable IRQ */
#define __pv_clf()                                \
    ({                                         \
        unsigned long temp;                    \
        __asm__ __volatile__(                  \
            "mrs %1, cpsr \n"                  \
            "orr %1, %1, #0x40 \n"             \
            "msr cpsr_c, %0 \n"                \
            "mov %0, #1"                       \
        : "=r" (clf_var), "=r" (temp)          \
        : "0" (clf_var)                        \
        : "memory");                           \
    })


#define     irq_ack(IRQ)       ({ outw((1<<INTC_IRQ_SHIFT(IRQ)), INTC_IRQ_STATUS(IRQ)); })
#define     fiq_ack(FIQ)       ({ outw((1<<INTC_FIQ_SHIFT(FIQ)), INTC_FIQ_STATUS(FIQ)); })
#define     irq_mask(IRQ)      ({ unsigned int __addr=INTC_IRQ_ENABLE(IRQ); outw(inw(__addr)&(~(1<<INTC_IRQ_SHIFT(IRQ))),__addr); })
#define     irq_unmask(IRQ)    ({ unsigned int __addr=INTC_IRQ_ENABLE(IRQ); outw(inw(__addr)|(1<<INTC_IRQ_SHIFT(IRQ)),__addr); })
#define     irq_maskAndAck(IRQ)  ({ mask_irq(IRQ); irq_ack(IRQ); })
#define     irq_enabled(IRQ)   ({ int __val; __val=(inw(INTC_IRQ_ENABLE(IRQ)) & (1<<INTC_IRQ_SHIFT(IRQ)))!=0; __val;})

#define     irq_state(IRQ)     ((IRQ>=0 && IRQ<NR_IRQS)?irq_enabled(IRQ)!=0:0)

struct irq_data_s {
    int irq;
    void (*action)(int irqnr,struct pt_regs * regs);
    char * name;
    unsigned int nb_irq;
};

extern void irq_init(void);

extern void irq_addHandler(int irq,void(*action)(int irqnr,struct pt_regs * regs),char * name);
extern void irq_deleteHandler(int irq);
extern void irq_changeHandler(int irq_num,void(*fct)(int irq,struct pt_regs * regs));
extern void irq_disable(int irq);
extern void irq_enable(int irq);
extern void irq_print(void);
void arch_irq_init(void);


#endif
