/* 
*   include/uart.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/


#ifndef __UART_H
#define __UART_H

#define UART_DTRR     0x00000000
#define UART_BRSR     0x00000002
#define UART_MSR      0x00000004
#define UART_RFCR     0x00000006
#define UART_TFCR     0x00000008
#define UART_LCR      0x0000000a
#define UART_SR       0x0000000c

int  uartIn         (unsigned char * data,int uartNum);
void uartOut        (unsigned char data,int uartNum);
void uartOutString  (unsigned char * data,int uartNum);

#endif
