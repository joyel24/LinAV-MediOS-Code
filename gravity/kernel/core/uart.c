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
#include <kernel/kernel.h>

__IRAM_DATA unsigned int uartAdrr[2]={
    UART0_BASE,
    UART0_BASE
};

__IRAM_CODE int uartIn(unsigned char * data,int uartNum)  /* this one is probably buggy */
{
    if(inw(uartAdrr[uartNum]+UART_RFCR)&0x3F) /* check if something is in the reception buffer */
    {
        *data=(unsigned char)inw(uartAdrr[uartNum]+UART_DTRR);
        return 1;
    }
    else
        return 0;
}

__IRAM_CODE void uartOut(unsigned char data,int uartNum)
{
    while(!(inw(uartAdrr[uartNum]+UART_SR)&0x400)) /* Nothing */; /* using transmission buffer level */
    outw(data,uartAdrr[uartNum]+UART_DTRR);
}

__IRAM_CODE void uartOutString(unsigned char * data,int uartNum)
{
    while(*data)
    {
        uartOut(*data,uartNum);
        data++;
    }
}
