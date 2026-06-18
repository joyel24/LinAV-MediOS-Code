/*
*   kernel/target/arch_AV1XX/uart.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/gio.h>
#include <kernel/uart.h>
#include <kernel/io.h>

void arch_uart_need(int uart_num)
{
    if(uart_num==UART_1)
    {
        GIO_DIRECTION(GIO_SPDIF,GIO_OUT);
        GIO_DIRECTION(GIO_VID_OUT,GIO_OUT);
        GIO_SET(GIO_SPDIF);
        GIO_SET(GIO_VID_OUT);
    }
}

extern int uart_clockSpeed;

void arch_uart_init()
{
    uart_clockSpeed = CONFIG_ARM_CLK;
}
