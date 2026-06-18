/* 
*   include/sys_def/timer.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*
*/

#ifndef __SYS_DEF_TIMER_H
#define __SYS_DEF_TIMER_H

#define TMR0_BASE                      0x00030000
#define TMR1_BASE                      0x00030080
#define TMR2_BASE                      0x00030100
#define TMR3_BASE                      0x00030180

#define TMR_MODE                       0x00000000
#define TMR_SEL                        0x00000002
#define TMR_SCAL                       0x00000004
#define TMR_DIV                        0x00000006
#define TMR_TRG                        0x00000008
#define TMR_CNT                        0x0000000a

#define TMR_MODE_STOP                  0x0000
#define TMR_MODE_ONESHOT               0x0001
#define TMR_MODE_FREERUN               0x0002
#define TMR_MODE_RES                   0x0003

#define TMR_SEL_ARM                    0x0000
#define TMR_SEL_EXT                    0x0001

#define TMR_BASE(TMR_NUM)              (TMR0_BASE+0x80*TMR_NUM)

#define TMR_SET_REGS(VAL,REG,TMR_NUM)  {outw(VAL,TMR_BASE(TMR_NUM)+REG);}
#define TMR_GET_REGS(REG,TMR_NUM)      (inw(TMR_BASE(TMR_NUM)+REG))

#define TMR0 0
#define TMR1 1
#define TMR2 2
#define TMR3 3

#define TMR_SET_MODE(MODE,TMR_NUM)     {TMR_SET_REGS(MODE&0x3,TMR_MODE,TMR_NUM)}
#define TMR_GET_MODE(TMR_NUM)          (TMR_GET_REGS(TMR_MODE,TMR_NUM)&0x3)

#define TMR_SET_SEL(MODE,TMR_NUM)      {TMR_SET_REGS(MODE&0x1,TMR_SEL,TMR_NUM)}
#define TMR_GET_SEL(TMR_NUM)           (TMR_GET_REGS(TMR_SEL,TMR_NUM)&0x1)

#define TMR_SET_SCAL(SCAL_VAL,TMR_NUM) {TMR_SET_REGS(SCAL_VAL&0x3FF,TMR_SCAL,TMR_NUM)}
#define TMR_GET_SCAL(TMR_NUM)          (TMR_GET_REGS(TMR_SCAL,TMR_NUM)&0x3FF)

#define TMR_SET_DIV(DIV_VAL,TMR_NUM)   {TMR_SET_REGS(DIV_VAL&0xFFFF,TMR_DIV,TMR_NUM)}
#define TMR_GET_DIV(TMR_NUM)           (TMR_GET_REGS(TMR_DIV,TMR_NUM)&0xFFFF)

#define TMR_TRIGGER(TMR_NUM)           {TMR_SET_REGS(0x1,TMR_TRG,TMR_NUM)}

#define TMR_GET_CNT(TMR_NUM)           (TMR_GET_REGS(TMR_CNT,TMR_NUM)&0xFFFF)

#endif
