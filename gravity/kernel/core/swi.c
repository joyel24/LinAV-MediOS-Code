/* 
*   kernel/swi.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/kernel.h>
#include <kernel/threads.h>
#include <kernel/pipes.h>
#include <api.h>
#include <kernel/swi.h>
#include <kernel/irq.h>

extern int gfw_swi_handler(int cmd,GFX_DATA * gfxD, void * pvData);
extern void user_printf(const char * fmt, va_list args);

__IRAM_CODE int kcswi_handler (
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3,
	unsigned long nCmd)
{
   switch (nCmd)
   {
/*
	case nAPI_MEMAVAIL: //unsigned long* pnBytes
	{
		printk ("# MEMAVAIL #\n");
		cli ();
		API_TASK_YIELD ();
		sti ();
		return 0;
	}
*/

/// Serialize critical API calls to memory manager
	case nAPI_MALLOC:   //void** ppvBuffer, unsigned long nBytes
	case nAPI_FREE:     //void* pvBuffer
	case nAPI_MEMAVAIL: //unsigned long* pnBytes
	{
		cli ();

		SYSTEM_CTRL_COMMAND* pSysCtrl = (SYSTEM_CTRL_COMMAND*)(g_pSystemCtrlPipe->buffer + g_pSystemCtrlPipe->nSender);
		pSysCtrl->nCmdId = nCmd;
		pSysCtrl->nCmdParam1 = nParam1;
		pSysCtrl->nCmdParam2 = nParam2;
		pSysCtrl->pSenderThread = g_pActiveTask;
		g_pSystemCtrlPipe->nSender = (g_pSystemCtrlPipe->nSender + sizeof(SYSTEM_CTRL_COMMAND)) & PIPE_SIZE_MASK;

		/// Block calling task...
		g_pActiveTask->nActivationTime = 1;
		g_pActiveTask->pBlockerParameter = 0;
		g_pActiveTask->pBlocker = 1;
		unsigned char cmd = KERNEL_CMD_BLOCK;
		kpipe_write (g_pKernelCtrlPipe, &cmd, 1);

		API_TASK_YIELD ();
		sti ();
	}
	break;
/// Serialize critical API calls to memory manager
        case nAPI_GFX:
            return gfw_swi_handler((int)nParam1,(GFX_DATA *)nParam2, (void *)nParam3);
        case nAPI_PRINTF:
            user_printf((const char *)nParam1, (va_list) nParam2);
            return 0;
      default:
         return 0;
   }

   return 0;
}
