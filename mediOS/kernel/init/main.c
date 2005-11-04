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

#include <kernel/bflat.h>

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/kernel.h>
#include <kernel/irq.h>
#include <kernel/timer.h>

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


#include <kernel/pipes.h>
#include <kernel/kgraphics.h>

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
}

extern void ini_debugOnScreen(void);

extern int kata_manager (void* pvParameters);
extern void API_BKT(void);
void kload_context();

void kernel_start (void)
{
    DEBUG_UART_INIT
    
    ini_graphics();
    
#ifdef USE_DEBUG_ON_SCREEN    
    ini_debugOnScreen();
#endif
    /* malloc of max space in SDRAM */
    init_malloc((void*)MALLOC_START,MALLOC_SIZE);

    /* print banner on uart */
    printk("MediOS %d.%d - kernel loading\n",VER_MAJOR,VER_MINOR);
  
    printk("Initial SP: %08x, kernel end: %08x, size in IRAM: %08x  Malloc start: %08x, size: %08x\n",get_sp(),
        (unsigned int)&_end_kernel,
        (unsigned int)&_iram_end - (unsigned int)&_iram_start,
        (unsigned int)MALLOC_START,
        (unsigned int)MALLOC_SIZE);

    /* init the irq */
    init_irq();
    
    /* init the tick timer */
    init_timer();
    
    /* driver init */

    init_uart();

    init_cpld();
#ifdef HAVE_CMD_LINE
    init_cmd_line();
#endif

#ifdef HAVE_EVT
    init_evt();
#endif

    init_buttons();   
#ifdef HAVE_BAT_POWER    
    init_power();
#endif

    init_rtc();
#ifdef HAVE_USB_FW
    init_usb_fw();
#endif
#ifdef HAVE_FM_REMOTE
    init_fm_remote();
#endif
#ifdef HAVE_EXT_MODULE
    init_ext_module();
#endif
    init_disk();
#ifdef HAVE_SOUND
    init_sound();
#endif
    /* enable the IRQ */
    printk("[init] INT enabled\n");
    __sti();
    
    printk("[init] ------------ all drivers\n");
   
    print_boot_info();

    printk("[init] END\n");

    //load_bflat("/test.grv");
    
    load_med("/test.grv");
    
    //printk("Back from blat\n");
        
    while(1) /*nothing*/;


}
