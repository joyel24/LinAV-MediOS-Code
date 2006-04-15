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

char trap_stack[1024];

void do_trap_undef(void)
{
    printk("[INT] UNDEF !!\n");
}

void do_trap_swi(void)
{
    printk("[INT] SWI !!\n");
}

void do_trap_prefetch(void)
{
    printk("[INT] PREFETCH !!\n");
}

void do_trap_data(void)
{
    printk("[INT] DATA !!\n");
}

void do_trap_addrexcptn(void)
{
    printk("[INT] ADDREXCPTN => should not happen let's loop !!\n");
}

void print_wdt(void)
{
    printk("[INT] WDT HALTING !!\n");
}
