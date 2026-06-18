/*
* asm/arch/av3xx_ir.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/
#ifndef __ASM_ARCH_AV3XX_IR_H
#define __ASM_ARCH_AV3XX_IR_H


void restartTimer(void);

int  av3xx_ir_set(void);

void processCode(int code);

void av3xx_ir_dvr_disconnected(void);
void av3xx_ir_dvr_connected(void);

#endif
