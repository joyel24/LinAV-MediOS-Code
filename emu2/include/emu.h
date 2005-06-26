/* 
*   emu.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef EMU_H
#define EMU_H

#include "emu_types.h"

#define STEP 0
#define RUN  1

extern int run_mode;
extern int HW_mode;
extern int disp_mode;

#define CPU_DEBUG       (0x1)
#define NULL_HW_DEBUG   (0x1<< 1)
#define CLOCK_HW_DEBUG  (0x1<< 2)
#define UART_HW_DEBUG   (0x1<< 3)
#define RTC_HW_DEBUG    (0x1<< 4)
#define TSC_HW_DEBUG    (0x1<< 5)
#define I2C1_HW_DEBUG   (0x1<< 6)
#define I2C2_HW_DEBUG   (0x1<< 7)
#define TMR_HW_DEBUG    (0x1<< 8)
#define GPIO_HW_DEBUG   (0x1<< 9)
#define MEM_HW_DEBUG    (0x1<< 10)
#define MAS_HW_DEBUG    (0x1<< 11)
#define CPLD_HW_DEBUG   (0x1<< 12)

#define DEBUG_MODE
#define DEBUG_MODE_HW

#ifdef DEBUG_MODE
#define DEBUG(fmt, arg...) if(disp_mode==1 || run_mode==STEP) printf(fmt,## arg);
#define MKDEBUG sprintf
#else
#define DEBUG(...)
#define MKDEBUG(...)
#endif

#ifdef DEBUG_MODE_HW
#define DEBUG_HW(level,fmt, arg...) if(HW_mode&level || run_mode==STEP) printf(fmt,## arg);
#else
#define DEBUG_HW(...)
#endif

#endif
