/* 
*   kernel/main.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/malloc.h>

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/kernel.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/hw_chk.h>

#include <kernel/uart.h>
#include <kernel/cpld.h>

#include <kernel/version.h>

#include <kernel/threads.h>
#include <kernel/graphics.h>


int lcd_state=1;
int lcd_bright=10;

void print_boot_info(void)
{
    printk("SP: %08x\n",get_sp());
    print_irq();
    print_timer();
}

void kidle (void)
{
    while (1);
}

void kernel_startup_thread (void);

void kernel_start (void)
{
    /* malloc of max space in SDRAM */
    init_malloc((void*)MALLOC_START,MALLOC_SIZE);

    /* initialize thread lists and setup first two threads */
    kinit_tcb ();
    kadd_tcb (&g_pActiveTask, kcreate_tcb (kidle, TASK_STACK_SIZE,   0, "IDLE"));
    kadd_tcb (&g_pActiveTask, kcreate_tcb (kernel_startup_thread, TASK_STACK_SIZE, 0, "USER"));

//    asm ("MSR CPSR_c, #0x1F");
//    kStartScheduler ();

    ini_graphics();
    ini_debugOnScreen();

    /* init the irq */
    init_irq(); 

    /* init the tick timer */
    init_timer();

//	interruptsSetMaskA (0xffffff7f);
//	gioSetAllIRQsA (0xff);    // Set all gio (0-7) to be IRQ!
//    timersConfigA (0, TIMERS_TMMD_FREERUN, 0, 999,  864); //  16 millisecond interval

    kload_context ();
}

void kernel_startup_thread (void)
{
    int i;

    /* print banner on uart */ 
    printk("AMOS %d.%d - kernel loading\n",VER_MAJOR,VER_MINOR);    
    printk("Initial SP: %08x, kernel end: %08x, size in IRAM: %08x  Malloc start: %08x, size: %08x\n",get_sp(),
        (unsigned int)&_end_kernel,
        (unsigned int)&_iram_end - (unsigned int)&_iram_start,
        (unsigned int)MALLOC_START,
        (unsigned int)MALLOC_SIZE);

    /* enable the IRQ */
//    sti();       
//    printk("[init] int. IRQ enable\n"); 

    /* driver init */
//    init_cpld();
//    init_HW_chk();

//    init_buttons();

    printk("[init] ------------ all drivers\n");

//    print_boot_info();    


    while(1)
    {
        printk("C ");
        for(i=0;i<0x10000;i++) /*nothing*/;
    }
    /* exit not allowed */
}
