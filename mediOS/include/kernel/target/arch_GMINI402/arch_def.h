/*
 * include/kernel/target/gmini4xx_def.h
 *
 * mediOS project
 * Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
 *
 * This file contains the hardware definitions of the AV3XX series machines
 */

#ifndef __GMINI402_DEF_H
#define __GMINI402_DEF_H

#include <sys_def/arch.h>

#define SDRAM_START       0x0900000
#define SDRAM_END         0x18F0000

#define IRAM_SIZE         0x4000

#define MALLOC_START  ((((unsigned int)&_end_kernel) & 0xFFFFF000)+0x1000)
#define MALLOC_SIZE   (SDRAM_END-MALLOC_START)

#define CONFIG_ARM_CLK 100000000
#define CONFIG_EXT_CLK 27000000

#define CURRENT_ARCH  GMINI402_ARCH

/* uarts */

#define UART0_BASE                        0x00030300
#define UART1_BASE                        0x00030380

/* DEBUG */
#define DEBUG_UART                        UART_0
#define CMD_LINE_UART                     UART_0

/* Interrupts */

#define INTC_BASE                         0x00030500

#define INTC_FIQ0_STATUS                  (INTC_BASE + 0x0000)
#define INTC_FIQ1_STATUS                  (INTC_BASE + 0x0002)
#define INTC_FIQ2_STATUS                  (INTC_BASE + 0x0004)
#define INTC_IRQ0_STATUS                  (INTC_BASE + 0x0008)
#define INTC_IRQ1_STATUS                  (INTC_BASE + 0x000a)
#define INTC_IRQ2_STATUS                  (INTC_BASE + 0x000c)

#define INTC_FIQ0_ENTRY                   (INTC_BASE + 0x0010)
#define INTC_FIQ1_ENTRY                   (INTC_BASE + 0x0012)
#define INTC_IRQ0_ENTRY                   (INTC_BASE + 0x0018)
#define INTC_IRQ1_ENTRY                   (INTC_BASE + 0x001A)

#define INTC_FISEL0                       (INTC_BASE + 0x0020)
#define INTC_FISEL1                       (INTC_BASE + 0x0022)
#define INTC_FISEL2                       (INTC_BASE + 0x0024)
#define INTC_INT0_ENABLE                  (INTC_BASE + 0x0028)
#define INTC_INT1_ENABLE                  (INTC_BASE + 0x002a)
#define INTC_INT2_ENABLE                  (INTC_BASE + 0x002c)

#define INTC_INTRAW                       (INTC_BASE + 0x0030)

#define INTC_EABASE0                      (INTC_BASE + 0x0038)
#define INTC_EABASE1                      (INTC_BASE + 0x003A)

#define INTC_IRQ_STATUS(val)              ((val < 16) ? INTC_IRQ0_STATUS : ((val<32) ? INTC_IRQ1_STATUS : INTC_IRQ2_STATUS))
#define INTC_IRQ_ENABLE(val)              ((val < 16) ? INTC_INT0_ENABLE : ((val<32) ? INTC_INT1_ENABLE : INTC_INT2_ENABLE))
#define INTC_FIQ_STATUS(val)              ((val < 16) ? INTC_FIQ0_STATUS : ((val<32) ? INTC_FIQ1_STATUS : INTC_FIQ2_STATUS))
#define INTC_FIQ_ENABLE(val)              ((val < 16) ? INTC_INT0_ENABLE : ((val<32) ? INTC_INT1_ENABLE : INTC_INT2_ENABLE))
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
#define IDE_BASE                          0x50000000

#define IDE_DATA                          (IDE_BASE+0xa0)
#define IDE_ERROR                         (IDE_BASE+0xa2)
#define IDE_NSECTOR                       (IDE_BASE+0x24)
#define IDE_SECTOR                        (IDE_BASE+0x26)
#define IDE_LCYL                          (IDE_BASE+0x28)
#define IDE_HCYL                          (IDE_BASE+0x2a)
#define IDE_SELECT                        (IDE_BASE+0x2c)
#define IDE_CONTROL                       (IDE_BASE+0x9c)
#define IDE_STATUS                        (IDE_BASE+0xae)
#define IDE_COMMAND                       (IDE_BASE+0x2e)

/* DMA          */

#define DMA_BASE                          0x30a54

/* CPLD		*/

#define CPLD_BASE                         0x40400000

#define CPLD_PORT0                        (CPLD_BASE+0x00)
#define CPLD_PORT1                        (CPLD_BASE+0x02)
#define CPLD_PORT2                        (CPLD_BASE+0x04)
#define CPLD_PORT3                        (CPLD_BASE+0x06)
#define CPLD_PORT4                        (CPLD_BASE+0x000)
/* video */

#define VIDEO_BASE                        0x30800


/* osd */
#define OSD_BASE                          0x30680

/* video in/out */
#define VIDEO_IO_BASE                     0x30780

/** I2C **/

/* I2C uses GIO!! */

/** BUTTONS **/

#define BUTTON_BASE                       0x02600000

#define BUTTON_PORT0                      (BUTTON_BASE+0x08)
#define BUTTON_PORT1                      (BUTTON_BASE+0x0a)
#define BUTTON_PORT2                      (BUTTON_BASE+0x0c)

#endif  /* __HARDWARE_H */
