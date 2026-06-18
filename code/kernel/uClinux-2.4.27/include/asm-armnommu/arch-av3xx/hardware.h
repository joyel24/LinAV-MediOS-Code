/*
 * linux/include/asm-arm/arch-trio/hardware.h
 *
 * Copyright (C) 1996 Russell King.
 *
 * This file contains the hardware definitions of the AV3XX series machines
 */

#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

#define CONFIG_DEBUG_LL 1
#define DEBUG_ENABLED 1

#define doRetTest(val,func_name,errmsg)       if(val<0) { printk("[func] ermssg\n"); return -1;}


#ifdef CONFIG_ARM_CLK
	#undef CONFIG_ARM_CLK
#endif
#define CONFIG_ARM_CLK 54000000
/* ARM clock rate (MHz) */
//#define AV3XX_CLOCK_RATE     37125000

/* uarts (note that the register fields aren't needed by the standard linux
   serial driver -- we only use them for our initial debugging console) */
#define AV3XX_UART0_BASE                        0x00030300
#define AV3XX_UART1_BASE                        0x00030380

#define AV3XX_UART_DTRR                         0x00000000
#define AV3XX_UART_BRSR                         0x00000002
#define AV3XX_UART_MSR                          0x00000004
#define AV3XX_UART_RFCR                         0x00000006
#define AV3XX_UART_TFCR                         0x00000008
#define AV3XX_UART_LCR	                        0x0000000a
#define AV3XX_UART_SR 	                        0x0000000c

#define AV3XX_UART_DTRR_DTR_MASK                0x00ff
#define AV3XX_UART_DTRR_PEF                     0x0100
#define AV3XX_UART_DTRR_ORF                     0x0200
#define AV3XX_UART_DTRR_FE                      0x0400
#define AV3XX_UART_DTRR_BF                      0x0800
#define AV3XX_UART_DTRR_RVF                     0x1000
#define AV3XX_UART_DTRR_RSV_MASK                0xe000

#define AV3XX_UART_BRSR_VAL(baud)               ((CONFIG_ARM_CLK/(16*(baud))) - 1)

#define AV3XX_UART_MSR_CLS                      0x0001
#define AV3XX_UART_MSR_SBLS                     0x0004
#define AV3XX_UART_MSR_PSB                      0x0008
#define AV3XX_UART_MSR_PEB                      0x0010
#define AV3XX_UART_MSR_TOIC_MASK                0x0c00
#define AV3XX_UART_MSR_REIE                     0x2000
#define AV3XX_UART_MSR_TFTIE                    0x4000
#define AV3XX_UART_MSR_RFTIE                    0x8000
#define AV3XX_UART_MSR_RSV_MASK                 0x13e2

#define AV3XX_UART_RFCR_RWC_MASK                0x003f
#define AV3XX_UART_RFCR_RTL_MASK                0x0700
#define AV3XX_UART_RFCR_RDEF                    0x4000
#define AV3XX_UART_RFCR_RFCB                    0x8000
#define AV3XX_UART_RFCR_RSV_MASK                0x38c0

#define AV3XX_UART_TFCR_TWC_MASK                0x003f
#define AV3XX_UART_TFCR_TTL_MASK                0x0700
#define AV3XX_UART_TFCR_TFCB                    0x8000
#define AV3XX_UART_TFCR_RSV_MASK                0x78c0

#define AV3XX_UART_LCR_BOC                      0x0100
#define AV3XX_UART_LCR_UTST                     0x4000
#define AV3XX_UART_LCR_RSV_MASK                 0xbeff

#define AV3XX_UART_SR_TREF                      0x0001
#define AV3XX_UART_SR_TFEF                      0x0002
#define AV3XX_UART_SR_RFNEF                     0x0004
#define AV3XX_UART_SR_TOIF                      0x0100
#define AV3XX_UART_SR_RFER                      0x0200
#define AV3XX_UART_SR_TFTI                      0x0400
#define AV3XX_UART_SR_RFTI                      0x0800
#define AV3XX_UART_SR_RSV_MASK                  0xf0f8

