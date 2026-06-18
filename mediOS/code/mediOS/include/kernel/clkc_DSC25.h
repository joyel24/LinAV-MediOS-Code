/*
*   include/kernel/clkc_DSC25.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __CLKC_DSC25_H
#define __CLKC_DSC25_H

#define CLKC_BASE                      0x00030880

#define CLKC_PLL0                      (CLKC_BASE + 0x0000)
#define CLKC_PLL1                      (CLKC_BASE + 0x0002)
#define CLKC_PLL2                      (CLKC_BASE + 0x0004)
#define CLKC_PLL_DIVIDER               (CLKC_BASE + 0x0008)
#define CLKC_BYPASS                    (CLKC_BASE + 0x000e)
#define CLKC_POWER0                    (CLKC_BASE + 0x0014)
#define CLKC_POWER1                    (CLKC_BASE + 0x0016)

#define CLKC_PLL_M_SHIFT               4
#define CLKC_PLL_N_SHIFT               0
#define CLKC_PLL_M_MASK                0x01f0
#define CLKC_PLL_N_MASK                0x0003
#define CLKC_PLL_M_MAX                 20
#define CLKC_PLL_N_MAX                 4
#define CLKC_PLL_FREQ_MIN              25000000
#define CLKC_PLL_FREQ_MAX              175000000

#define CLKC_DIV_ARM_SHIFT             0
#define CLKC_DIV_DSP_SHIFT             4
#define CLKC_DIV_SDRAM_SHIFT           8
#define CLKC_DIV_ARM_MASK              0x0003
#define CLKC_DIV_DSP_MASK              0x0030
#define CLKC_DIV_SDRAM_MASK            0x0300

#define CLKC_BYP_ALL                   0x0007
#define CLKC_BYP_NONE                  0x0000

#endif
