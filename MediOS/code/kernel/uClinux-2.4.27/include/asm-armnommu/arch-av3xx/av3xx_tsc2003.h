/*
* asm/arch/av3xx_tsc2003.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*
* This chip is used for battery measure, but can also do temp, and touch screen measure
*
*/
#ifndef __ASM_ARCH_AV3XX_TSC2003_H
#define __ASM_ARCH_AV3XX_TSC2003_H

#define CMD_TEMP0                0x00
#define CMD_BAT0                 0x10
#define CMD_IN0                  0x20

#define CMD_TEMP1                0x40
#define CMD_BAT1                 0x50
#define CMD_IN1                  0x60

#define INTERNAL_ON              0x08
#define ADC_ON                   0x04


int av3xx_tsc2003getVal(char cmdN);

#endif