#define AV3XX_UART_DTRR_VALID                   (AV3XX_UART_DTRR_RVF)

#define AV3XX_UART_MSR_8_DBITS                  0x0000
#define AV3XX_UART_MSR_7_DBITS                  (AV3XX_UART_MSR_CLS)
#define AV3XX_UART_MSR_1_SBITS                  0x0000
#define AV3XX_UART_MSR_2_SBITS                  (AV3XX_UART_MSR_SBLS)
#define AV3XX_UART_MSR_PARITY_MASK              (AV3XX_UART_MSR_PSB|AV3XX_UART_MSR_PEB)
#define AV3XX_UART_MSR_NO_PARITY                0x0000
#define AV3XX_UART_MSR_EVEN_PARITY              (AV3XX_UART_MSR_PEB)
#define AV3XX_UART_MSR_ODD_PARITY               (AV3XX_UART_MSR_PSB|AV3XX_UART_MSR_PEB)
#define AV3XX_UART_MSR_TIMEOUT_DISABLED         0x0000
#define AV3XX_UART_MSR_TIMEOUT_3                0x0400
#define AV3XX_UART_MSR_TIMEOUT_7                0x0800
#define AV3XX_UART_MSR_TIMEOUT_15               0x0c00
#define AV3XX_UART_MSR_INTS                     0xfc00    // All interrupt bits
#define AV3XX_UART_MSR_MODE_BITS                0x001f // data length, stop & parity

#define AV3XX_UART_RFCR_TRG_1                   0x0000
#define AV3XX_UART_RFCR_TRG_4                   0x0100
#define AV3XX_UART_RFCR_TRG_8                   0x0200
#define AV3XX_UART_RFCR_TRG_16                  0x0300
#define AV3XX_UART_RFCR_TRG_24                  0x0400
#define AV3XX_UART_RFCR_TRG_32                  0x0500
#define AV3XX_UART_RFCR_RESET                   AV3XX_UART_RFCR_RDEF
#define AV3XX_UART_RFCR_CLEAR                   AV3XX_UART_RFCR_RFCB

#define AV3XX_UART_TFCR_TRG_1                   0x0000
#define AV3XX_UART_TFCR_TRG_4                   0x0100
#define AV3XX_UART_TFCR_TRG_8                   0x0200
#define AV3XX_UART_TFCR_TRG_16                  0x0300
#define AV3XX_UART_TFCR_TRG_24                  0x0400
#define AV3XX_UART_TFCR_TRG_32                  0x0500
#define AV3XX_UART_TFCR_CLEAR                   AV3XX_UART_TFCR_TFCB

#define AV3XX_UART_RXFIFO_BYTESIZE              32
#define AV3XX_UART_TXFIFO_BYTESIZE              32


/*
 * HARD_RESET_NOW -- used in blkmem.c. Should call arch_hard_reset(), but I
 * don't appear to have one ;).
 * --gmcnutt
 */
#define HARD_RESET_NOW()

/* timers */

#define AV3XX_TIMER0_BASE                       0x00030000
#define AV3XX_TIMER1_BASE                       0x00030080
#define AV3XX_TIMER2_BASE                       0x00030100
#define AV3XX_TIMER3_BASE                       0x00030180

#define AV3XX_TIMER_MODE                        0x00000000
#define AV3XX_TIMER_SEL                         0x00000002
#define AV3XX_TIMER_SCAL                        0x00000004
#define AV3XX_TIMER_DIV                         0x00000006
#define AV3XX_TIMER_TRG                         0x00000008
#define AV3XX_TIMER_CNT                         0x0000000a

#define AV3XX_TIMER0_MODE                       (AV3XX_TIMER0_BASE+AV3XX_TIMER_MODE)
#define AV3XX_TIMER1_MODE                       (AV3XX_TIMER0_BASE+AV3XX_TIMER_MODE)
#define AV3XX_TIMER2_MODE                       (AV3XX_TIMER0_BASE+AV3XX_TIMER_MODE)
#define AV3XX_TIMER3_MODE                       (AV3XX_TIMER0_BASE+AV3XX_TIMER_MODE)

