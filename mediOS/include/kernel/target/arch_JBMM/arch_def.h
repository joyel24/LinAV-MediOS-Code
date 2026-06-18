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

#define SDRAM_START   0x08000000
#define SDRAM_END     0x09000000

#define IRAM_SIZE         0x8000

#define MALLOC_START  ((((unsigned int)&_end_kernel) & 0xFFFFF000)+0x1000)
#define MALLOC_SIZE   (((unsigned int)&_sdend_start)-MALLOC_START)

#define CONFIG_ARM_CLK 37125000
#define CONFIG_EXT_CLK 27000000

#define CURRENT_ARCH  JBMM_ARCH

/* uarts */

#define UART0_BASE                        0x00030380
#define UART1_BASE                        0x00030400

#define BUS_ECR                           0x30900
#define BUS_EBYTER                        0x30902
#define BUS_EBITR                         0x30904
#define BUS_REVR                          0x30906

/* DEBUG */
#define DEBUG_UART              UART_1
#define CMD_LINE_UART           UART_1
#define FM_REMOTE_UART          UART_1

#define FM_VER                  1

/* Interrupts */

#define INTC_BASE                         0x00030580

#define INTC_FIQ0_STATUS                  (INTC_BASE + 0x0000)
#define INTC_FIQ1_STATUS                  (0)
#define INTC_IRQ0_STATUS                  (INTC_BASE + 0x0002)
#define INTC_IRQ1_STATUS                  (INTC_BASE + 0x0004)

#define INTC_FIQ0_ENTRY                   (INTC_BASE + 0x0008)
#define INTC_FIQ1_ENTRY                   (INTC_BASE + 0x000A)
#define INTC_IRQ0_ENTRY                   (INTC_BASE + 0x000C)
#define INTC_IRQ1_ENTRY                   (INTC_BASE + 0x000E)

#define INTC_FISEL0                       (INTC_BASE + 0x0020)
#define INTC_FISEL1                       (0)
#define INTC_INT0_ENABLE                  (INTC_BASE + 0x0022)
#define INTC_INT1_ENABLE                  (INTC_BASE + 0x0024)

#define INTC_INTIDR                       (INTC_BASE + 0x0040)
#define INTC_INTRAW                       (INTC_BASE + 0x0042)

#define INTC_IRQ_STATUS(val)              ((val < 16) ? INTC_IRQ1_STATUS : INTC_IRQ0_STATUS)
#define INTC_IRQ_ENABLE(val)              ((val < 16) ? INTC_INT1_ENABLE : INTC_INT0_ENABLE)
#define INTC_FIQ_STATUS(val)              ((val < 16) ? INTC_FIQ0_STATUS : INTC_FIQ1_STATUS)
#define INTC_FIQ_ENABLE(val)              ((val < 16) ? INTC_INT0_ENABLE : INTC_INT1_ENABLE)

#define INTC_SHIFT(val)                   ((val < 16) ? val : val-16)
#define INTC_FIQ_SHIFT(val)               INTC_SHIFT(val)
#define INTC_IRQ_SHIFT(val)               INTC_SHIFT(val)

/* WatchDog */

#define WDT_BASE                          0x30300

#define WDT_MODE                          (WDT_BASE + 0x0000)
#define WDT_RESET                         (WDT_BASE + 0x0002)
#define WDT_SCAL                          (WDT_BASE + 0x0004)
#define WDT_DIV                           (WDT_BASE + 0x0006)
//#define WDT_EXT_RESET                     (WDT_BASE + 0x0008)

#define WDT_MODE_ARM_RESET                0x0004
#define WDT_MODE_ENABLED                  0x0001

#define WDT_EXTRST_ARM_RESET              0x0001

/* ide interface */
#define HD_BASE                          0x04000000
#define CF_BASE                          0x04000000

#define HD_DATA                          (HD_BASE+0x800)
#define HD_ERROR                         (HD_BASE+0x900)
#define HD_NSECTOR                       (HD_BASE+0xa00)
#define HD_SECTOR                        (HD_BASE+0xb00)
#define HD_LCYL                          (HD_BASE+0xc00)
#define HD_HCYL                          (HD_BASE+0xd00)
#define HD_SELECT                        (HD_BASE+0xe00)
#define HD_CONTROL                       (HD_BASE+0xe80)
#define HD_ALTSTATUS                     (HD_BASE+0xe80)
#define HD_STATUS                        (HD_BASE+0xf00)
#define HD_COMMAND                       (HD_BASE+0xf00)

#define CF_DATA                          (CF_BASE+0x800)
#define CF_ERROR                         (CF_BASE+0x900)
#define CF_NSECTOR                       (CF_BASE+0xa00)
#define CF_SECTOR                        (CF_BASE+0xb00)
#define CF_LCYL                          (CF_BASE+0xc00)
#define CF_HCYL                          (CF_BASE+0xd00)
#define CF_SELECT                        (CF_BASE+0xe00)
#define CF_CONTROL                       (CF_BASE+0xe80)
#define CF_ALTSTATUS                     (CF_BASE+0xe80)
#define CF_STATUS                        (CF_BASE+0xf00)
#define CF_COMMAND                       (CF_BASE+0xf00)


/* DMA          */

#define DMA_BASE                          0x30b80

/* CPLD		*/

#define CPLD_BASE                         0x02400000

#define CPLD_PORT0                        (CPLD_BASE+0x000)
#define CPLD_PORT1                        (CPLD_BASE+0x100)
#define CPLD_PORT2                        (CPLD_BASE+0x200)
#define CPLD_PORT3                        (CPLD_BASE+0x300)
#define CPLD_PORT4                        (CPLD_BASE+0x000)
/* video */
#define VIDEO_BASE                        0x30900

/* osd */
#define OSD_BASE                          0x30680

/* video in/out */
#define VIDEO_IO_BASE                     0x30780



/** I2C **/

/* I2C uses GIO!! */

/** BUTTONS **/

#define BUTTON_BASE                       0x02400680

#define BUTTON_PORT0          (BUTTON_BASE)
#define BUTTON_PORT1          (BUTTON_BASE+0x80)
#define BUTTON_PORT2          (BUTTON_BASE+0x100)

#define READ_BUTTONS(VAL)                {       \
    VAL =  inw(BUTTON_PORT0)&0xF;            \
    VAL|=((inw(BUTTON_PORT1)&0xF)<<4);           \
    /* ON, OFF keys */                           \
    if(gio_is_set(GIO_ON_BTN))  VAL |= (0x1<<8); \
    if(gio_is_set(GIO_OFF_BTN)) VAL |= (0x1<<9); \
    VAL = (~VAL)&0x3FF;                          \
}

#endif  /* __HARDWARE_H */
