/*
 * include/kernel/target/gmini4xx_def.h
 *
 * mediOS project
 * Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
 *
 * This file contains the hardware definitions of the AV3XX series machines
 */

#ifndef __GMINI4XX_DEF_H
#define __GMINI4XX_DEF_H


#define SDRAM_START       0x0900000
#define SDRAM_END         0x1900000

#define MALLOC_START  ((((unsigned int)&_end_kernel) & 0xFFFFF000)+0x1000)
#define MALLOC_SIZE   (SDRAM_END-MALLOC_START)

#define CONFIG_ARM_CLK 54000000
#define CONFIG_EXT_CLK 27000000


/* uarts */

#define UART0_BASE                        0x00030300
#define UART1_BASE                        0x00030380

/* DEBUG */
#define DEBUG_UART                        UART_0
#define DEBUG_UART_INIT

#define USE_DEBUG_ON_SCREEN     1

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
#define INTC_FIQ2_STATUS                  (INTC_BASE + 0x0004)
#define INTC_IRQ0_STATUS                  (INTC_BASE + 0x0008)
#define INTC_IRQ1_STATUS                  (INTC_BASE + 0x000a)
#define INTC_IRQ2_STATUS                  (INTC_BASE + 0x000c)

#define INTC_FISEL0                       (INTC_BASE + 0x0020)
#define INTC_FISEL1                       (INTC_BASE + 0x0022)
#define INTC_FISEL2                       (INTC_BASE + 0x0024)
#define INTC_IRQ0_ENABLE                  (INTC_BASE + 0x0028)
#define INTC_IRQ1_ENABLE                  (INTC_BASE + 0x002a)
#define INTC_IRQ2_ENABLE                  (INTC_BASE + 0x002c)

#define INTC_INTIDR                       (INTC_BASE + 0x00FF) // does not seem to exist
#define INTC_INTRAW                       (INTC_BASE + 0x0030)

#define INTC_IRQ_STATUS(val)              ((val < 16) ? INTC_IRQ0_STATUS : ((val<32) ? INTC_IRQ1_STATUS : INTC_IRQ2_STATUS))
#define INTC_IRQ_ENABLE(val)              ((val < 16) ? INTC_IRQ0_ENABLE : ((val<32) ? INTC_IRQ1_ENABLE : INTC_IRQ2_ENABLE))
#define INTC_FIQ_STATUS(val)              ((val < 16) ? INTC_FIQ0_STATUS : ((val<32) ? INTC_FIQ1_STATUS : INTC_FIQ2_STATUS))
#define INTC_FIQ_ENABLE(val)              ((val < 16) ? INTC_FIQ0_ENABLE : ((val<32) ? INTC_FIQ1_ENABLE : INTC_FIQ2_ENABLE))
#define INTC_SHIFT(val)                   (val & 0x0f)
#define INTC_FIQ_SHIFT(val)               INTC_SHIFT(val)
#define INTC_IRQ_SHIFT(val)               INTC_SHIFT(val)


/* WatchDog */

#define WDT_BASE                          0x30400

#define WDT_MODE                          (WDT_BASE + 0x0000)
#define WDT_RESET                         (WDT_BASE + 0x0002)
#define WDT_SCAL                          (WDT_BASE + 0x0004)
#define WDT_DIV                           (WDT_BASE + 0x0006)
//#define WDT_EXT_RESET                     (WDT_BASE + 0x0008)

#define WDT_MODE_ARM_RESET                0x0004
#define WDT_MODE_ENABLED                  0x0001

#define WDT_EXTRST_ARM_RESET              0x0001

/* ide interface */
#define IDE_BASE                          0x02400000

#define IDE_DATA                          (IDE_BASE+0x020)
#define IDE_ERROR                         (IDE_BASE+0x022)
#define IDE_NSECTOR                       (IDE_BASE+0x024)
#define IDE_SECTOR                        (IDE_BASE+0x026)
#define IDE_LCYL                          (IDE_BASE+0x028)
#define IDE_HCYL                          (IDE_BASE+0x02a)
#define IDE_SELECT                        (IDE_BASE+0x02c)
#define IDE_CONTROL                       (IDE_BASE+0x01c)
#define IDE_STATUS                        (IDE_BASE+0x02e)
#define IDE_COMMAND                       (IDE_BASE+0x02e)

/* DMA          */

#define DMA_BASE                          0x30a38

/* CPLD		*/

#define CPLD_BASE                         0x02600000

#define CPLD_PORT0                        (CPLD_BASE+0x00)
#define CPLD_PORT1                        (CPLD_BASE+0x02)
#define CPLD_PORT2                        (CPLD_BASE+0x04)
#define CPLD_PORT3                        (CPLD_BASE+0x06)

/* GIO         */

#define GIO_BASE                          0x30580

#define GIO_DIRECTION0                    (GIO_BASE+0x00)  // GIO 0-15
#define GIO_DIRECTION1                    (GIO_BASE+0x02)  // GIO 16-31
#define GIO_DIRECTION2                    (GIO_BASE+0x04)
#define GIO_INVERT0                       (GIO_BASE+0x06)  // GIO 0-15
#define GIO_INVERT1                       (GIO_BASE+0x08)  // GIO 16-31
#define GIO_INVERT2                       (GIO_BASE+0x0a)
#define GIO_BITSET0                       (GIO_BASE+0x0c)  // GIO 0-15
#define GIO_BITSET1                       (GIO_BASE+0x0e)  // GIO 16-31
#define GIO_BITSET2                       (GIO_BASE+0x10)
#define GIO_BITCLEAR0                     (GIO_BASE+0x12)  // GIO 0-15
#define GIO_BITCLEAR1                     (GIO_BASE+0x14)  // GIO 16-31
#define GIO_BITCLEAR2                     (GIO_BASE+0x16)
#define GIO_ENABLE_IRQ                    (GIO_BASE+0x24)  // GIO 0-7

/* video */

#define VIDEO_BASE                        0x30800
#define LCD_BACK_LIGHT                    0x02600200
#define VIDEO_LCD_VSYNC                   (VIDEO_BASE+0x2C)
#define SCREEN_WIDTH      240
#define SCREEN_REAL_WIDTH 220
#define SCREEN_HEIGHT     176

#define MAX_COL      53
#define MAX_LINE     27

#define LCD_INIT

/* osd */
#define OSD_BASE                          0x30680

/* video in/out */
#define VIDEO_IO_BASE                     0x30780

/** USB state **/

#define USB_STATE                         0x30a24
#define kusbIsConnected()                 ((inw(USB_STATE) & 0x40)!=0)
#define kFWIsConnected()                  ((cpld_read(CPLD3) & 0x8)==0)
#define FW_enable()                       cpld_select(CPLD_FW_EXT,0x1)
#define FW_disable()                      cpld_select(CPLD_FW_EXT,0x0)

/** power state **/

#define POWER_STATE                       0x30a24
#define POWER_CONNECTED                   ((inw(POWER_STATE) >> 0x5)&0x1)

/** I2C **/

/* I2C uses GIO!! */

/** BUTTONS **/

#define BUTTON_BASE                       0x02600008

#define BUTTON_PORT0                      (BUTTON_BASE)
#define BUTTON_PORT1                      (BUTTON_BASE+0x0a)
#define BUTTON_PORT2                      (BUTTON_BASE+0x0c)

#endif  /* __HARDWARE_H */