#define AV3XX_TMR_MODE_STOP                     0x0000
#define AV3XX_TMR_MODE_ONESHOT                  0x0001
#define AV3XX_TMR_MODE_FREERUN                  0x0002
#define AV3XX_TMR_MODE_RES                      0x0003

#define AV3XX_TMR_SEL_ARM                       0x0000
#define AV3XX_TMR_SEL_EXT                       0x0001

/* Interrupts */

#define AV3XX_INTC_BASE                         0x00030500

#define AV3XX_INTC_FIQ0_STATUS                  (AV3XX_INTC_BASE + 0x0000)
#define AV3XX_INTC_FIQ1_STATUS                  (AV3XX_INTC_BASE + 0x0002)
#define AV3XX_INTC_IRQ0_STATUS                  (AV3XX_INTC_BASE + 0x0004)
#define AV3XX_INTC_IRQ1_STATUS                  (AV3XX_INTC_BASE + 0x0006)

#define AV3XX_INTC_FISEL0                       (AV3XX_INTC_BASE + 0x0020)
#define AV3XX_INTC_FISEL1                       (AV3XX_INTC_BASE + 0x0022)
#define AV3XX_INTC_IRQ0_ENABLE                  (AV3XX_INTC_BASE + 0x0024)
#define AV3XX_INTC_IRQ1_ENABLE                  (AV3XX_INTC_BASE + 0x0026)

#define AV3XX_INTC_INTIDR                       (AV3XX_INTC_BASE + 0x0040)
#define AV3XX_INTC_INTRAW                       (AV3XX_INTC_BASE + 0x0042)

#define AV3XX_INTC_IRQ_STATUS(val)              ((val < 16) ? AV3XX_INTC_IRQ0_STATUS : AV3XX_INTC_IRQ1_STATUS)
#define AV3XX_INTC_IRQ_ENABLE(val)              ((val < 16) ? AV3XX_INTC_IRQ0_ENABLE : AV3XX_INTC_IRQ1_ENABLE)
#define AV3XX_INTC_FIQ_STATUS(val)              ((val < 16) ? AV3XX_INTC_FIQ0_STATUS : AV3XX_INTC_FIQ1_STATUS)
#define AV3XX_INTC_FIQ_ENABLE(val)              ((val < 16) ? AV3XX_INTC_FIQ0_ENABLE : AV3XX_INTC_FIQ1_ENABLE)
#define AV3XX_INTC_SHIFT(val)                   ((val < 16) ? val : val - 16)
#define AV3XX_INTC_FIQ_SHIFT(val)               AV3XX_INTC_SHIFT(val)
#define AV3XX_INTC_IRQ_SHIFT(val)               AV3XX_INTC_SHIFT(val)

/* WatchDog */

#define AV3XX_WDT_BASE                          0x30400

#define AV3XX_WDT_ENABLE                        (AV3XX_WDT_BASE + 0x0000)
#define AV3XX_WDT_RESET                         (AV3XX_WDT_BASE + 0x0002)

/* ide interface */
#define AV3XX_IDE_BASE                          0x02400000

#define AV3XX_IDE_IRQ                           1

#define AV3XX_NR_PORTS                          10

#define AV3XX_DMA_BASE                          0x30a38

/* CPLD		*/
#if 0
#define AV3XX_CPLD_BASE                         0x02600000



#define AV3XX_SELECT_DEVICE(devNum)             outb(devNum,AV3XX_CPLD_PORT0);

#define AV3XX_CPLD_SET_0(val)                   outb(val,AV3XX_CPLD_PORT0);
#define AV3XX_CPLD_SET_1(val)                   outb(val,AV3XX_CPLD_PORT1);
#define AV3XX_CPLD_SET_2(val)                   outb(val,AV3XX_CPLD_PORT2);
#define AV3XX_CPLD_SET_3(val)                   outb(val,AV3XX_CPLD_PORT3);

