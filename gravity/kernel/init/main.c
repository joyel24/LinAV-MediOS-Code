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

#include <sys_def/file.h>

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

extern void API_BKT(void);

#if 1
typedef struct _SDBG
{
	GFX_POINT ptOrig;
	int w, h;
	int nDelay;
	int nStep;
} SDBG;
void win_thread (GFX_RECT* rc)
{
	API_GFX_CREATE_CONTEXT (rc->w, rc->h, 0);
	GFX_POINT pt;
	pt.x = rc->x;
	pt.y = rc->y;
	API_GFX_MOVE (&pt);
	while (1)
		API_TASK_SLEEP (10000);
}

void debug_thread (SDBG* pS)
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

	API_GFX_CREATE_CONTEXT (pS->w, pS->h, 0);

	TASK_INFO* pTask = 0;
	API_TASK_GETHANDLE (&pTask);

	GFX_POINT pt;

	int i = 0;

	while (1)
	{
		pt.x = pS->ptOrig.x + (i%140);
		pt.y = pS->ptOrig.y + (i%85);

		i += 1;

		API_GFX_MOVE (&pt);
	};
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
    kadd_tcb (&g_pTaskRing, kcreate_tcb (kmemory_manager,       TASK_STACK_SIZE,   0, "MEMMGR"));
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

SDBG pS1, pS2, pS3;

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

    char * ptr1,*ptr2,*ptr3,*ptr4;
    int i,count,j;

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
    

   
    
    //////////////////////////////////////////////////////////
    //////////// MP3 PLAYER test
    
    start_mp3_player("/file.mp3");
    
    
    ///////////////////////////////////////////////////////////

	printk ("Accessing registry file /gravity.cnf...\n");
	int fReg = kfopen ("/gravity.cnf",O_RDONLY);
	if (fReg < 0)
		printk ("Setting not loaded.\n");
	else
	{
		printk ("Setting loaded.\n");
		kfclose (fReg);
	}

	printk ("Loading background image...\n");
	int fBack = kfopen ("/back.img",O_RDONLY);
	if (fBack < 0)
		printk ("Background not loaded.\n");
	else
	{
		int nReaded = fread (fBack, screen_VID2, 307200);
		printk ("Background loaded (%d bytes, %d)\n", nReaded, screen_VID2[0]);
		kfclose (fBack);
	}

	////////////////////////////////////////////////////
	// Setup background memory context in vid1 plane...
	GFX_init ();
	////////////////////////////////////////////////////

	pS2.ptOrig.x = 40;
	pS2.ptOrig.y = 20;
	pS2.w = 128;
	pS2.h = 80;
	pS2.nDelay = 0;
	pS2.nStep = 2;
	API_TASK_CREATE (debug_thread, &pS2, 0);

	GFX_RECT rc1;
	rc1.x = 200;
	rc1.y = 100;
	rc1.w = 32;
	rc1.h = 32;
	API_TASK_CREATE (win_thread, &rc1, 0);

	GFX_RECT rc2;
	rc2.x = 90;
	rc2.y = 90;
	rc2.w = 64;
	rc2.h = 64;
	API_TASK_CREATE (win_thread, &rc2, 0);

	GFX_RECT rc3;
	rc3.x = 160;
	rc3.y = 20;
	rc3.w = 80;
	rc3.h = 64;
	API_TASK_CREATE (win_thread, &rc3, 0);

	GFX_RECT rc4;
	rc4.x = 0;
	rc4.y = 0;
	rc4.w = 100;
	rc4.h = 100;
	API_TASK_CREATE (win_thread, &rc4, 0);

	GFX_RECT rc5;
	rc5.x = 250;
	rc5.y = 10;
	rc5.w = 20;
	rc5.h = 220;
	API_TASK_CREATE (win_thread, &rc5, 0);

	GFX_RECT rc6;
	rc6.x = 10;
	rc6.y = 150;
	rc6.w = 48;
	rc6.h = 48;
	API_TASK_CREATE (win_thread, &rc6, 0);

	API_GFX_UPDATE_RECT (0);

//    avwm();
	while (1)
	{
		API_TASK_SLEEP (10000);
	};
}
