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
#include <kernel/rtc.h>
#include <kernel/usb_fw.h>
#include <kernel/bat_power.h>

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
	case nAPI_TASK_CREATE:      //(void* pvCode, void* pParam, HTASK* phTask)                     { SAVE; asm("swi 1"); LOAD; }
	{
		cli ();
		TASK_INFO* pTCB = kcreate_tcb ((void*)nParam1, 16384, (void*)nParam2, "SOFT");
		if (nParam3)
			*((TASK_INFO**)nParam3) = pTCB;
		kadd_tcb (&g_pActiveTask, pTCB);
		sti ();
	}
	break;

	case nAPI_TASK_SUSPEND:     //(HTASK hTask)                                                              { SAVE; asm("swi 2"); LOAD; }
	{
		cli ();
		unsigned char cmd = KERNEL_CMD_SUSPEND;
		kpipe_write (g_pKernelCtrlPipe, &cmd, 1);
		kpipe_write (g_pKernelCtrlPipe, &nParam1, 4);
		API_TASK_YIELD ();
		sti ();
	}
	break;

	case nAPI_TASK_CONTINUE:    //(HTASK hTask)                                                              { SAVE; asm("swi 3"); LOAD; }
	{
		cli ();
		unsigned char cmd = KERNEL_CMD_CONTINUE;
		kpipe_write (g_pKernelCtrlPipe, &cmd, 1);
		kpipe_write (g_pKernelCtrlPipe, &nParam1, 4);
		API_TASK_YIELD ();
		sti ();
	}
	break;

	case nAPI_TASK_GETHANDLE:   //(HTASK* phTask)                                                              { SAVE; asm("swi 4"); LOAD; }
	{
		cli ();
		*((TASK_INFO**)nParam1) = g_pActiveTask;
		sti ();
	}
	break;

	case nAPI_TASK_SLEEP:       //(unsigned long nMilliseconds)                                   { SAVE; asm("swi 5"); LOAD; }
	{
		cli ();
		g_pActiveTask->nActivationTime = tick + nParam1 / 10;
		unsigned char cmd = KERNEL_CMD_SLEEP;
		kpipe_write (g_pKernelCtrlPipe, &cmd, 1);
		API_TASK_YIELD ();
		sti ();
	}
	break;

	case nAPI_TASK_SENDMESSAGE: //(HTASK hTask, MESSAGE msg)                                      { SAVE; asm("swi 6"); LOAD; }
	{
	}
	break;

	case nAPI_TASK_PEEKMESSAGE: //()                                                              { SAVE; asm("swi 7"); LOAD; }
	{
	}
	break;

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

        case nAPI_TIME:
            switch((int)nParam1) {
                case 0x000:
                    return rtc_getTime((struct av_tm *)nParam2);
                case 0x001:
                    return rtc_setTime((struct av_tm *)nParam2);
                default:
                    printk("time swi %d not implemented\n",(int)nParam1);
            }
            return 0;
        case nAPI_POWER:
            switch((int)nParam1) {
                case 0x000:
                    *((int*)nParam2)=kusbIsConnected();
                    break;
                case 0x001:
                    *((int*)nParam2)=kFWIsConnected();
                    break;
                case 0x002:
                    *((int*)nParam2)=kpowerConnected();
                    break;
                case 0x003:
                    *((int*)nParam2)=kgetBatLevel();
                    break;
                default:
                    printk("power swi %d not implemented\n",(int)nParam1);
            }
            return 0;


	case nAPI_PIPE_DELETE:      //(HPIPE hPipe);
	{
		API_FREE ((void*)nParam1);
	}
	break;

	case nAPI_PIPE_SEND:        //(HPIPE hPipe, void* pData, unsigned long nBytesToSend);
	{
		cli ();
		PIPE* pPipe = (PIPE*)nParam1;
		kpipe_write (pPipe, (void*)nParam2, nParam3);
		sti ();
	}
	break;

	case nAPI_PIPE_RECV:        //(HPIPE hPipe, void* pData, unsigned long nBytesToReceive);
	{
		cli ();
		PIPE* pPipe = (PIPE*)nParam1;
		int i=0;
		unsigned char* pData = (unsigned char*)nParam2;

		while (i < nParam3)
		{
			if (pPipe->nReceiver != pPipe->nSender)
			{
				pData[i] = pPipe->buffer[pPipe->nReceiver ++];
				pPipe->nReceiver &= PIPE_SIZE_MASK;
				i ++;
			}
			else
			{
				g_pActiveTask->nActivationTime = 0;
				g_pActiveTask->pBlockerParameter = pPipe;
				g_pActiveTask->pBlocker = 0;
				unsigned char cmd = KERNEL_CMD_BLOCK;
				kpipe_write (g_pKernelCtrlPipe, &cmd, 1);
				API_TASK_YIELD ();
			}
		}
		sti ();
	}
	break;

	case nAPI_CRITSEC_CREATE:   //(HCRITSEC* phCritSec);
	{
		CRITSEC_INFO* pCS;
		API_MALLOC (&pCS, sizeof(CRITSEC_INFO));
		pCS->nBlocked = 0;
		*(CRITSEC_INFO**)nParam1 = pCS;
	}
	break;

	case nAPI_CRITSEC_DELETE:   //(HCRITSEC hCritSec);
	{
		API_FREE (nParam1);
	}
	break;

	case nAPI_CRITSEC_ENTER:    //(HCRITSEC hCritSec);
	{
		cli ();
		sti ();
	}
	break;

	case nAPI_CRITSEC_LEAVE:    //(HCRITSEC hCritSec);
	{
		cli ();
		sti ();
	}
	break;


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