#define AV3XX_CPLD_GET_0()                      inb(AV3XX_CPLD_PORT0)
#define AV3XX_CPLD_GET_1()                      inb(AV3XX_CPLD_PORT1)
#define AV3XX_CPLD_GET_2()                      inb(AV3XX_CPLD_PORT2)
#define AV3XX_CPLD_GET_3()                      inb(AV3XX_CPLD_PORT3)

#define AV3XX_SELECT_HDD_BIT                    0x0 /* hard drive                   */
#define AV3XX_SELECT_CFC_BIT                    0x1 /* Compact flash card access    */
#define AV3XX_SELECT_VIDIN_BIT                  0x4 /* video in                     */
#define AV3XX_SELECT_IR_BIT                     0x4 /* and IR                       */
#define AV3XX_SELECT_SMR_BIT                    0x8 /* Smart media reader           */
#define AV3XX_SELECT_MCR_BIT                    0x9 /* multimedia card reader       */

#define AV3XX_IDE_POWERED                       0x30a24

#define AV3XX_POWER_UP_HDD                      AV3XX_CPLD_SET_3(0xa);
#define AV3XX_POWER_DOWN_HDD                    AV3XX_CPLD_SET_3(0x0);
#define AV3XX_SELECT_HDD                        AV3XX_SELECT_DEVICE(AV3XX_SELECT_HDD_BIT);

#endif


/* video */

#define AV3XX_VIDEO_BASE                        0x30800


#define AV3XX_LCD_BACK_LIGHT                    0x02600200

/* osd */
#define AV3XX_OSD_BASE                          0x30680

#define AV3XX_OSD_CONF                          (AV3XX_OSD_BASE+0x00)   // OSD Main config and border color
#define AV3XX_OSD_VID0_1_CONF                   (AV3XX_OSD_BASE+0x02)   // Video0 and video1 config
#define AV3XX_OSD_BITMAP0_CONF                  (AV3XX_OSD_BASE+0x04)   // Bitmap0 config
#define AV3XX_OSD_BITMAP1_CONF                  (AV3XX_OSD_BASE+0x06)   // Bitmap1 config
#define AV3XX_OSD_CURSOR0_CONF                  (AV3XX_OSD_BASE+0x08)   // Cursor0 config
#define AV3XX_OSD_CURSOR1_CONF                  (AV3XX_OSD_BASE+0x0a)   // Cursor1 config

#define AV3XX_OSD_COMP_CONF(component)          (AV3XX_OSD_CONF+(component*2)) // warning result is wrong for comp 0 & 1

#define AV3XX_OSD_VID0_BUFF_W                   (AV3XX_OSD_BASE+0x0c)   // Video0 buffer width
#define AV3XX_OSD_VID1_BUFF_W                   (AV3XX_OSD_BASE+0x0e)   // Video1 buffer width
#define AV3XX_OSD_BITMAP0_BUFF_W                (AV3XX_OSD_BASE+0x10)   // Bitmap0 buffer width
#define AV3XX_OSD_BITMAP1_BUFF_W                (AV3XX_OSD_BASE+0x12)   // Bitmap1 buffer width

#define AV3XX_OSD_COMP_BUFF_W(component)        (AV3XX_OSD_VID0_BUFF_W+(component*2))

#define AV3XX_OSD_SDRAM_OFF_HI_VID0_1           (AV3XX_OSD_BASE+0x14)   // SDRAM offset HI Video0 | Video1
#define AV3XX_OSD_SDRAM_OFF_VID0                (AV3XX_OSD_BASE+0x16)   // SDRAM offset video0
#define AV3XX_OSD_SDRAM_OFF_VID1                (AV3XX_OSD_BASE+0x18)   // SDRAM offset video1
#define AV3XX_OSD_SDRAM_OFF_HI_BITMAP0_1        (AV3XX_OSD_BASE+0x1a)   // SDRAM offset HI Bitmap0 | Bitmap1
#define AV3XX_OSD_SDRAM_OFF_BITMAP0             (AV3XX_OSD_BASE+0x1c)   // SDRAM offset Bitmap0
#define AV3XX_OSD_SDRAM_OFF_BITMAP1             (AV3XX_OSD_BASE+0x1e)   // SDRAM offset Bitmap1

