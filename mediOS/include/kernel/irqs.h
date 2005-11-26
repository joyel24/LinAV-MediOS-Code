/* 
*   include/kernel/irqs.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __IRQS_H
#define __IRQS_H

#define IRQ_TMR_0            0
#define IRQ_TMR_1            1
#define IRQ_TMR_2            2
#define IRQ_TMR_3            3

#define IRQ_OSD              7 // interrupt really fast, related to OSD

#define IRQ_DSP              11

#define IRQ_UART0            12 
#define IRQ_UART1            13

#include <kernel/target/arch/irqs.h>

#endif
