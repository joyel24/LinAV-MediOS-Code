/* 
*   kernel/trap_fct.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/kernel.h>

__attribute__ ((section(".core"))) void do_trap_undef(void)
{
    printk("[INT] UNDEF !!\n");
}

__attribute__ ((section(".core"))) void do_trap_swi(void)
{
    printk("[INT] SWI !!\n");
}

__attribute__ ((section(".core"))) void do_trap_prefetch(void)
{
    printk("[INT] PREFETCH !!\n");
}

__attribute__ ((section(".core"))) void do_trap_data(void)
{
    printk("[INT] DATA !!\n");
}

__attribute__ ((section(".core"))) void do_trap_addrexcptn(void)
{
    printk("[INT] ADDREXCPTN => should not happen let's loop !!\n");
}


__attribute__ ((section(".core"))) void print_wdt(void)
{
    printk("[INT] WDT HALTING !!\n");
}