#define AV3XX_OSD_BITMAP0_SHIFT_HORIZ           (AV3XX_OSD_BASE+0x20)   // Main shift horizontal
#define AV3XX_OSD_BITMAP0_SHIFT_VERT            (AV3XX_OSD_BASE+0x22)   // Main shift vertical

#define AV3XX_OSD_VID0_X                        (AV3XX_OSD_BASE+0x24)   // Video0 X
#define AV3XX_OSD_VID0_Y                        (AV3XX_OSD_BASE+0x26)   // Video0 Y
#define AV3XX_OSD_VID0_W                        (AV3XX_OSD_BASE+0x28)   // Video0 width
#define AV3XX_OSD_VID0_H                        (AV3XX_OSD_BASE+0x2a)   // Video0 height

#define AV3XX_OSD_VID1_X                        (AV3XX_OSD_BASE+0x2c)   // Video1 X
#define AV3XX_OSD_VID1_Y                        (AV3XX_OSD_BASE+0x2e)   // Video1 Y
#define AV3XX_OSD_VID1_W                        (AV3XX_OSD_BASE+0x30)   // Video1 width
#define AV3XX_OSD_VID1_H                        (AV3XX_OSD_BASE+0x32)   // Video1 height

#define AV3XX_OSD_BITMAP0_X                     (AV3XX_OSD_BASE+0x34)   // Bitmap0 X
#define AV3XX_OSD_BITMAP0_Y                     (AV3XX_OSD_BASE+0x36)   // Bitmap0 Y
#define AV3XX_OSD_BITMAP0_W                     (AV3XX_OSD_BASE+0x38)   // Bitmap0 width
#define AV3XX_OSD_BITMAP0_H                     (AV3XX_OSD_BASE+0x3a)   // Bitmap0 height

#define AV3XX_OSD_BITMAP1_X                     (AV3XX_OSD_BASE+0x3c)   // Bitmap1 X
#define AV3XX_OSD_BITMAP1_Y                     (AV3XX_OSD_BASE+0x3e)   // Bitmap1 Y
#define AV3XX_OSD_BITMAP1_W                     (AV3XX_OSD_BASE+0x40)   // Bitmap1 width
#define AV3XX_OSD_BITMAP1_H                     (AV3XX_OSD_BASE+0x42)   // Bitmap1 height

#define AV3XX_OSD_CURSOR0_X                     (AV3XX_OSD_BASE+0x44)   // Cursor0 X
#define AV3XX_OSD_CURSOR0_Y                     (AV3XX_OSD_BASE+0x46)   // Cursor0 Y
#define AV3XX_OSD_CURSOR0_W                     (AV3XX_OSD_BASE+0x48)   // Cursor0 width
#define AV3XX_OSD_CURSOR0_H                     (AV3XX_OSD_BASE+0x4a)   // Cursor0 height

#define AV3XX_OSD_COMP_W(component)             (AV3XX_OSD_VID0_W+(component*8))
#define AV3XX_OSD_COMP_H(component)             (AV3XX_OSD_VID0_H+(component*8))

#define AV3XX_OSD_CURSOR1_X                     (AV3XX_OSD_BASE+0x4c)   // Cursor1 X
#define AV3XX_OSD_CURSOR1_Y                     (AV3XX_OSD_BASE+0x4e)   // Cursor1 Y

#define AV3XX_OSD_COMP_X(component)             (AV3XX_OSD_VID0_X+(component*8))
#define AV3XX_OSD_COMP_Y(component)             (AV3XX_OSD_VID0_Y+(component*8))

