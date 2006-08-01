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
#include <kernel/fm_remote.h>
#include <kernel/ext_module.h>
#include <kernel/dvr_module.h>
#include <kernel/sound.h>
#include <kernel/cmd_line.h>
#include <kernel/evt.h>

#include <kernel/graphics.h>

#include <gui/gui.h>

#include <kernel/stdfs.h>
#include <kernel/vfs.h>

#warning ADDED FOR TEST PURPOSE
#include <kernel/vfs_pathname.h>


unsigned int _iram_size = IRAM_SIZE;

void print_boot_info(void)
{
    printk("SP: %08x\n",get_sp());
    irq_print();
    tmr_print();
}

#if 0
#include <kernel/clkc.h>

void tst_fct(void){
    int m,n,d,p;
    int f;

    clkc_getClockParameters(CLK_ARM,&m,&n,&d);
    p=clkc_getPllNum(CLK_ARM);
    f=clkc_getClockFrequency(CLK_ARM);
    printk("arm f=%d m=%d n=%d d=%d p=%d\n",f,m,n,d,p);
    clkc_getClockParameters(CLK_DSP,&m,&n,&d);
    p=clkc_getPllNum(CLK_DSP);
    f=clkc_getClockFrequency(CLK_DSP);
    printk("dsp f=%d m=%d n=%d d=%d p=%d\n",f,m,n,d,p);
    clkc_getClockParameters(CLK_SDRAM,&m,&n,&d);
    p=clkc_getPllNum(CLK_SDRAM);
    f=clkc_getClockFrequency(CLK_SDRAM);
    printk("sdr f=%d m=%d n=%d d=%d p=%d\n",f,m,n,d,p);
    clkc_getClockParameters(CLK_ACCEL,&m,&n,&d);
    p=clkc_getPllNum(CLK_ACCEL);
    f=clkc_getClockFrequency(CLK_ACCEL);
    printk("axl f=%d m=%d n=%d d=%d p=%d\n",f,m,n,d,p);

    for(f=80000000;f<=200000000;f+=2000000){
        clkc_setClockFrequency(CLK_ARM,f);
        mdelay(500);
    }

    reload_firmware();
}
#endif

void tst_fct(void)
{
    DIR * dir_ptr;
    struct dirent * dir_entry;
    int i=0;
    mkdir("/testd",0);
    vfs_rootPrint();
    mkdir("/testd/testa",0);
    vfs_rootPrint();
    
    /*dir_ptr=opendir("/");
    if(!dir_ptr)
    {
        printk("Error opening root folder\n");
        return;
    }
    while((dir_entry=readdir(dir_ptr))!=NULL)
    {
        printk("%d:%s: %s\n",i++,(dir_entry->attribute&ATTR_DIRECTORY)!=0?"folder":"file",dir_entry->d_name);
    }
    closedir(dir_ptr);*/
    printk("opening testd\n");
    dir_ptr=opendir("/testd");
    if(!dir_ptr)
    {
        printk("Error opening folder\n");
        return;
    }
    while((dir_entry=readdir(dir_ptr))!=NULL)
    {
        printk("%d:%s: %s\n",i++,(dir_entry->attribute&ATTR_DIRECTORY)!=0?"folder":"file",dir_entry->d_name);
    }
    closedir(dir_ptr);
}

void kernel_start (void)
{

    /* malloc of max space in SDRAM */
    mem_addPool((void*)MALLOC_START,MALLOC_SIZE);

    gfx_init();
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
    btn_init();
#ifdef CHK_BAT_POWER
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
    sound_init();
    /* enable the IRQ */
    printk("[init] about to enable INT\n");
    __sti();
    printk("[init] ------------ drivers done\n");
    print_boot_info();
    printk("[init] END\n");

#ifdef BUILD_LIB
    do_bkpt();
    app_main(1,"STDALONE");
    reload_firmware();
#endif

#if 0
   tst_fct();
#endif

    do_bkpt();

    gui_start();

    /* should we launch HALT */
    printk("BACK TO MAIN !!!!\n");
    while(1);
}
