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

extern int gfx_swi_handler(int cmd,GFX_DATA * gfxD, void * pvData);
extern int fs_swi(int cmd,void * data1, void * data2);
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
		TASK_INFO* pTCB = 0;
		API_MALLOC (&pTCB, sizeof(TASK_INFO));
		if (!pTCB)
			return ERR_NOMEMORY;

		API_MALLOC (&pTCB->pStack, 16384 /*nStackSize*/ );

		kInitialiseTCBVariables (pTCB, 16384 /*nStackSize*/, "USER" /*pszTaskName*/);
		unsigned char* pTopOfStack = (unsigned char*)pTCB->pStack;
		pTopOfStack += pTCB->nStackSize - 4;
		pTCB->pTopOfStack = kInitialiseStack ((unsigned long*)pTopOfStack, nParam1 /*pvTaskCode*/, nParam2 /*pParams*/);
		API_MALLOC (&pTCB->pMessagePipe, sizeof(PIPE));
		pTCB->pMessagePipe->nReceiver = 0;
		pTCB->pMessagePipe->nSender = 0;

		__cli ();
		if (nParam3)
			*((TASK_INFO**)nParam3) = pTCB;
		kadd_tcb (&g_pTaskRing, pTCB);
		__sti ();
	}
	break;

	case nAPI_TASK_SUSPEND:     //(HTASK hTask)                                                              { SAVE; asm("swi 2"); LOAD; }
	{
		__cli ();
		((TASK_INFO*)nParam1)->nBlockingState = TASK_BLOCKED_BY_SUSPEND;
		API_TASK_YIELD ();
		__sti ();
	}
	break;

	case nAPI_TASK_CONTINUE:    //(HTASK hTask)                                                              { SAVE; asm("swi 3"); LOAD; }
	{
		__cli ();
		((TASK_INFO*)nParam1)->nBlockingState = TASK_BLOCKED_BY_NONE;
		API_TASK_YIELD ();
		__sti ();
	}
	break;

	case nAPI_TASK_GETHANDLE:   //(HTASK* phTask)                                                              { SAVE; asm("swi 4"); LOAD; }
	{
		__cli ();
		*((TASK_INFO**)nParam1) = g_pTaskRing;
		__sti ();
	}
	break;

	case nAPI_TASK_SLEEP:       //(unsigned long nMilliseconds)                                   { SAVE; asm("swi 5"); LOAD; }
	{
		__cli ();
		g_pTaskRing->nBlockingState = TASK_BLOCKED_BY_SLEEP;
		g_pTaskRing->nBlockingValue = tick + nParam1 / 10;
		API_TASK_YIELD ();
		__sti ();
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

	case nAPI_TASK_WAITMESSAGE: //()
	{
	}
	break;

/// Serialize critical API calls to memory manager
	case nAPI_MALLOC:   //void** ppvBuffer, unsigned long nBytes
	case nAPI_FREE:     //void* pvBuffer
	case nAPI_MEMAVAIL: //unsigned long* pnBytes
	{
		__cli ();

		SYSTEM_CTRL_COMMAND* pSysCtrl = (SYSTEM_CTRL_COMMAND*)(g_pSystemCtrlPipe->buffer + g_pSystemCtrlPipe->nSender);
		pSysCtrl->nCmdId = nCmd;
		pSysCtrl->nCmdParam1 = nParam1;
		pSysCtrl->nCmdParam2 = nParam2;
		pSysCtrl->pSenderThread = g_pTaskRing;
		g_pSystemCtrlPipe->nSender = (g_pSystemCtrlPipe->nSender + sizeof(SYSTEM_CTRL_COMMAND)) & PIPE_SIZE_MASK;

		/// Block calling task...
		g_pTaskRing->nBlockingState = TASK_BLOCKED_BY_MEMMGR;
		g_pTaskRing->nBlockingValue = 0;

		API_TASK_YIELD ();
		__sti ();
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
		__cli ();
		PIPE* pPipe = (PIPE*)nParam1;
		kpipe_write (pPipe, (void*)nParam2, nParam3);
		__sti ();
	}
	break;

	case nAPI_PIPE_RECV:        //(HPIPE hPipe, void* pData, unsigned long nBytesToReceive);
	{
		__cli ();
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
				g_pTaskRing->nBlockingState = TASK_BLOCKED_BY_PIPE;
				g_pTaskRing->nBlockingValue = (unsigned long)pPipe;
				API_TASK_YIELD ();
			}
		}
		__sti ();
	}
	break;

	case nAPI_CRITSEC_CREATE:   //(HCRITSEC* phCritSec);
	{
		CRITSEC_INFO* pCS;
		API_MALLOC (&pCS, sizeof(CRITSEC_INFO));
		pCS->pOwnerTask = 0;
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
		__cli ();
		CRITSEC_INFO* pCS = (CRITSEC_INFO*)nParam1;
		if (!pCS->pOwnerTask)
			pCS->pOwnerTask = g_pTaskRing;
		else
		if (pCS->pOwnerTask != g_pTaskRing)
		{// Block task while critical section is busy...
			g_pTaskRing->nBlockingState = TASK_BLOCKED_BY_MUTEX;
			g_pTaskRing->nBlockingValue = nParam1;
			API_TASK_YIELD ();
		}
		__sti ();
	}
	break;

	case nAPI_CRITSEC_LEAVE:    //(HCRITSEC hCritSec);
	{
		__cli ();
		if (((CRITSEC_INFO*)nParam1)->pOwnerTask == (unsigned long)g_pTaskRing)
		{
			((CRITSEC_INFO*)nParam1)->pOwnerTask = 0;
			API_TASK_YIELD ();
		}
		__sti ();
	}
	break;

        case nAPI_GFX:
            return gfx_swi_handler((int)nParam1,(GFX_DATA *)nParam2, (void *)nParam3);
        case nAPI_PRINTF:
            user_printf((const char *)nParam1, (va_list) nParam2);
            return 0;
        case nAPI_FILE:
            return fs_swi((int)nParam1,(void *)nParam2, (void *)nParam3);
      default:
         return 0;
   }

   return 0;
}
