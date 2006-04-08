/* 
*   include/kernel/uart.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __UART_H
#define __UART_H


#include <kernel/irq.h>

#define UART_0        0
#define UART_1        1

#define UART_DTRR     0x00000000
#define UART_BRSR     0x00000002
#define UART_MSR      0x00000004
#define UART_RFCR     0x00000006
#define UART_TFCR     0x00000008
#define UART_LCR      0x0000000a
#define UART_SR       0x0000000c

void uart_intAction(int irq,struct pt_regs * regs);

int  uart_in         (unsigned char * data,int uartNum);
void uart_out        (unsigned char data,int uartNum);
void uart_outString  (unsigned char * data,int uartNum);

void uart_init      (void);

#endif
