/* 
*   include/irq.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/


#ifndef __IRQ_H
#define __IRQ_H

#include <kernel/irqs.h>
#include <kernel/hardware.h>

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
#define     mask_irq(IRQ)      ({ unsigned int __addr=INTC_IRQ_ENABLE(IRQ); outw(inw(__addr)&(~(1<<INTC_IRQ_SHIFT(IRQ))),__addr); })
#define     unmask_irq(IRQ)    ({ unsigned int __addr=INTC_IRQ_ENABLE(IRQ); outw(inw(__addr)|(1<<INTC_IRQ_SHIFT(IRQ)),__addr); })
#define     mask_ack_irq(IRQ)  ({ mask_irq(IRQ); irq_ack(IRQ); })
#define     irq_enabled(IRQ)   ({ int __val; __val=(inw(INTC_IRQ_ENABLE(IRQ)) & (1<<INTC_IRQ_SHIFT(IRQ)))!=0; __val;})

#define     irq_state(IRQ)     ((IRQ>=0 && IRQ<NR_IRQS)?irq_enabled(IRQ)!=0:0)

struct irq_data_s {
    int irq;
    void (*action)(int irqnr);
    char * name;
    unsigned int nb_irq;
};

extern void init_irq(void);

extern void add_irq_handler(int irq,void(*action)(int irqnr),char * name);
extern void del_irq_handler(int irq);
extern void disable_irq(int irq);
extern void enable_irq(int irq);
//extern int  irq_state(int irq);
extern void chg_irq_handler(int irq_num,void(*fct)(int irq));
extern void print_irq(void);

#endif
