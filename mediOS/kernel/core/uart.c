/* 
*   kernel/core/uart.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/irq.h>
#include <kernel/uart.h>
#include <kernel/kernel.h>
#include <kernel/pipes.h>

PIPE UART_PIPES[2];

__IRAM_DATA PIPE * UART_0_Pipe;
__IRAM_DATA PIPE * UART_1_Pipe;

__IRAM_DATA unsigned int uartAdrr[2]={
    UART0_BASE,
    UART1_BASE
};

__IRAM_CODE void uart_intr_action(int irq)
{
    char c;
    int uart = irq - IRQ_UART0;

//    if(inw(uartAdrr[uart]+UART_SR)&0x0004)
    while(inw(uartAdrr[uart]+UART_SR)&0x0004)
    {
        c=(unsigned char)(inw(uartAdrr[uart]+UART_DTRR)&0xFF);
        kpipe_write (&UART_PIPES[uart], &c, 1);
    }
}

__IRAM_CODE int uartIn(unsigned char * data,int uartNum) 
{
    if(inw(uartAdrr[uartNum]+UART_SR)&0x0004) /* check if something is in the reception buffer */
    {
        *data=(unsigned char)(inw(uartAdrr[uartNum]+UART_DTRR)&0xFF);
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

void init_uart(void)
{
        
    UART_0_Pipe=&UART_PIPES[0];
    UART_1_Pipe=&UART_PIPES[1];
    
    UART_0_Pipe->nReceiver = 0;
    UART_0_Pipe->nSender   = 0;
    UART_1_Pipe->nReceiver = 0;
    UART_1_Pipe->nSender   = 0;
    
    enable_irq(IRQ_UART0);
    enable_irq(IRQ_UART1);

    printk("[init] uart\n");
}
