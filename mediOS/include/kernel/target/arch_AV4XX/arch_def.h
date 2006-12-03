/*
 * include/kernel/AV4XX/arch_def.h
 *
 * mediOS project
 * Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
 *
 * This file contains the hardware definitions of the AV3XX series machines
 */

#ifndef __AV4XX_DEF_H
#define __AV4XX_DEF_H

#include <sys_def/arch.h>

#define SDRAM_START       0x0900000
#define EXPLOIT_START     0x0DA0000
// RQ: exploit is at 0xDA0000 at end of avi load
#define SDRAM_END         0x1900000

#define IRAM_SIZE         0x8000

#define MALLOC_START  ((((unsigned int)&_end_kernel) & 0xFFFFF000)+0x1000)
#define MALLOC_SIZE   (((unsigned int)&_sdend_start)-MALLOC_START)

#define CONFIG_ARM_CLK 101250000
#define CONFIG_EXT_CLK 27000000

#define CURRENT_ARCH  AV4XX_ARCH

/* uarts */

#define UART0_BASE                        0x00030300
#define UART1_BASE                        0x00030380

/* DEBUG */
#define DEBUG_UART                        UART_0
#define CMD_LINE_UART                     UART_0

#define BUS_ECR                           0x30900
#define BUS_EBYTER                        0x30902
#define BUS_EBITR                         0x30904
#define BUS_REVR                          0x30906

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
#define HD_BASE                          0x05100000
#define CF_BASE                          0x06900000

#define HD_DATA                          (HD_BASE+0x020)
#define HD_ERROR                         (HD_BASE+0x022)
#define HD_NSECTOR                       (HD_BASE+0x024)
#define HD_SECTOR                        (HD_BASE+0x026)
#define HD_LCYL                          (HD_BASE+0x028)
#define HD_HCYL                          (HD_BASE+0x02a)
#define HD_SELECT                        (HD_BASE+0x02c)
#define HD_CONTROL                       (HD_BASE+0x01c)
#define HD_ALTSTATUS                     (HD_BASE+0x01c)
#define HD_STATUS                        (HD_BASE+0x02e)
#define HD_COMMAND                       (HD_BASE+0x02e)

#define CF_DATA                          (CF_BASE+0x020)
#define CF_ERROR                         (CF_BASE+0x022)
#define CF_NSECTOR                       (CF_BASE+0x024)
#define CF_SECTOR                        (CF_BASE+0x026)
#define CF_LCYL                          (CF_BASE+0x028)
#define CF_HCYL                          (CF_BASE+0x02a)
#define CF_SELECT                        (CF_BASE+0x02c)
#define CF_CONTROL                       (CF_BASE+0x01c)
#define CF_ALTSTATUS                     (CF_BASE+0x01c)
#define CF_STATUS                        (CF_BASE+0x02e)
#define CF_COMMAND                       (CF_BASE+0x02e)

/* DMA          */

#define DMA_BASE                          0x30a38

/* CPLD		*/

#define CPLD_BASE                         0x02500000

#define CPLD_PORT0                        (CPLD_BASE+0x00)
#define CPLD_PORT1                        (CPLD_BASE+0x02)
#define CPLD_PORT2                        (CPLD_BASE+0x04)
#define CPLD_PORT3                        (CPLD_BASE+0x06)
#define CPLD_PORT4                        (CPLD_BASE+0x08)
/* video */

#define VIDEO_BASE                        0x30800


/* osd */
#define OSD_BASE                          0x30680

/* video in/out */
#define VIDEO_IO_BASE                     0x30780

/** I2C **/

/* I2C uses GIO!! */

/** BUTTONS **/

#define BUTTON_BASE                       0x02500000

#define BUTTON_PORT0                      (BUTTON_BASE+0x0A)
#define BUTTON_PORT1                      (BUTTON_BASE+0x0C)

#endif  /* __HARDWARE_H */
