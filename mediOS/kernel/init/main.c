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

void kernel_thread(void);

#ifdef STD_MEDIOS
#include <kernel/lcd.h>
void test_fct(void)
{
    int evt;
    int evt_handler=evt_getHandler(BTN_CLASS);
    if(evt_handler<0)             /* we need a proper error handling in api */
    {
        printf("[main test] can't register to evt\n");
        return;
    }

    while(1)
    {
        evt=evt_getStatus(evt_handler);
        /*while(!stop)
        {
            evt=evt_getStatus(evt_handler);
            switch(evt)
            {
                case BTN_LEFT:
                    GIO_DIRECTION(gio_n,GIO_OUT);
                    GIO_SET(gio_n);
                    printk("GIO %d SET\n",gio_n);
                    break;
                case BTN_RIGHT:
                    GIO_DIRECTION(gio_n,GIO_OUT);
                    GIO_CLEAR(gio_n);
                    printk("GIO %d CLEAR\n",gio_n);
                    break;
                case BTN_UP:
                    if(gio_n<40)
                        gio_n++;
                    printk("Cur gio = %d\n",gio_n);
                    break;
                case BTN_DOWN:
                    if(gio_n>0)
                        gio_n--;
                    printk("Cur gio = %d\n",gio_n);
                    break;
                case BTN_OFF:
                    stop=1;
                    break;
            }
        }*/
        if(evt==BTN_LEFT)
        {
            lcd_ON();
            printk("LCD ON\n");
        }
        else if (evt==BTN_RIGHT)
        {
            lcd_OFF();
            printk("LCD OFF\n");
        }
        else if (evt==BTN_OFF)
            break;
    }
}
#endif

extern THREAD_INFO * threadCurrent;

void testThread1()
{
    int i;
    while(1)
    {
        uart_out('1',0);
        for(i=0;i<10000;i++) /*nothing*/;
    }
}

void testThread2()
{
    int i;
    while(1)
    {
        uart_out('2',0);
        for(i=0;i<10000;i++) /*nothing*/;
    }
}


void kernel_start (void)
{


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

    printk("Chip rev : %x\n",inw(BUS_REVR));

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
    lcd_init;
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
    printk("[init] ------------ drivers done\n");
    
    /*init thread before irq enable*/
    thread_init(kernel_thread);
    /* Err */
    printk("[init] error: back to main(), SYS thread not started\n");
    for(;;);
}

void kernel_thread(void)
{  
#ifdef BUILD_LIB
    char * stdalone="STDALONE";
#endif

    THREAD_INFO * ptr1,* ptr2;
    
    printk("[SYS thread] starting\n");
    
    /*thread_create(&ptr1,(void*)testThread1,"Thread 1");
    thread_create(&ptr2,(void*)testThread2,"Thread 2");
    thread_insert(ptr1);
    thread_insert(ptr2);*/

    print_boot_info();

#if 0
    test_fct();
#endif


#ifdef BUILD_LIB
    do_bkpt();
    app_main(1,&stdalone);
    reload_firmware();
#endif

    do_bkpt();

#ifdef SIMPLE_LOADER
#ifdef LOADBIN
#warning using LOADBIN
loadBin(LOADBIN);
#else
loadBin("/medios.bin");
#endif
#else
    shell_main();
#endif
    /* should we launch HALT */
    printk("[SYS thread] error: back to main()\n");
    for(;;);
}
