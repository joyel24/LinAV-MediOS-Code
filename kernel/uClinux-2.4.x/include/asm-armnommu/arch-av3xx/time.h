/*
 * linux/include/asm-arm/arch-av3xx/time.h
 * based on dsc21
 *
 * Copyright (C) 1998 Deborah Wallach.
 * Twiddles  (C) 1999 	Hugo Fiennes <hugo@empeg.com>
 * 
 * 2000/03/29 (C) Nicolas Pitre <nico@cam.org>
 *	Rewritten: big cleanup, much simpler, better HZ acuracy.
 *
 * 2001/02/20 (C) RidgeRun, Inc (http://www.ridgerun.com)
 *
 */
#ifndef __ASM_ARCH_TIME_H__
#define __ASM_ARCH_TIME_H__

#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/hardware.h>

extern struct irqaction timer_irq;

extern unsigned long av3xx_gettimeoffset(void);
extern void av3xx_timer_interrupt(int irq, void *dev_id, struct pt_regs *regs);

void __inline__ setup_timer (void)
{
	u_int16_t reg;

 /* disable */
	outw(AV3XX_TMR_MODE_STOP, AV3XX_TIMER0_BASE+AV3XX_TIMER_MODE);
	outw(AV3XX_TMR_MODE_STOP, AV3XX_TIMER1_BASE+AV3XX_TIMER_MODE);
	outw(AV3XX_TMR_MODE_STOP, AV3XX_TIMER2_BASE+AV3XX_TIMER_MODE);
	outw(AV3XX_TMR_MODE_STOP, AV3XX_TIMER3_BASE+AV3XX_TIMER_MODE);

	/*
	 * System clock formula:
	 *         freq = clock / (div * scale)
	 *  freq need to be 100Hz
	 */

	outw(AV3XX_TMR_SEL_ARM, AV3XX_TIMER0_BASE+AV3XX_TIMER_SEL);

	/* prescale  */
	outw(9, AV3XX_TIMER0_BASE+AV3XX_TIMER_SCAL);

	/* div  */
	outw(CONFIG_ARM_CLK/1000, AV3XX_TIMER0_BASE+AV3XX_TIMER_DIV);

	/* freerun */
	outw(AV3XX_TMR_MODE_FREERUN, AV3XX_TIMER0_BASE+AV3XX_TIMER_MODE);

	gettimeoffset = av3xx_gettimeoffset;
	timer_irq.handler = av3xx_timer_interrupt;
	setup_arm_irq(IRQ_TIMER, &timer_irq);

}


#endif /* __ASM_ARCH_TIME_H__ */
