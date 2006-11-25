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

#include <kernel/thread.h>

#include <kernel/lcd.h>

#ifdef BUILD_LIB
extern int app_main(int argc, char * argv[]);
#endif

unsigned int _svc_IniStack = IRAM_SIZE;
unsigned int _sys_IniStack = IRAM_SIZE-SVC_STACK_SIZE;

void print_boot_info(void)
{
    printk("SP: %08x\n",get_sp());
    irq_print();
    tmr_print();
    thread_ps();
}

void kernel_thread(void);

extern THREAD_INFO * threadCurrent;

/*
void testThread1()
{
    while(1);    
    printk("About to exit th1\n");
}

void testThread2()
{
    while(1);       
    printk("About to exit th2\n");
}
*/

/*
void test_fct(void)
{
    int fd,cnt;
    
    
    fd=open("/sd.bin",O_RDWR|O_CREAT);
    if(fd<0)
    {
        printk("Error opening file\n");
        return;
    }
    cnt=write(fd,(void*)(SDRAM_START), SDRAM_END-SDRAM_START);
    printk("Wrote %d\n",cnt);
    close(fd);
    
}
*/

void kernel_start (void)
{
   /* sanity init */
    threadCurrent=NULL;
   /* malloc of max space in SDRAM */
   
    mem_init((void*)MALLOC_START,MALLOC_SIZE);

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
        IRAM_SIZE,
        (unsigned int)MALLOC_START,
        (unsigned int)MALLOC_SIZE);

    printk("Chip rev : %x\n",inw(BUS_REVR));
    
    if(thread_init(kernel_thread)!=MED_OK)
    {
        /* no KERNEL thread => loop */
        printk("CAN'T BOOT\n");
        while(1) ;
    }
    
    /* init the watchdog timer */
    wdt_init();
    /* init the irq */
    irq_init();
    /* init the tick timer */
    tmr_init();

    /* driver init */
    uart_init();
    cpld_init();
    
    lcd_init();

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
    if(disk_init()!=MED_OK)
    {
        printk("[init] ------ Halting\n");
        for(;;);
    }
    
    sound_init();
    printk("[init] ------------ drivers done\n");
    
    /*Load kernel thread to enable irq*/
    thread_loadContext();
    /* Err */
    printk("[init] error: back to main(), SYS thread not started\n");
    for(;;);
}

void kernel_thread(void)
{  
#ifdef BUILD_LIB
    char * stdalone="STDALONE";
#endif
  
    printk("[SYS thread] starting\n");
    print_boot_info();
        
#if 0   
        THREAD_INFO * ptr_thread1;
        THREAD_INFO * ptr_thread2;
        
        thread_startFct(&ptr_thread1,testThread1,"Thread 1",THREAD_DISABLE_STATE,PRIO_MED);
        thread_startFct(&ptr_thread2,testThread2,"Thread 2",THREAD_DISABLE_STATE,PRIO_LOW);
#endif
    
#ifdef BUILD_LIB
    app_main(1,&stdalone);
    reload_firmware();
#endif
    
    shell_main();

    /* should we launch HALT */
    printk("[SYS thread] error: back to main()\n");
    for(;;);
}
