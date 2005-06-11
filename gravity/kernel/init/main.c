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

//#include "../../apps/gpsnav.h"

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

#if 1

int seed = 1;
int random ()
{
	seed *= 0x000343FD;
	seed += 0x00269EC3;
	return (seed >> 10) & 0x7FFF;
}

void SetSkin (int w, int h)
{
	HTASK hTask = 0;
	API_TASK_GETHANDLE (&hTask);
	TASK_INFO* pTI = (TASK_INFO*)hTask;

	pTI->pRegionLeft = 0;
	API_MALLOC ((void**)&pTI->pRegionLeft, h * 4);
	pTI->pRegionRight = 0;
	API_MALLOC ((void**)&pTI->pRegionRight, h * 4);

	int i;
	for (i=0;i<h;i++)
	{
		pTI->pRegionLeft[i]  = 0;
		pTI->pRegionRight[i] = 0;
	}

	pTI->pRegionLeft[0] = pTI->pRegionRight[0] = 9;
	pTI->pRegionLeft[1] = pTI->pRegionRight[1] = 6;
	pTI->pRegionLeft[2] = pTI->pRegionRight[2] = 4;
	pTI->pRegionLeft[3] = pTI->pRegionRight[3] = 3;
	pTI->pRegionLeft[4] = pTI->pRegionRight[4] = 2;
	pTI->pRegionLeft[5] = pTI->pRegionRight[5] = 2;
	pTI->pRegionLeft[6] = pTI->pRegionRight[6] = 1;
	pTI->pRegionLeft[7] = pTI->pRegionRight[7] = 1;
	pTI->pRegionLeft[8] = pTI->pRegionRight[8] = 1;

	pTI->pRegionLeft[h-1-0] = pTI->pRegionRight[h-1-0] = 9;
	pTI->pRegionLeft[h-1-1] = pTI->pRegionRight[h-1-1] = 6;
	pTI->pRegionLeft[h-1-2] = pTI->pRegionRight[h-1-2] = 4;
	pTI->pRegionLeft[h-1-3] = pTI->pRegionRight[h-1-3] = 3;
	pTI->pRegionLeft[h-1-4] = pTI->pRegionRight[h-1-4] = 2;
	pTI->pRegionLeft[h-1-5] = pTI->pRegionRight[h-1-5] = 2;
	pTI->pRegionLeft[h-1-6] = pTI->pRegionRight[h-1-6] = 1;
	pTI->pRegionLeft[h-1-7] = pTI->pRegionRight[h-1-7] = 1;
	pTI->pRegionLeft[h-1-8] = pTI->pRegionRight[h-1-8] = 1;

	API_GFX_CREATE_CONTEXT (w, h, 0);

	GFX_RECT rc;
	rc.x = 0;
	rc.y = 0;
	rc.w = w;
	rc.h = h;

//	API_GFX_FILLRECT (&rc, random());

/*
	switch (random()%5)
	{
		case 0: API_GFX_FILLRECT (&rc, COLOR32_BLACK); break;
		case 1: API_GFX_FILLRECT (&rc, COLOR32_RED); break;
		case 2: API_GFX_FILLRECT (&rc, COLOR32_BLUE); break;
		case 3: API_GFX_FILLRECT (&rc, COLOR32_GREEN); break;
		case 4: API_GFX_FILLRECT (&rc, COLOR32_ORANGE); break;
	}
*/

	rc.x = 5;
	rc.y = 5;
	rc.w = w-10;
	rc.h = h-10;
	API_GFX_SET_DRAWING_RECT (&rc);
}

typedef struct _SDBG
{
	GFX_POINT ptOrig;
	int w, h;
	int nDelay;
	int nStep;
	COLOR color;
} SDBG;

