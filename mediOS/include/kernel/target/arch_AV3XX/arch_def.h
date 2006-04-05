/*
 * include/kernel/target/av3xx_def.h
 *
 * mediOS project
 * Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
 *
 * This file contains the hardware definitions of the AV3XX series machines
 */

#ifndef __AV3XX_DEF_H
#define __AV3XX_DEF_H

#include <sys_def/arch.h>

#define SDRAM_START   0x00900000
#define SDRAM_END     0x01900000

#define MALLOC_START  ((((unsigned int)&_end_kernel) & 0xFFFFF000)+0x1000)
#define MALLOC_SIZE   (SDRAM_END-MALLOC_START)

#define CONFIG_ARM_CLK 54000000

#define CURRENT_ARCH  AV3XX_ARCH

/* uarts */

#define UART0_BASE                        0x00030300
#define UART1_BASE                        0x00030380

/* DEBUG */
#define DEBUG_UART              UART_0

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

#define IDE_DATA                          (IDE_BASE+0x000)
#define IDE_ERROR                         (IDE_BASE+0x080)
#define IDE_NSECTOR                       (IDE_BASE+0x100)
#define IDE_SECTOR                        (IDE_BASE+0x180)
#define IDE_LCYL                          (IDE_BASE+0x200)
#define IDE_HCYL                          (IDE_BASE+0x280)
#define IDE_SELECT                        (IDE_BASE+0x300)
#define IDE_CONTROL                       (IDE_BASE+0x340)
#define IDE_STATUS                        (IDE_BASE+0x380)
#define IDE_COMMAND                       (IDE_BASE+0x380)

/* DMA          */

#define DMA_BASE                          0x30a38

/* CPLD		*/

#define CPLD_BASE                         0x02600000

#define CPLD_PORT0                        (CPLD_BASE+0x000)
#define CPLD_PORT1                        (CPLD_BASE+0x100)
#define CPLD_PORT2                        (CPLD_BASE+0x200)
#define CPLD_PORT3                        (CPLD_BASE+0x300)

/* video */

#define VIDEO_BASE                        0x30800

/* osd */
#define OSD_BASE                          0x30680



/* video in/out */
#define VIDEO_IO_BASE                     0x30780



/** I2C **/

/* I2C uses GIO!! */

/** BUTTONS **/

#define BUTTON_BASE                       0x02600680

#define BUTTON_PORT0          (BUTTON_BASE)
#define BUTTON_PORT1          (BUTTON_BASE+0x80)
#define BUTTON_PORT2          (BUTTON_BASE+0x100)

#define READ_BUTTONS(VAL)                {       \
    VAL =  inw(BUTTON_PORT0)&0x3;            \
    VAL|=((inw(BUTTON_PORT1)&0x7)<<2);           \
    VAL|=((inw(BUTTON_PORT2)&0x7)<<5);           \
    /* ON, OFF keys */                           \
    if(gio_is_set(GIO_ON_BTN))  VAL |= (0x1<<8); \
    if(gio_is_set(GIO_OFF_BTN)) VAL |= (0x1<<9); \
    VAL = (~VAL)&0x3FF;                          \
}

#endif  /* __HARDWARE_H */