#define AV3XX_OSD_BANK1_0_1                     (AV3XX_OSD_BASE+0x50)   // 16c bank1 pallette #0 | #1
#define AV3XX_OSD_BANK1_2_3                     (AV3XX_OSD_BASE+0x52)   // 16c bank1 pallette #2 | #3
#define AV3XX_OSD_BANK1_4_5                     (AV3XX_OSD_BASE+0x54)   // 16c bank1 pallette #4 | #5
#define AV3XX_OSD_BANK1_6_7                     (AV3XX_OSD_BASE+0x56)   // 16c bank1 pallette #6 | #7
#define AV3XX_OSD_BANK1_8_9                     (AV3XX_OSD_BASE+0x58)   // 16c bank1 pallette #8 | #9
#define AV3XX_OSD_BANK1_a_b                     (AV3XX_OSD_BASE+0x5a)   // 16c bank1 pallette #a | #b
#define AV3XX_OSD_BANK1_c_d                     (AV3XX_OSD_BASE+0x5c)   // 16c bank1 pallette #c | #d
#define AV3XX_OSD_BANK1_e_f                     (AV3XX_OSD_BASE+0x5e)   // 16c bank1 pallette #e | #f

#define AV3XX_OSD_BANK2_0_1                     (AV3XX_OSD_BASE+0x60)   // 16c bank2 pallette #0 | #1
#define AV3XX_OSD_BANK2_2_3                     (AV3XX_OSD_BASE+0x62)   // 16c bank2 pallette #2 | #3
#define AV3XX_OSD_BANK2_4_5                     (AV3XX_OSD_BASE+0x64)   // 16c bank2 pallette #4 | #5
#define AV3XX_OSD_BANK2_6_7                     (AV3XX_OSD_BASE+0x66)   // 16c bank2 pallette #6 | #7
#define AV3XX_OSD_BANK2_8_9                     (AV3XX_OSD_BASE+0x68)   // 16c bank2 pallette #8 | #9
#define AV3XX_OSD_BANK2_a_b                     (AV3XX_OSD_BASE+0x6a)   // 16c bank2 pallette #a | #b
#define AV3XX_OSD_BANK2_c_d                     (AV3XX_OSD_BASE+0x6c)   // 16c bank2 pallette #c | #d
#define AV3XX_OSD_BANK2_e_f                     (AV3XX_OSD_BASE+0x6e)   // 16c bank2 pallette #e | #f


#define AV3XX_GET_BANK(bankN)                   ((bankN == 0) ? AV3XX_OSD_BANK1_0_1 : AV3XX_OSD_BANK2_0_1)
#define AV3XX_GET_BANK_ADDR(bankN,index)        (AV3XX_GET_BANK(bankN) + ((index>>1)<<1))
#define AV3XX_GET_BANK_SHIFT(index)             ((( index<<31 ) >>31)<<3)

//				(AV3XX_OSD_BASE+0x70)	306f0   ? Unknown
#define AV3XX_OSD_CURSOR2_DATA                  (AV3XX_OSD_BASE+0x72)   // Cursor2 data
#define AV3XX_OSD_PAL_ACCESS_STATUS             (AV3XX_OSD_BASE+0x74)   // Pallette access status
#define AV3XX_OSD_CURSOR2_ADD_LATCH             (AV3XX_OSD_BASE+0x74)   // cursor2 address/latch
#define AV3XX_OSD_PAL_DATA_WRITE                (AV3XX_OSD_BASE+0x76)   // Pallette data write
#define AV3XX_OSD_PAL_INDEX_WRITE               (AV3XX_OSD_BASE+0x78)   // Pallette index write

#define AV3XX_OSD_ALT_VID_OFF_LO                (AV3XX_OSD_BASE+0x7c)   // Alternative video sdram offset LO
#define AV3XX_OSD_ALT_VID_OFF_HI                (AV3XX_OSD_BASE+0x7e)   // Alternative video sdram offset HI

/** USB state **/

#define AV3XX_USB_STATE                         0x30a24

/** power state **/

#define AV3XX_POWER_STATE                       0x30a24

/** I2C **/

#if 0
#define AV3XX_I2C_BASE                          0x30580

#define AV3XX_I2C_REG_DR                        (AV3XX_I2C_BASE+0x02)
#define AV3XX_I2C_REG_IN                        (AV3XX_I2C_BASE+0x0A)
#define AV3XX_I2C_REG_IO                        (AV3XX_I2C_BASE+0x0E)
#endif






#endif  /* _ASM_ARCH_HARDWARE_H */
