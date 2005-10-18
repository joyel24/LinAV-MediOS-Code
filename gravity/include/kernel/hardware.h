/*
 * include/hardware.h
 *
 * AMOS project
 * Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
 *
 * This file contains the hardware definitions of the AV3XX series machines
 */

#ifndef __HARDWARE_H
#define __HARDWARE_H

#define SDRAM_START   0x03000000
#define SDRAM_END     0x04000000

#define ARM_RAM_START 0x00000100
#define ARM_RAM_END   0x00008000

#define MALLOC_START  ((((unsigned int)&_end_kernel) & 0xFFFFF000)+0x1000)
#define MALLOC_SIZE   (SDRAM_END-MALLOC_START)

#define CONFIG_ARM_CLK 54000000

/* uarts */

#define UART0_BASE                        0x00030300
#define UART1_BASE                        0x00030380


/* timers */

#define TIMER0_BASE                       0x00030000
#define TIMER1_BASE                       0x00030080
#define TIMER2_BASE                       0x00030100
#define TIMER3_BASE                       0x00030180

#define TIMER_MODE                        0x00000000
#define TIMER_SEL                         0x00000002
#define TIMER_SCAL                        0x00000004
#define TIMER_DIV                         0x00000006
#define TIMER_TRG                         0x00000008
#define TIMER_CNT                         0x0000000a

#define TIMER0_MODE                       (TIMER0_BASE+TIMER_MODE)
#define TIMER1_MODE                       (TIMER0_BASE+TIMER_MODE)
#define TIMER2_MODE                       (TIMER0_BASE+TIMER_MODE)
#define TIMER3_MODE                       (TIMER0_BASE+TIMER_MODE)

#define TMR_MODE_STOP                     0x0000
#define TMR_MODE_ONESHOT                  0x0001
#define TMR_MODE_FREERUN                  0x0002
#define TMR_MODE_RES                      0x0003

#define TMR_SEL_ARM                       0x0000
#define TMR_SEL_EXT                       0x0001

/* Interrupts */

#define INTC_BASE                         0x00030500

#define INTC_FIQ0_STATUS                  (INTC_BASE + 0x0000)
#define INTC_FIQ1_STATUS                  (INTC_BASE + 0x0002)
#define INTC_IRQ0_STATUS                  (INTC_BASE + 0x0004)
#define INTC_IRQ1_STATUS                  (INTC_BASE + 0x0006)

#define INTC_FISEL0                       (INTC_BASE + 0x0020)
#define INTC_FISEL1                       (INTC_BASE + 0x0022)
#define INTC_IRQ0_ENABLE                  (INTC_BASE + 0x0024)
#define INTC_IRQ1_ENABLE                  (INTC_BASE + 0x0026)

#define INTC_INTIDR                       (INTC_BASE + 0x0040)
#define INTC_INTRAW                       (INTC_BASE + 0x0042)

#define INTC_IRQ_STATUS(val)              ((val < 16) ? INTC_IRQ0_STATUS : INTC_IRQ1_STATUS)
#define INTC_IRQ_ENABLE(val)              ((val < 16) ? INTC_IRQ0_ENABLE : INTC_IRQ1_ENABLE)
#define INTC_FIQ_STATUS(val)              ((val < 16) ? INTC_FIQ0_STATUS : INTC_FIQ1_STATUS)
#define INTC_FIQ_ENABLE(val)              ((val < 16) ? INTC_FIQ0_ENABLE : INTC_FIQ1_ENABLE)
#define INTC_SHIFT(val)                   ((val < 16) ? val : val - 16)
#define INTC_FIQ_SHIFT(val)               INTC_SHIFT(val)
#define INTC_IRQ_SHIFT(val)               INTC_SHIFT(val)

/* WatchDog */

#define WDT_BASE                          0x30400

#define WDT_ENABLE                        (WDT_BASE + 0x0000)
#define WDT_RESET                         (WDT_BASE + 0x0002)

/* ide interface */
#define IDE_BASE                          0x02400000

/* DMA          */

#define DMA_BASE                          0x30a38

/* CPLD		*/

#define CPLD_BASE                         0x02600000

/* GIO         */

#define GIO_BASE                          0x30580

/* video */

#define VIDEO_BASE                        0x30800
#define LCD_BACK_LIGHT                    0x02600200

/* osd */
#define OSD_BASE                          0x30680

/** USB state **/

#define USB_STATE                         0x30a24

/** power state **/

#define POWER_STATE                       0x30a24

/** I2C **/

/* I2C uses GIO!! */

/** BUTTONS **/

#define BUTTON_BASE                       0x02600680

#endif  /* __HARDWARE_H */
