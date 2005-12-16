/*
*   lib/uart.c
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <io.h>
#include <hardware.h>
#include <uart.h>
#include <kernel.h>

unsigned int uartAdrr[2]={
    UART0_BASE,
    UART1_BASE
};

void uartOut(unsigned char data,int uartNum)
{
    while(!(inw(uartAdrr[uartNum]+UART_SR)&0x400)) /* Nothing */; /* using transmission buffer level */
    outw(data,uartAdrr[uartNum]+UART_DTRR);
}

void uartOutString(unsigned char * data,int uartNum)
{
    while(*data)
    {
        if (*data=='\n') uartOut('\r',uartNum); //gligli: uart fix
        uartOut(*data,uartNum);
        data++;
    }
}
