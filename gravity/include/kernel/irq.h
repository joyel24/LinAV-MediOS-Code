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

/*
 * Enable IRQs
 */
#define sti()							\
	({							\
		unsigned long temp;				\
	__asm__ __volatile__(					\
	"mrs	%0, cpsr		@ sti\n"		\
"	bic	%0, %0, #128\n"					\
"	msr	cpsr_c, %0"					\
	: "=r" (temp)						\
	:							\
	: "memory");						\
	})

/*
 * Disable IRQs
 */
#define cli()							\
	({							\
		unsigned long temp;				\
	__asm__ __volatile__(					\
	"mrs	%0, cpsr		@ cli\n"		\
"	orr	%0, %0, #128\n"					\
"	msr	cpsr_c, %0"					\
	: "=r" (temp)						\
	:							\
	: "memory");						\
	})

/*
 * Enable FIQs
 */
#define stf()							\
	({							\
		unsigned long temp;				\
	__asm__ __volatile__(					\
	"mrs	%0, cpsr		@ stf\n"		\
"	bic	%0, %0, #64\n"					\
"	msr	cpsr_c, %0"					\
	: "=r" (temp)						\
	:							\
	: "memory");						\
	})

/*
 * Disable FIQs
 */
#define clf()							\
	({							\
		unsigned long temp;				\
	__asm__ __volatile__(					\
	"mrs	%0, cpsr		@ clf\n"		\
"	orr	%0, %0, #64\n"					\
"	msr	cpsr_c, %0"					\
	: "=r" (temp)						\
	:							\
	: "memory");						\
	})

struct irq_data_s {
    int enable;
    void (*action)(void);
    char * name;
    unsigned int lck_jif;
    unsigned int lck_pc;
    unsigned int lck_cnt;
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