void win_thread (GFX_RECT* rc)
{
	SetSkin (rc->w, rc->h);
//	API_GFX_CREATE_CONTEXT (rc->w, rc->h, 0);

	printk ("We are before moving our context...\n");

	GFX_POINT pt;
	pt.x = rc->x;
	pt.y = rc->y;
	API_GFX_MOVE (&pt);

	printk ("Sleeping...\n");
	API_TASK_SLEEP (1000000);

	while (1)
	{
//		API_TASK_SLEEP (50);

		int i;
		for (i=0;i<5;i++)
		{
			GFX_POINT pt1, pt2;
			pt1.x = random() % rc->w;
			pt1.y = random() % rc->h;
			pt2.x = random() % rc->w;
			pt2.y = random() % rc->h;
			API_GFX_DRAWLINE (&pt1, &pt2, random());
		}

		API_GFX_UPDATE_RECT (0);
	}
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

	SetSkin (pS->w, pS->h);
//	API_GFX_CREATE_CONTEXT (pS->w, pS->h, 0);

	TASK_INFO* pTask = 0;
	API_TASK_GETHANDLE (&pTask);

//	printk ("app mctx 1 w/h: %i, %i\n", pTask->pMemoryContext->w, pTask->pMemoryContext->h);

	API_TASK_SETPRIORITY (pTask, 13);

//	printk ("app mctx 1a w/h: %i, %i\n", pTask->pMemoryContext->w, pTask->pMemoryContext->h);

	GFX_RECT rc;
	rc.x = 0;
	rc.y = 0;
	rc.w = pS->w;
	rc.h = pS->h;
	API_GFX_FILLRECT (&rc, pS->color);

	GFX_POINT pt;

	pt.x = 0;
	pt.y = 0;
	int dx = 1;
	int dy = 1;

//	printk ("app mctx 2 w/h: %i, %i\n", pTask->pMemoryContext->w, pTask->pMemoryContext->h);

	while (1)
	{
		API_TASK_SLEEP (10);

		pt.x += dx;
		pt.y += dy;

		if (pt.x < 0)
			dx = -dx;

		if (pt.y < 0)
			dy = -dy;

		if (pt.x+pS->w >= 320)
			dx = -dx;

		if (pt.y+pS->h >= 240)
			dy = -dy;

//		pt.x = pS->ptOrig.x + (i%140);
//		pt.y = pS->ptOrig.y + (i%85);

//		printk("[%i, %i, %.8x]\n", pt.x, pt.y, (unsigned long)&pt);

//		printk ("app mctx 3 w/h: %i, %i\n", pTask->pMemoryContext->w, pTask->pMemoryContext->h);

//		i += 1;

		API_GFX_MOVE (&pt);

//		if (i > 10)
//			break;
	};

	__cli ();
	printk("*** debug_thread exiting ***\n");
	__sti ();
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
    kadd_tcb (&g_pTaskRing, kcreate_tcb (kata_manager,          TASK_STACK_SIZE,   0, "ATAMGR"));
    kadd_tcb (&g_pTaskRing, kcreate_tcb (kernel_startup_thread, TASK_STACK_SIZE,   0, "USER"));


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

SDBG pS1, pS2, pS3, pS4, pS5;

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

//    int i;
//    DIR* myDir;
//    struct dirent* entry;

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
    
//    start_mp3_player("/file.mp3");
    
    
    ///////////////////////////////////////////////////////////

	printk ("Accessing registry file /gravity.cnf...\n");
	int fReg = fopen ("/gravity.cnf",O_RDONLY);
	if (fReg < 0)
		printk ("Setting not loaded.\n");
	else
	{
		printk ("Setting loaded.\n");
		fclose (fReg);
	}

	printk ("Loading background image...\n");
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

	////////////////////////////////////////////////////
	// Setup background memory context in vid1 plane...
	GFX_init ();
	////////////////////////////////////////////////////


/*        myDir=opendir("/");
        while(entry=readdir(myDir))
            printk("%s\n",entry->d_name);
        closedir(myDir);*/
//	API_RUN_GRV ("/jpeg_view.grv", 0);

//	API_RUN_GRV ("/gpsnav.grv", 0);
//	_start ();


	pS1.ptOrig.x = 40;
	pS1.ptOrig.y = 20;
	pS1.w = 128;
	pS1.h = 80;
	pS1.nDelay = 0;
	pS1.nStep = 2;
	pS1.color = COLOR32_PINK;
	API_TASK_CREATE (debug_thread, &pS1, 0);

//	GFX_RECT rc1;
//	rc1.x = 200;
//	rc1.y = 100;
//	rc1.w = 32;
//	rc1.h = 32;
	pS2.w = 100;
	pS2.h = 32;
	pS2.color = COLOR32_RED;
	API_TASK_CREATE (debug_thread, &pS2, 0);

//	GFX_RECT rc2;
//	rc2.x = 90;
//	rc2.y = 90;
//	rc2.w = 64;
//	rc2.h = 64;
	pS3.w = 32;
	pS3.h = 128;
	pS3.color = COLOR32_BLUE;
	API_TASK_CREATE (debug_thread, &pS3, 0);

//	GFX_RECT rc3;
//	rc3.x = 160;
//	rc3.y = 20;
//	rc3.w = 80;
//	rc3.h = 64;
	pS4.w = 24;
	pS4.h = 24;
	pS4.color = COLOR32_GREEN;
	API_TASK_CREATE (debug_thread, &pS4, 0);

//	GFX_RECT rc4;
//	rc4.x = 5;
//	rc4.y = 25;
//	rc4.w = 110;
//	rc4.h = 100;
	pS5.w = 256;
	pS5.h = 16;
	pS5.color = COLOR32_BLACK;
	API_TASK_CREATE (debug_thread, &pS5, 0);


/*
	GFX_RECT rc5;
	rc5.x = 9;
	rc5.y = 5;
	rc5.w = 73;
	rc5.h = 230;
	API_TASK_CREATE (win_thread, &rc5, 0);
*/

/*
	GFX_RECT rc6;
	rc6.x = 10;
	rc6.y = 150;
	rc6.w = 48;
	rc6.h = 48;
	API_TASK_CREATE (win_thread, &rc6, 0);
*/

/*
	printk ("Loading font 1...\n");
	void* pFont1 = 0;
	int fFnt1 = kfopen ("/fonts/8bit.grf",O_RDONLY);
	if (fFnt1 < 0)
		printk ("Font not loaded.\n");
	else
	{
		int nSize = filesize (fFnt1);
		API_MALLOC (&pFont1, nSize);
		int nReaded = fread (fFnt1, pFont1, nSize);
		printk ("Font loaded (%d bytes)\n", nReaded);
		kfclose (fFnt1);
	}


	API_SET_FONT_COLOR (COLOR32_RED);
	printk ("Setting font...\n");
	API_SET_FONT (pFont1);


	while (1)
	{
		printk ("Printing text...\n");
		for (i=0;i<5000;i++)
		{
			API_TEXT ("Hello from GRAVITY!", 50, 50);
		}
		API_GFX_UPDATE_RECT (0);
	}
*/

    //avwm();
{

	while (1)
	{
/*
		int fLog = fopen ("/log.txt",O_RDWR | O_APPEND);
		if (fLog >= 0)
		{
			printk ("Log print...\n");
			fwrite (fLog, "Log...\r\n", 8);
			fsync (fLog);
			kfclose (fLog);
		}
		else
			printk ("Log not opened.\n");
*/

/*
		int i;

		API_TASK_SLEEP (2000);

		for (i=1;i<=10;i++)
		{
			API_SET_LCD_BRIGHTNESS (i);
			API_TASK_SLEEP (50);
		}

		API_TASK_SLEEP (2000);

		for (i=10;i>=1;i--)
		{
			API_SET_LCD_BRIGHTNESS (i);
			API_TASK_SLEEP (50);
		}
*/

		API_TASK_SLEEP (3000);
	};
}

}
