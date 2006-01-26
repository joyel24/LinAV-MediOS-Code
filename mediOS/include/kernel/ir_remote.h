/* 
*   include/kernel/ir_remote.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __IR_REMOTE_H
#define __IR_REMOTE_H


void ir_remote_interrupt(int irq,struct pt_regs * regs);
void ir_remote_tmr_interrupt(int irq,struct pt_regs * regs);
void restartTimer(void);

int  ir_set(void);

void processCode(int code);

void init_ir_remote(void);
void start_ir_remote(void);
void stop_ir_remote(void);



#endif
