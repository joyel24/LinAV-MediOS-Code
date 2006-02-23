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
#include <kernel/dvr_module.h>
#include <kernel/sound.h>
#include <kernel/cmd_line.h>
#include <kernel/evt.h>

#include <kernel/graphics.h>

void print_boot_info(void)
{
    printk("SP: %08x\n",get_sp());
    irq_print();
    tmr_print();
}

extern void dbgscr_init(void);

void tst_fct(void)
{
#if 0
    int evt_hand;
    int evt;
    iniIcon();
    open_graphics();
    clearScreen(COLOR_WHITE);
    setFont(STD6X9);
    
    ini_menu();
    if(evt_getHandler(ALL_CLASS,&evt_hand)!=MED_OK)
    {
        printk("Can't get evt handler\n");
        return;
    }
    virtKbd(evt_hand);
    while(1) ;
    while(1)
    {
        evt_getStatus(evt_hand,&evt);
        menuEvtHandler(evt);
    }
    /*ini_file_browser();
    browse("/",1);*/

    int evt_hand;
    struct evt_t evt;
    if(evt_getHandler(ALL_CLASS,&evt_hand)!=MED_OK)
    {
        printk("Can't get evt handler\n");
        return;
    }

    while(1)
    {
        if(evt_getFullStatus(evt_hand, &evt)!=MED_OK)
        {
            printk("Can't get evt\n");
            break;
        }
        
        if(evt.evt)
            printk("evt %x:%x\n",evt.evt,(int)evt.data);
    }
    
    evt_freeHandler(evt_hand);
#endif
}

void kernel_start (void)
{
    /* malloc of max space in SDRAM */
    mem_addPool((void*)MALLOC_START,MALLOC_SIZE);

    gfx_init();

    con_init();
    con_screenSwitch();

    /* print banner on uart */
    printk("MediOS %d.%d - kernel loading\n\n",VER_MAJOR,VER_MINOR);

    printk("Initial SP: %08x, kernel end: %08x, size in IRAM: %d  Malloc start: %08x, size: %d\n",get_sp(),
        (unsigned int)&_end_kernel,
        (unsigned int)&_iram_end - (unsigned int)&_iram_start,
        (unsigned int)MALLOC_START,
        (unsigned int)MALLOC_SIZE);

    
        
    /* init the watchdog timer */
    wdt_init();

    /* init the irq */
    irq_init();

    /* init the tick timer */
    tmr_init();

    /* driver init */

    uart_init();

    cpld_init();

#ifdef HAVE_CMD_LINE
    init_cmd_line();
#endif

#ifdef HAVE_EVT
    evt_init();
#endif
    btn_init();
#ifdef HAVE_BAT_POWER
    init_power();
#endif

    init_rtc();
    init_usb_fw();

#ifdef HAVE_FM_REMOTE
    init_fm_remote();
#endif
#ifdef HAVE_EXT_MODULE
    init_ext_module();
#endif
#ifdef HAVE_DVR
    init_dvr_module();
#endif
    disk_init();

    /* enable the IRQ */
    printk("[init] INT enabled\n");
    __sti();

#ifdef HAVE_SOUND
    init_sound();
#endif

    printk("[init] ------------ all drivers\n");

    print_boot_info();

    printk("[init] END\n");

/* evt & btn test */
#if 0
   tst_fct();
    while(1)
    {
        int key=btn_readState();
        if(key)
            printk("%x\n",key);
    }
#endif
#ifdef BUILD_LIB
    do_bkpt();
    app_main();
    reload_firmware();
#endif
    do_bkpt();

    //load_med("/doom.med");
    load_med("/test.med");

    printk("Back from med\n");

    while(1);
}
