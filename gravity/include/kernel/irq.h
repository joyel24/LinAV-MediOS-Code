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

#define MAX_IRQ_CNT  10

extern int cli_var;
extern int clf_var;

/*
 * Enable IRQs
 */
/*********************************** std enable IRQ */
#define sti()                                  \
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
#define __sti()                                \
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
#define cli()                                  \
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
#define __cli()                                \
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
#define stf()                                  \
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
#define __stf()                                \
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
#define clf()                                  \
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
#define __clf()                                \
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
        
struct irq_data_s {
    int enable;
    void (*action)(void);
    char * name;
    unsigned int nb_irq;
};

extern void init_irq(void);

extern void add_irq_handler(int irq,void(*action)(void),char * name);
extern void del_irq_handler(int irq);
extern void disable_irq(int irq);
extern void enable_irq(int irq);
extern int  irq_state(int irq);

extern void print_irq(void);

extern void mask_irq(unsigned int irq);
extern void unmask_irq(unsigned int irq);
extern void mask_ack_irq(unsigned int irq);

#endif
