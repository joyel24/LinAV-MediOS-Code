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
#include <kernel/disk.h>
#include <kernel/fm_remote.h>
#include <kernel/ext_module.h>
#include <kernel/sound.h>
#include <kernel/cmd_line.h>
#include <kernel/evt.h>


#include <kernel/threads.h>
#include <kernel/pipes.h>
#include <kernel/kgraphics.h>

#include <api.h>
#include <kernel/memmgr.h>

#include <kernel/config.h>

#include <sys_def/file.h>

void print_boot_info(void)
{
    printk("SP: %08x\n",get_sp());
    print_irq();
    print_timer();
    print_HW_chk();
}

extern void kernel_startup_thread (void);
extern void ini_debugOnScreen(void);

extern void avwm(void);

extern int kmemory_manager (void* pvParameters);

extern void API_BKT(void);

#if 0
void debug_thread (void)
{
	register long _r7 asm("r7");
	register long _r8 asm("r8");
	register long _r12 asm("r12");
	register long _r13 asm("r13");
	asm volatile ( "mrs r7, cpsr");
	asm volatile ( "mov r8, lr");
	__cli ();
	printk("*** debug_thread *** [CPSR:%08x, R12:%08x, SP:%08x, LR:%08x]\n", _r7, _r12, _r13, _r8);
	__sti ();

    while(1);
}
#endif

void kload_context();

void kernel_start (void)
{
    ini_graphics();
#ifdef USE_DEBUG_ON_SCREEN    
    ini_debugOnScreen();
#endif
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
    kadd_tcb (&g_pTaskRing, kcreate_tcb (kmemory_manager, TASK_STACK_SIZE,   0, "MEMMGR"));
    kadd_tcb (&g_pTaskRing, kcreate_tcb (kernel_startup_thread, TASK_STACK_SIZE, 0, "USER"));

#if 0
    kadd_tcb (&g_pTaskRing, kcreate_tcb (debug_thread, TASK_STACK_SIZE, 0, "DEBUG"));
    kadd_tcb (&g_pTaskRing, kcreate_tcb (debug_thread, TASK_STACK_SIZE, 0, "DEBUG"));
    kadd_tcb (&g_pTaskRing, kcreate_tcb (debug_thread, TASK_STACK_SIZE, 0, "DEBUG"));
    kadd_tcb (&g_pTaskRing, kcreate_tcb (debug_thread, TASK_STACK_SIZE, 0, "DEBUG"));
#endif
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
/*
	register long _r7 asm("r7");
	register long _r8 asm("r8");
	register long _r12 asm("r12");
	register long _r13 asm("r13");
	asm volatile ( "mrs r7, cpsr");
	asm volatile ( "mov r8, lr");
	__cli ();
	printk("*** kernel_startup_thread *** [CPSR:%08x, R12:%08x, SP:%08x, LR:%08x]\n", _r7, _r12, _r13, _r8);
	__sti ();
*/

    /* driver init */

    init_uart();
    
    init_cpld();
    init_HW_chk();

    init_cmd_line();

    init_evt();
    
    init_buttons();
    init_power();
    init_rtc();
    init_usb_fw();
    
    init_fm_remote();
    
    init_ext_module();
    
    init_sound();
    
    init_disk();

    printk("[init] ------------ all drivers\n");

    print_boot_info();

    printk("[init] END\n");

    printk ("Accessing registry file /gravity.cnf...\n");
    int fReg = kfopen ("/gravity.cnf",O_RDONLY);
    if (fReg < 0)
        printk ("Setting not loaded.\n");
    else
    {
        printk ("Setting loaded.\n");
        kfclose (fReg);
    }

    avwm();
}
