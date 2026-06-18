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

#include <sys_def/string.h>

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
#include <kernel/gfxmgr.h>

#include <kernel/config.h>

#include <kernel/kfile.h>
#include <kernel/kdir.h>
#include <sys_def/colordef.h>

#include <kernel/dsp.h>

#include <kernel/osd.h>

extern char screen_VID2 [320 * 240 *4 + 40];

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
extern int kata_manager (void* pvParameters);
extern void API_BKT(void);
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
    
do_tasks (NULL);

///////////////////////////////////////////////////
/// TODO: This code gows to kinit_tcb ()...
    kadd_tcb (&g_pTaskRing, kcreate_tcb (kmemory_manager,       TASK_STACK_SIZE,   0, "MEMMGR"));
    kadd_tcb (&g_pTaskRing, kcreate_tcb (kata_manager,          TASK_STACK_SIZE,   0, "ATAMGR"));
    kadd_tcb (&g_pTaskRing, kcreate_tcb (kernel_startup_thread, TASK_STACK_SIZE,   0, "U_KSTART"));

///////////////////////////////////////////////////
do_tasks (NULL);
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

    init_disk();
    
    init_sound();

    printk("[init] ------------ all drivers\n");
   
    print_boot_info();

    printk("[init] END\n");

    /*printk ("Loading background image...\n");
    int fBack = fopen ("/back.img",O_RDONLY);
    if (fBack < 0)
            printk ("Background not loaded.\n");
    else
    {
            printk ("Reading background image...\n");
            int nReaded = fread (fBack, screen_VID2, 307200);
            printk ("Background loaded (%d bytes, %x @%08x)\n", nReaded, screen_VID2[0], screen_VID2);
            kfclose (fBack);
    }
*/
    //GFX_init ();
    
    //API_RUN_GRV("/othello.grv",0);
    API_RUN_GRV("/av_sms.grv",0);
    //avwm();
    //av_sms();
    
    while(1) API_TASK_SLEEP (3000);


}
