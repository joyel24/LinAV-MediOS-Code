//
// shatbox (C) 2002 Matthew Conte (matt@conte.com)
//
//
// regs.h
//
// nes register equates
//
// $Id$
//

#ifndef _REGS_H_
#define _REGS_H_

/*
** register equates
*/

/* $20xx register equates */
#define  REG_CTRL0            0x00
#define  REG_CTRL1            0x01
#define  REG_STAT             0x02
#define  REG_OAMADDR          0x03
#define  REG_OAMDATA          0x04
#define  REG_SCROLL           0x05
#define  REG_VADDR            0x06
#define  REG_VDATA            0x07

/* $40xx io register equates */
#define  REG_OAMDMA           0x14
#define  REG_IO0              0x16
#define  REG_IO1              0x17

/* $40xx apu register equates */
#define  REG_A0               0x00
#define  REG_A1               0x01
#define  REG_A2               0x02
#define  REG_A3               0x03
#define  REG_B0               0x04
#define  REG_B1               0x05
#define  REG_B2               0x06
#define  REG_B3               0x07
#define  REG_C0               0x08
/* 0x09 unused */
#define  REG_C2               0x0a
#define  REG_C3               0x0b
#define  REG_D0               0x0c
/* 0x0d unused */
#define  REG_D2               0x0e
#define  REG_D3               0x0f
#define  REG_E0               0x10
#define  REG_E1               0x11
#define  REG_E2               0x12
#define  REG_E3               0x13

#define  REG_ST               0x15

/*
** per-register flags and structures
*/

#define  REGF_CTRL0_NMI       0x80
#define  REGF_CTRL0_SLAVE     0x40
#define  REGF_CTRL0_OBJ16     0x20
#define  REGF_CTRL0_BGADDR    0x10
#define  REGF_CTRL0_OBJADDR   0x08
#define  REGF_CTRL0_ADDRINC   0x04
#define  REGF_CTRL0_NAMETAB   0x03

#define  REGF_CTRL1_COL_RED   0x80
#define  REGF_CTRL1_COL_BLUE  0x40
#define  REGF_CTRL1_COL_GREEN 0x20
#define  REGF_CTRL1_OBJON     0x10
#define  REGF_CTRL1_BGON      0x08
#define  REGF_CTRL1_OBJNOCLIP 0x04
#define  REGF_CTRL1_BGNOCLIP  0x02

#define  REGF_STAT_VBLINT     0x80
#define  REGF_STAT_STRIKE     0x40
#define  REGF_STAT_MAXSPRITE  0x20

/* 
** sound registers
*/

#define  REGF_A0_DUTYSHIFT 6
#define  REGF_A0_LOOPED    0x20
#define  REGF_A0_ENVFIX    0x10
#define  REGF_A0_ENVMASK   0x0f

#define  REGF_A1_SENABLE   0x80
#define  REGF_A1_SLEN      0x70
#define  REGF_A1_SDEC      0x08
#define  REGF_A1_SSHIFT    0x07

#define  REGF_C0_LOOPED    0x80
#define  REGF_C0_LENMASK   0x7f

#define  REGF_D0_LOOPED    0x20
#define  REGF_D0_ENVFIX    0x10
#define  REGF_D0_ENVMASK   0x0f

#define  REGF_D2_MODE      0x80
#define  REGF_D2_FREQMASK  0x0f

#define  REGF_D3_LENRSHIFT 3

#define  REGF_E0_IRQGEN    0x80
#define  REGF_E0_LOOPED    0x40
#define  REGF_E0_FREQMASK  0x0f

#define  REGF_ST_ENABLEA   0x01
#define  REGF_ST_ENABLEB   0x02
#define  REGF_ST_ENABLEC   0x04
#define  REGF_ST_ENABLED   0x08
#define  REGF_ST_ENABLEE   0x10

#define  REGF_ST_DMCIRQ    0x80

#endif /* !_REGS_H_ */

