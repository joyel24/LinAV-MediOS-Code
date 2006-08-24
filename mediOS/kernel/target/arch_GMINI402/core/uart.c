/*
*   kernel/target/arch_AV3XX/uart.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/uart.h>
#include <kernel/clkc.h>
#include <kernel/io.h>
#include <kernel/gio.h>

extern unsigned int uart_addr[];

void arch_uart_need(int uart_num)
{
    if(uart_num==UART_0)
    {
        GIO_DIRECTION(GIO_VID_OUT,GIO_OUT);
        GIO_SET(GIO_VID_OUT);
    }
}

void arch_uart_init()
{
    int brsr;

    // set ref clock to 27Mhz (EXT_CLK)
    outw(inw(CLKC_SOURCE_SELECT)|CLKC_SRC_UART0_27M|CLKC_SRC_UART1_27M,CLKC_SOURCE_SELECT);

    // set bitrate do 115200bps
    brsr=((float)CONFIG_EXT_CLK/(16*115200))-1+0.5; // round to nearest integer
    outw(brsr,uart_addr[0]+UART_BRSR);
    outw(brsr,uart_addr[1]+UART_BRSR);
}
