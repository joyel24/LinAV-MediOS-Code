/*
*   kernel/core/trap_fct.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/kernel.h>
#include <kernel/hardware.h>
#include <kernel/uart.h>

char trap_stack[1024];

void do_trap_undef(void)
{
    uart_outString("[INT] UNDEF !!\n",DEBUG_UART);
}

void do_trap_swi(void)
{
    uart_outString("[INT] SWI !!\n",DEBUG_UART);
}

void do_trap_prefetch(void)
{
    uart_outString("[INT] PREFETCH !!\n",DEBUG_UART);
}

void do_trap_data(void)
{
    uart_outString("[INT] DATA !!\n",DEBUG_UART);
}

void do_trap_addrexcptn(void)
{
    uart_outString("[INT] ADDREXCPTN => should not happen let's loop !!\n",DEBUG_UART);
}

void print_wdt(void)
{
    uart_outString("[INT] WDT HALTING !!\n",DEBUG_UART);
}

void do_trap_fiq(void)
{
    uart_outString("[INT] FIQ !!\n",DEBUG_UART);
}
