/* 
*   kernel/uart.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/uart.h>

unsigned int uartAdrr[2]={
    UART0_BASE,
    UART0_BASE
};

int uartIn(unsigned char * data,int uartNum)  /* this one is probably buggy */
{
    if(inw(uartAdrr[uartNum]+UART_RFCR)&0x3F) /* check if something is in the reception buffer */
    {
        *data=(unsigned char)inw(uartAdrr[uartNum]+UART_DTRR);
        return 1;
    }
    else
        return 0;
}

void uartOut(unsigned char data,int uartNum)
{
    while(!(inw(uartAdrr[uartNum]+UART_SR)&0x400)) /* Nothing */; /* using transmission buffer level */
    outw(data,uartAdrr[uartNum]+UART_DTRR);
}

void uartOutString(unsigned char * data,int uartNum)
{
    while(*data)
    {
        uartOut(*data,uartNum);
        data++;
    }
}
