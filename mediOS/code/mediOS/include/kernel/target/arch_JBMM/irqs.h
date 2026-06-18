/* 
*   include/kernel/target/arch_AV3XX/irqs.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __IRQS_ARCH_H
#define __IRQS_ARCH_H

#define NR_IRQS              32

#define IRQ_IDE              15  // ide

#define IRQ_BTN_ON           18  // GIO0

#define IRQ_MAS_DATA         22  // GIO4
#define IRQ_IR               24  // GIO6

#define IRQ_DSP              10

#define IRQ_OSD              7

#define IRQ_UART0            11 
#define IRQ_UART1            12

/*
#define DSC21_IRQ_TIMER0  0x10
#define DSC21_IRQ_TIMER1  0x11
#define DSC21_IRQ_TIMER2  0x12
#define DSC21_IRQ_TIMER3  0x13
#define DSC21_IRQ_CCDVD0  0x14
#define DSC21_IRQ_CCDVD1  0x15
#define DSC21_IRQ_CCDVD2  0x16
#define DSC21_IRQ_ENCVD   0x17
#define DSC21_IRQ_SERIAL0 0x18
#define DSC21_IRQ_SERIAL1 0x19
#define DSC21_IRQ_DSP     0x1a
#define DSC21_IRQ_UART0   0x1b
#define DSC21_IRQ_UART1   0x1c
#define DSC21_IRQ_USB     0x1d
#define DSC21_IRQ_BURST   0x1e
#define DSC21_IRQ_CARD    0x1f
*/
#endif
