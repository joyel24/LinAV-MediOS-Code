/*
 * linux/include/asm-armnommu/arch-av3xx/cpld.h
 *
 * Copyright (c) 1998 Hugo Fiennes & Nicolas Pitre
 *
 * 18-aug-2000: Cleanup by Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *              Get rid of the special ide_init_hwif_ports() functions
 *              and make a generalised function that can be used by all
 *              architectures.
 */

#ifndef __ASM_ARCH_CPLD_H
#define __ASM_ARCH_CPLD_H

#define AV3XX_CPLD0                  0x0
#define AV3XX_CPLD1                  0x1
#define AV3XX_CPLD2                  0x2
#define AV3XX_CPLD3                  0x3

/* PORT 0 */
/*** HD / CF */
#define AV3XX_CPLD_HD_CF             0x0
#define AV3XX_CPLD_SEL_HD            0x0
#define AV3XX_CPLD_SEL_CF            0x1

#define AV3XX_CPLD_IR                0x2

/* PORT 1*/
#define AV3XX_CPLD_USB               0x0

/* PORT 2*/
#define AV3XX_CPLD2_IR               0x1
#define AV3XX_CPLD_LCD               0x2

/* PORT 3*/
#define AV3XX_CPLD_FM                0x0
#define AV3XX_CPLD3_IR               0x1
#define AV3XX_CPLD_HD_POWER          0x3

#define AV3XX_CPLD_BASE              0x02600000
#define AV3XX_CPLD_PORT0             (AV3XX_CPLD_BASE+0x000)
#define AV3XX_CPLD_PORT1             (AV3XX_CPLD_BASE+0x100)
#define AV3XX_CPLD_PORT2             (AV3XX_CPLD_BASE+0x200)
#define AV3XX_CPLD_PORT3             (AV3XX_CPLD_BASE+0x300)

void cpld_chg_state   (int cpld_port,int bit_num,int direction);
void cpld_select      (int bit_num,int direction);
int  cpld_read        (int cpld_port);

#define cpld_set_port_0(bit_num)     cpld_select(bit_num,1)
#define cpld_clear_port_0(bit_num)   cpld_select(bit_num,0)
#define cpld_set_port_1(bit_num)     cpld_chg_state(AV3XX_CPLD1,bit_num,1)
#define cpld_clear_port_1(bit_num)   cpld_chg_state(AV3XX_CPLD1,bit_num,0)
#define cpld_set_port_2(bit_num)     cpld_chg_state(AV3XX_CPLD2,bit_num,1)
#define cpld_clear_port_2(bit_num)   cpld_chg_state(AV3XX_CPLD2,bit_num,0)
#define cpld_set_port_3(bit_num)     cpld_chg_state(AV3XX_CPLD3,bit_num,1)
#define cpld_clear_port_3(bit_num)   cpld_chg_state(AV3XX_CPLD3,bit_num,0)

void cpld_do_select(void);

#endif
