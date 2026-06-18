/*
 * linux/include/asm-armnommu/arch-av3xx/gio.h
 *
 * Copyright (c) 1998 Hugo Fiennes & Nicolas Pitre
 *
 * 18-aug-2000: Cleanup by Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *              Get rid of the special ide_init_hwif_ports() functions
 *              and make a generalised function that can be used by all
 *              architectures.
 */

#ifndef __ASM_ARCH_GIO_H
#define __ASM_ARCH_GIO_H

#define GIO_IN        1
#define GIO_OUT       0

#define GIO_INVERTED  1
#define GIO_NORMAL    0

#define GIO_IRQ       1
#define GIO_NOIRQ     0

#define GIO_BANK0     0
#define GIO_BANK1     1


/* GIO lines */

#define AV3XX_GIO_ON_BTN    0x00
#define AV3XX_GIO_OFF_BTN   0x1C

#define AV3XX_GIO_SPDIF     0x01
#define AV3XX_GIO_VID_OUT   0x1D

#define AV3XX_GIO_DVR_IR    0x06

#define AV3XX_GIO_MAS_EOD   0x04
#define AV3XX_GIO_MAS_D0    0x08
#define AV3XX_GIO_MAS_D1    0x09
#define AV3XX_GIO_MAS_D2    0x0a
#define AV3XX_GIO_MAS_D3    0x0b
#define AV3XX_GIO_MAS_D4    0x0c
#define AV3XX_GIO_MAS_D5    0x0d
#define AV3XX_GIO_MAS_D6    0x0e
#define AV3XX_GIO_MAS_D7    0x0f
#define AV3XX_GIO_MAS_PWR   0x10
#define AV3XX_GIO_MAS_RTR   0x1E
#define AV3XX_GIO_MAS_PR    0x1F

#define AV3XX_GIO_I2C_CLOCK 0x12
#define AV3XX_GIO_I2C_DATA  0x13

#define AV3XX_GIO_LCD_BACKLIGHT 0x17

/* GIO */
#define AV3XX_GIO_BASE                          0x30580
#define AV3XX_GIO_DIRECTION0                    (AV3XX_GIO_BASE+0x00)  // GIO 0-15
#define AV3XX_GIO_DIRECTION1                    (AV3XX_GIO_BASE+0x02)  // GIO 16-31
#define AV3XX_GIO_INVERT0                       (AV3XX_GIO_BASE+0x04)  // GIO 0-15
#define AV3XX_GIO_INVERT1                       (AV3XX_GIO_BASE+0x06)  // GIO 16-31
#define AV3XX_GIO_BITSET0                       (AV3XX_GIO_BASE+0x08)  // GIO 0-15
#define AV3XX_GIO_BITSET1                       (AV3XX_GIO_BASE+0x0a)  // GIO 16-31
#define AV3XX_GIO_BITCLEAR0                     (AV3XX_GIO_BASE+0x0c)  // GIO 0-15
#define AV3XX_GIO_BITCLEAR1                     (AV3XX_GIO_BASE+0x0e)  // GIO 16-31
#define AV3XX_GIO_ENABLE_IRQ                    (AV3XX_GIO_BASE+0x10)  // GIO 0-7

/*functions */

void av3xx_gio_dir    (int gio_num,int direction);
void av3xx_gio_inv    (int gio_num,int direction);
void av3xx_gio_raw    (int data,int bank);
void av3xx_gio_set    (int gio_num);
void av3xx_gio_clear  (int gio_num);
void av3xx_gio_IRQ    (int gio_num,int set);
int  av3xx_gio_is_set (int gio_num);

#endif
