/*
*   include/kernel/clkc_DM270.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __CLKC_DM270_H
#define __CLKC_DM270_H

#define CLKC_BASE                      0x00030880

#define CLKC_PLL0                      (CLKC_BASE + 0x0000)
#define CLKC_PLL1                      (CLKC_BASE + 0x0002)
#define CLKC_SOURCE_SELECT             (CLKC_BASE + 0x0004)
#define CLKC_PLL_SELECT                (CLKC_BASE + 0x0006)
#define CLKC_PLL_DIVIDER               (CLKC_BASE + 0x0008)
#define CLKC_BYPASS                    (CLKC_BASE + 0x000a)
#define CLKC_POWER0                    (CLKC_BASE + 0x0010)
#define CLKC_POWER1                    (CLKC_BASE + 0x0012)
#define CLKC_POWER2                    (CLKC_BASE + 0x0014)
#define CLKC_PWM0_PERIOD               (CLKC_BASE + 0x0022)
#define CLKC_PWM0_HIGH                 (CLKC_BASE + 0x0024)
#define CLKC_PWM1_PERIOD               (CLKC_BASE + 0x0026)
#define CLKC_PWM1_HIGH                 (CLKC_BASE + 0x0028)

#define CLKC_PLL_M_SHIFT               4
#define CLKC_PLL_N_SHIFT               0
#define CLKC_PLL_M_MASK                0x00f0
#define CLKC_PLL_N_MASK                0x0007
#define CLKC_PLL_M_MAX                 16
#define CLKC_PLL_N_MAX                 8
#define CLKC_PLL_FREQ_MIN              110000000
#define CLKC_PLL_FREQ_MAX              351000000

#define CLKC_SRC_PLL_MXI               0x0000
#define CLKC_SRC_PLL_27M               0x0001
#define CLKC_SRC_VID0_MXI              0x0000
#define CLKC_SRC_VID0_27M              0x0002
#define CLKC_SRC_VID1_VID0             0x0000
#define CLKC_SRC_VID1_P                0x0004
#define CLKC_SRC_CCD_P                 0x0000
#define CLKC_SRC_CCD_27M               0x0008
#define CLKC_SRC_OSD_VIDHALF           0x0000
#define CLKC_SRC_OSD_PCLK              0x0010
#define CLKC_SRC_TMR0_ARM              0x0000
#define CLKC_SRC_TMR0_27M              0x0020
#define CLKC_SRC_TMR1_ARM              0x0000
#define CLKC_SRC_TMR1_27M              0x0040
#define CLKC_SRC_TMR2_ARM              0x0000
#define CLKC_SRC_TMR2_27M              0x0080
#define CLKC_SRC_TMR3_ARM              0x0000
#define CLKC_SRC_TMR3_27M              0x0100
#define CLKC_SRC_UART0_ARM             0x0000
#define CLKC_SRC_UART0_27M             0x0200
#define CLKC_SRC_UART1_ARM             0x0000
#define CLKC_SRC_UART1_27M             0x0400
#define CLKC_SRC_MS_PLL0               0x0000
#define CLKC_SRC_MS_PLL1               0x0800
#define CLKC_SRC_VID_NORMAL            0x0000
#define CLKC_SRC_VID_INVERT            0x1000
#define CLKC_SRC_CCD_NORMAL            0x0000
#define CLKC_SRC_CCD_INVERT            0x2000
#define CLKC_SRC_MMC_NORMAL            0x0000
#define CLKC_SRC_MMC_INVERT            0x4000

#define CLKC_SEL_ARM_PLL0              0x0000
#define CLKC_SEL_ARM_PLL1              0x0001
#define CLKC_SEL_DSP_PLL0              0x0000
#define CLKC_SEL_DSP_PLL1              0x0010
#define CLKC_SEL_SDRAM_PLL0            0x0000
#define CLKC_SEL_SDRAM_PLL1            0x0100
#define CLKC_SEL_ACCEL_PLL0            0x0000
#define CLKC_SEL_ACCEL_PLL1            0x1000

#define CLKC_DIV_ARM_SHIFT             0
#define CLKC_DIV_DSP_SHIFT             4
#define CLKC_DIV_SDRAM_SHIFT           8
#define CLKC_DIV_ACCEL_SHIFT           12
#define CLKC_DIV_ARM_MASK              0x0003
#define CLKC_DIV_DSP_MASK              0x0030
#define CLKC_DIV_SDRAM_MASK            0x0300
#define CLKC_DIV_ACCEL_MASK            0x3000

#define CLKC_BYP_ALL                   0x1111
#define CLKC_BYP_NONE                  0x0000

#endif
