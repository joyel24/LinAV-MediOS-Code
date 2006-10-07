/*
*   kernel/init/main.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/string.h>
#include <sys_def/colordef.h>
#include <sys_def/time.h>
#include <sys_def/font.h>

#include <kernel/malloc.h>

#include <kernel/bflat.h>
#include <kernel/med.h>

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/kernel.h>
#include <kernel/delay.h>
#include <kernel/exit.h>
#include <kernel/console.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/ti_wdt.h>
#include <kernel/aic23.h>

#include <kernel/uart.h>
#include <kernel/cpld.h>

#include <kernel/version.h>

#include <kernel/rtc.h>
#include <kernel/usb_fw.h>
#include <kernel/bat_power.h>
#include <kernel/buttons.h>
#include <kernel/disk.h>
#include <kernel/ata.h>

#ifdef HAVE_FM_REMOTE
#include <kernel/fm_remote.h>
#endif

#ifdef HAVE_EXT_MODULE
#include <kernel/ext_module.h>
#include <kernel/dvr_module.h>
#include <kernel/cf_module.h>
#endif

#include <kernel/sound.h>
#include <kernel/cmd_line.h>
#include <kernel/evt.h>

#include <kernel/graphics.h>

#include <kernel/shell.h>

#include <kernel/stdfs.h>
#include <kernel/vfs.h>

#ifdef SIMPLE_LOADER
#include <kernel/bin_load.h>
#endif

#ifdef BUILD_LIB
extern int app_main(int argc, char * argv[]);
#endif

unsigned int _iram_size = IRAM_SIZE;

void print_boot_info(void)
{
    printk("SP: %08x\n",get_sp());
    irq_print();
    tmr_print();
}

void kernel_start (void)
{
#ifdef BUILD_LIB
    char * stdalone="STDALONE";
#endif

    /* malloc of max space in SDRAM */
    mem_addPool((void*)MALLOC_START,MALLOC_SIZE);
#ifdef STD_MEDIOS
    gfx_init();
#endif

#ifdef HAVE_CONSOLE
    con_init();
    con_screenSwitch();
#endif
    /* print banner on uart */
    printk("MediOS %d.%d - kernel loading\n\n",VER_MAJOR,VER_MINOR);

    printk("Initial SP: %08x, kernel end: %08x, size in IRAM: %x/%x  Malloc start: %08x, size: %x\n",get_sp(),
        (unsigned int)&_end_kernel,
        (unsigned int)&_iram_end - (unsigned int)&_iram_start,
        _iram_size,
        (unsigned int)MALLOC_START,
        (unsigned int)MALLOC_SIZE);

    /* init the watchdog timer */
    wdt_init();
    /* init the irq */
    irq_init();
    /* init the tick timer */
    tmr_init();
    printk("[init] ------------ kernel done\n");
    /* driver init */
    uart_init();
    cpld_init();
#ifdef HAVE_CMD_LINE
    init_cmd_line();
#endif
#ifdef HAVE_EVT
    evt_init();
#endif
#ifdef STD_MEDIOS
    btn_init();
#endif
#ifdef CHK_BAT_POWER
    init_power();
#endif
#ifdef STD_MEDIOS
    init_rtc();
#endif
#ifdef CHK_USB_FW
    init_usb_fw();
#endif
#ifdef HAVE_FM_REMOTE
    init_fm_remote();
#endif
#ifdef HAVE_EXT_MODULE
    init_ext_module();
    init_dvr_module();
    cf_initModule();
#endif
    ata_init();
    vfs_init();
    disk_init();
#ifdef STD_MEDIOS
    sound_init();
#endif
    /* enable the IRQ */
    printk("[init] about to enable INT\n");
    __sti();
    printk("[init] ------------ drivers done\n");
    print_boot_info();
    printk("[init] END\n");
  
    
#ifdef BUILD_LIB
    do_bkpt();
    app_main(1,&stdalone);
    reload_firmware();
#endif
   
    do_bkpt();
    
#ifdef SIMPLE_LOADER
loadBin("/medios.bin");
#else
    shell_main();
#endif
    /* should we launch HALT */
    printk("[init] error: back to main()\n");
    for(;;);
}
