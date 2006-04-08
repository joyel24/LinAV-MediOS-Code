/*
*   kernel/core/uart.c
*
*   MediOS project
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
#include <kernel/gio.h>

struct pipe UART_PIPES[2];

__IRAM_DATA struct pipe * UART_0_Pipe;
__IRAM_DATA struct pipe * UART_1_Pipe;

__IRAM_DATA unsigned int uart_addr[2]={
    UART0_BASE,
    UART1_BASE
};

__IRAM_CODE void uart_intAction(int irq,struct pt_regs * regs)
{
    char c;
    int uart = irq - IRQ_UART0;

//    if(inw(uart_addr[uart]+UART_SR)&0x0004)
    while(inw(uart_addr[uart]+UART_SR)&0x0004)
    {
        c=(unsigned char)(inw(uart_addr[uart]+UART_DTRR)&0xFF);
        pipeWrite (&UART_PIPES[uart], &c, 1);
    }
}

int uart_in(unsigned char * data,int uartNum)
{
    if(inw(uart_addr[uartNum]+UART_SR)&0x0004) /* check if something is in the reception buffer */
    {
        *data=(unsigned char)(inw(uart_addr[uartNum]+UART_DTRR)&0xFF);
        return 1;
    }
    else
        return 0;
}

void uart_out(unsigned char data,int uartNum)
{
    while(!(inw(uart_addr[uartNum]+UART_SR)&0x400)) /* Nothing */; /* using transmission buffer level */
    outw(data,uart_addr[uartNum]+UART_DTRR);
}

void uart_outString(unsigned char * data,int uartNum)
{
    while(*data)
    {
        if (*data=='\n') uart_out('\r',uartNum); //gligli: uart fix
        uart_out(*data,uartNum);
        data++;
    }
}

void uart_restoreIrqHandler(int uartNum)
{
    irq_changeHandler(uartNum == 0?IRQ_UART0:IRQ_UART1,uart_intAction);
}

void uart_init(void)
{

    GIO_DIRECTION(GIO_VID_OUT,GIO_IN); // switch video out to uart in

    UART_0_Pipe=&UART_PIPES[0];
    UART_1_Pipe=&UART_PIPES[1];

    UART_0_Pipe->nOUT = UART_0_Pipe->nIN = 0;
    UART_1_Pipe->nOUT = UART_1_Pipe->nIN = 0;

    irq_enable(IRQ_UART0);
    irq_enable(IRQ_UART1);

    printk("[init] uart\n");
}
