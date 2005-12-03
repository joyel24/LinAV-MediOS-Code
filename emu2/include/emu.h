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

#ifdef AV3XX
#include <arch/arch_av3xx.h>
#else
#ifdef Gmini400
#include <arch/arch_gmini400.h>
#else
#ifdef Gmini400hb
#include <arch/arch_gmini400.h>
#else
    #error "undef arch"
#endif
#endif
#endif
#include "emu_types.h"

#define STEP 0
#define RUN  1

extern int run_mode;
extern int HW_mode;
extern int disp_mode;
extern uint32_t old_PC;
void void_cmdline(void);
extern void (*cmd_line_fct)(void);

#define CHG_RUN_MODE(MODE) {         \
    run_mode = MODE;                 \
    if(run_mode==RUN)                \
        cmd_line_fct=void_cmdline;   \
    else                             \
        cmd_line_fct=cmd_line;       \
}

bool chkIrqFlag();
bool chkFiqFlag();

void void_irq_fiq(void);
void cpu_do_irq(void);
void cpu_do_fiq(void);

extern void (*cur_irq_fct)(void);

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
#define HW_30A24_DEBUG  (0x1<< 13)
#define ATA_HW_DEBUG    (0x1<< 14)
#define DMA_HW_DEBUG    (0x1<< 15)
#define ECR_HW_DEBUG    (0x1<< 16)
#define OSD_HW_DEBUG    (0x1<< 17)
#define IRQ_HW_DEBUG    (0x1<< 18)
#define CPLDBTN_HW_DEBUG (0x1<< 19)
#define DVR_HW_DEBUG    (0x1<< 20)

#define DEBUG_MODE
#define DEBUG_MODE_HW

#ifdef DEBUG_MODE
#define DEBUG(fmt, arg...) if(disp_mode==1 || run_mode==STEP) printf(fmt,## arg) ;
#define MKDEBUG(dest,fmt, arg...) if(disp_mode==1 || run_mode==STEP) sprintf(dest,fmt,## arg);
#else
#define DEBUG(...)
#define MKDEBUG(...)
#endif

#ifdef DEBUG_MODE_HW
#define DEBUG_HW(level,fmt, arg...) if(HW_mode&level || run_mode==STEP) {printf("@%08x:",old_PC); printf(fmt,## arg); }
#else
#define DEBUG_HW(...)
#endif

#endif
