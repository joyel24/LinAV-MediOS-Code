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

#include <kernel/malloc.h>

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/kernel.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/hw_chk.h>

#include <kernel/uart.h>
#include <kernel/cpld.h>

#include <kernel/version.h>

#include <kernel/rtc.h>
#include <kernel/usb_fw.h>
#include <kernel/bat_power.h>
#include <kernel/buttons.h>


#include <kernel/threads.h>
#include <kernel/pipes.h>
#include <kernel/kgraphics.h>

#include <api.h>
#include <kernel/memmgr.h>

#include <kernel/config.h>

void print_boot_info(void)
{
    printk("SP: %08x\n",get_sp());
    print_irq();
    print_timer();
    /* int vector */
    print_data(0x0,0x20);
}

extern void kernel_startup_thread (void);
extern void ini_debugOnScreen(void);

extern void avwm(void);

extern int kmemory_manager (void* pvParameters);

extern void API_BKT(void);


void kernel_start (void)
{
    ini_graphics();
    ini_debugOnScreen();
   
    /* malloc of max space in SDRAM */
    init_malloc((void*)MALLOC_START,MALLOC_SIZE);
    
    /* print banner on uart */ 
    printk("GRAVITY %d.%d - kernel loading\n",VER_MAJOR,VER_MINOR);
    printk("Initial SP: %08x, kernel end: %08x, size in IRAM: %08x  Malloc start: %08x, size: %08x\n",get_sp(),
        (unsigned int)&_end_kernel,
        (unsigned int)&_iram_end - (unsigned int)&_iram_start,
        (unsigned int)MALLOC_START,
        (unsigned int)MALLOC_SIZE);

           
    /* initialize thread lists and setup first two threads */
    kinit_tcb ();

///////////////////////////////////////////////////
/// TODO: This code gows to kinit_tcb ()...
   g_pKernelCtrlPipe = kmalloc (sizeof(PIPE));
   g_pKernelCtrlPipe->nReceiver = 0;
   g_pKernelCtrlPipe->nSender = 0;

   g_pSystemCtrlPipe = kmalloc (sizeof(PIPE));
   g_pSystemCtrlPipe->nReceiver = 0;
   g_pSystemCtrlPipe->nSender = 0;

    kadd_tcb (&g_pTaskRing, kcreate_tcb (kernel_startup_thread, TASK_STACK_SIZE, 0, "USER"));
    kadd_tcb (&g_pTaskRing, kcreate_tcb (kmemory_manager, TASK_STACK_SIZE,   0, "SYSTEM"));
///////////////////////////////////////////////////

    /* init the irq */
    init_irq(); 

    /* init the tick timer */
    init_timer();

    /* enable the IRQ */
    __sti(); 
    
    /* switch to first task in list */
    kload_context();
}

void kernel_startup_thread (void)
{
    int i;
#if 0
    while (1)
    {
	unsigned long nBytes = 0;
	API_MEMAVAIL (&nBytes);

        cli(); // for safe multithreaded printing
        printk("[ FREE MEMORY: %08x bytes ]\n", nBytes);
        sti();

        for(i=0;i<0x10000;i++) /*nothing*/;
    };
#endif
    /* driver init */
    init_cpld();
    init_HW_chk();

    init_buttons();
    init_rtc();
    init_usb_fw();
    
    init_power();

    printk("[init] ------------ all drivers\n");

    print_boot_info();
    
    avwm();       
}
