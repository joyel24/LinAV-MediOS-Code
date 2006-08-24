/*
*   include/kernel/clkc_DM320.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __CLKC_DM320_H
#define __CLKC_DM320_H

#define CLKC_BASE                      0x00030880

#define CLKC_PLL0                      (CLKC_BASE + 0x0000)
#define CLKC_PLL1                      (CLKC_BASE + 0x0002)
#define CLKC_SOURCE_SELECT             (CLKC_BASE + 0x0004)
#define CLKC_PLL_SELECT                (CLKC_BASE + 0x0008)
#define CLKC_PLL_DIVIDER0              (CLKC_BASE + 0x000a)
#define CLKC_PLL_DIVIDER1              (CLKC_BASE + 0x000c)
#define CLKC_PLL_DIVIDER2              (CLKC_BASE + 0x000e)
#define CLKC_PWM0_PERIOD               (CLKC_BASE + 0x002a)
#define CLKC_PWM0_HIGH                 (CLKC_BASE + 0x002c)
#define CLKC_PWM1_PERIOD               (CLKC_BASE + 0x002e)
#define CLKC_PWM1_HIGH                 (CLKC_BASE + 0x0030)

#define CLKC_PLL_M_SHIFT               4
#define CLKC_PLL_N_SHIFT               0
#define CLKC_PLL_M_MASK                0x00f0
#define CLKC_PLL_N_MASK                0x0007
#define CLKC_PLL_M_MAX                 16
#define CLKC_PLL_N_MAX                 8
#define CLKC_PLL_FREQ_MIN              110000000
#define CLKC_PLL_FREQ_MAX              351000000

#define CLKC_SRC_TMR0_ARM              0x0000
#define CLKC_SRC_TMR0_27M              0x0002
#define CLKC_SRC_TMR1_ARM              0x0000
#define CLKC_SRC_TMR1_27M              0x0004
#define CLKC_SRC_TMR2_ARM              0x0000
#define CLKC_SRC_TMR2_27M              0x0008
#define CLKC_SRC_TMR3_ARM              0x0000
#define CLKC_SRC_TMR3_27M              0x0010
#define CLKC_SRC_UART0_ARM             0x0000
#define CLKC_SRC_UART0_27M             0x0020
#define CLKC_SRC_UART1_ARM             0x0000
#define CLKC_SRC_UART1_27M             0x0040

#define CLKC_SEL_ARM_PLL0              0x0000
#define CLKC_SEL_ARM_PLL1              0x0001
#define CLKC_SEL_DSP_PLL0              0x0000
#define CLKC_SEL_DSP_PLL1              0x0010
#define CLKC_SEL_SDRAM_PLL0            0x0000
#define CLKC_SEL_SDRAM_PLL1            0x0100
#define CLKC_SEL_ACCEL_PLL0            0x0000
#define CLKC_SEL_ACCEL_PLL1            0x1000

#define CLKC_DIV_ARM_ADDRESS           CLKC_PLL_DIVIDER0
#define CLKC_DIV_DSP_ADDRESS           CLKC_PLL_DIVIDER2
#define CLKC_DIV_SDRAM_ADDRESS         CLKC_PLL_DIVIDER1
#define CLKC_DIV_ACCEL_ADDRESS         CLKC_PLL_DIVIDER1
#define CLKC_DIV_ARM_SHIFT             0
#define CLKC_DIV_DSP_SHIFT             8
#define CLKC_DIV_SDRAM_SHIFT           0
#define CLKC_DIV_ACCEL_SHIFT           8
#define CLKC_DIV_ARM_MASK              0x001f
#define CLKC_DIV_DSP_MASK              0x1f00
#define CLKC_DIV_SDRAM_MASK            0x001f
#define CLKC_DIV_ACCEL_MASK            0x1f00

#define CLKC_BYP_ALL                   0x1111
#define CLKC_BYP_NONE                  0x0000

#endif
