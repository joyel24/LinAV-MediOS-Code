/* 
*   kernel/swi_kernel.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/kernel.h>
#include <kernel/swi.h>
#include <kernel/threads.h>
#include <kernel/evt.h>

int swi_kernel_handler (
	unsigned long nCmd,
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3)
{
	switch (nCmd)
	{
	case nAPI_TASK_CREATE:      //(void* pvCode, void* pParam, HTASK* phTask)                     { SAVE; asm("swi 1"); LOAD; }
	{
		TASK_INFO* pTCB = 0;
		API_MALLOC ((void**)&pTCB, sizeof(TASK_INFO));
		if (!pTCB)
			return ERR_NOMEMORY;

		API_MALLOC ((void**)&pTCB->pStack, 16384);

		kInitialiseTCBVariables (pTCB, 16384, "USER");
		unsigned char* pTopOfStack = (unsigned char*)pTCB->pStack;
		pTopOfStack += pTCB->nStackSize - 4;
		pTCB->pTopOfStack = kInitialiseStack ((unsigned long*)pTopOfStack, nParam1, nParam2);
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
		if (nParam1 == 0)
			break;

		__cli ();
		g_pTaskRing->nBlockingState = TASK_BLOCKED_BY_SLEEP;
		g_pTaskRing->nBlockingValue = tick + nParam1 / 10;
		API_TASK_YIELD ();
		__sti ();
	}
	break;

	case nAPI_TASK_SENDMESSAGE: //(HTASK hTask, MESSAGE* pMsg)
	{
		__cli ();

		PIPE* pPipe = ((TASK_INFO*)nParam1)->pMessagePipe;
		MESSAGE* pMsg = (MESSAGE*)nParam2;

		SYSTEM_CTRL_COMMAND* pSysCtrl = (SYSTEM_CTRL_COMMAND*)(pPipe->buffer + pPipe->nSender);
		pSysCtrl->nCmdId        = pMsg->nMsg;
		pSysCtrl->nCmdParam1    = pMsg->nParam1;
		pSysCtrl->nCmdParam2    = pMsg->nParam2;
		pSysCtrl->pSenderThread = g_pTaskRing;
		pPipe->nSender = (pPipe->nSender + sizeof(SYSTEM_CTRL_COMMAND)) & PIPE_SIZE_MASK;

		__sti ();
	}
	break;

	case nAPI_TASK_PEEKMESSAGE: //(MESSAGE* pMsg)
	{
		int nDataAvailable = 0;
		__cli ();
		if (g_pTaskRing->pMessagePipe->nReceiver != g_pTaskRing->pMessagePipe->nSender)
			nDataAvailable = 1;
		__sti ();

		if (nDataAvailable)
			API_TASK_WAITMESSAGE (nParam1);
		else
			return ERR_RECEIVER_EMPTY;
	}
	break;

	case nAPI_TASK_WAITMESSAGE: //(MESSAGE* pMsg)
	{
		__cli ();
		PIPE* pPipe = g_pTaskRing->pMessagePipe;

		if (pPipe->nReceiver == pPipe->nSender)
		{
			g_pTaskRing->nBlockingState = TASK_BLOCKED_BY_PIPE;
			g_pTaskRing->nBlockingValue = pPipe;
			API_TASK_YIELD ();
		}

		__cli ();

		SYSTEM_CTRL_COMMAND* pSysCtrl = (SYSTEM_CTRL_COMMAND*)(pPipe->buffer + pPipe->nReceiver);
		MESSAGE* pMsg = (MESSAGE*)nParam1;
		pMsg->nMsg    = pSysCtrl->nCmdId;
		pMsg->nParam1 = pSysCtrl->nCmdParam1;
		pMsg->nParam2 = pSysCtrl->nCmdParam2;
		pPipe->nReceiver = (pPipe->nReceiver + sizeof(SYSTEM_CTRL_COMMAND)) & PIPE_SIZE_MASK;

		__sti ();
	}
	break;

	case nAPI_TASK_SETPRIORITY: //(HTASK hTask, int nPriority);
	{
		__cli ();
		((TASK_INFO*)nParam1)->nPriority = nParam2;
		((TASK_INFO*)nParam1)->nCurrentScore = nParam2;
		__sti ();
	}
	break;

/// Serialize critical API calls to memory manager
	case nAPI_TASK_TERMINATE:   //()
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

	case nAPI_PIPE_CREATE:      //(HPIPE* phPipe);
	{
		PIPE* pPipe = 0;
		API_MALLOC (&pPipe, sizeof(PIPE));
		pPipe->nReceiver = 0;
		pPipe->nSender = 0;
		*((PIPE**)nParam1) = pPipe;
	}
	break;

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
		{
			pCS->pOwnerTask = g_pTaskRing;
			pCS->nLockCounter = 0;
		}
		else
		if (pCS->pOwnerTask != g_pTaskRing)
		{// Block task while critical section is busy...
			g_pTaskRing->nBlockingState = TASK_BLOCKED_BY_MUTEX;
			g_pTaskRing->nBlockingValue = nParam1;
			API_TASK_YIELD ();
		}
		else
		{
			pCS->nLockCounter ++;
		}
		__sti ();
	}
	break;

	case nAPI_CRITSEC_LEAVE:    //(HCRITSEC hCritSec);
	{
		__cli ();
		if (((CRITSEC_INFO*)nParam1)->pOwnerTask == (unsigned long)g_pTaskRing)
		{
			if (((CRITSEC_INFO*)nParam1)->nLockCounter == 0)
			{
				((CRITSEC_INFO*)nParam1)->pOwnerTask = 0;
				API_TASK_YIELD ();
			}
			else
			{
				((CRITSEC_INFO*)nParam1)->nLockCounter --;
			}
		}
		__sti ();
	}
	break;

	case nAPI_CRITSEC_TRYENTER: //(HCRITSEC hCritSec);
	{
		int nError = ERR_OK;
		__cli ();
		CRITSEC_INFO* pCS = (CRITSEC_INFO*)nParam1;
		if (!pCS->pOwnerTask)
		{
			pCS->pOwnerTask = g_pTaskRing;
			pCS->nLockCounter = 0;
		}
		else
		{
			nError = ERR_SECTION_LOCKED;
		}
		__sti ();
		return nError;
	}
	break;

        case nAPI_EVT:
            switch((int)nParam1)
            {
                case 0x000:  /* get handling */
                    *(unsigned int *)nParam2=(unsigned int)get_evt_handling();
                    break;
                case 0x001: /* rm handling */
                    rm_evt_handling((struct evt_pipes_s *)nParam2);
                    break;
                case 0x002: /* wait evt */
                    API_PIPE_RECV(&((struct evt_pipes_s *)nParam2)->evt_pipe, nParam3 , 1);
                    break;
                case 0x003: /* send evt */
                    send_evt(*(int*)nParam2);
                    break;
                case 0x004: /* send evt */
                    __cli ();
                    ((struct evt_pipes_s *)nParam2)->evt_pipe.nReceiver = 0;
                    ((struct evt_pipes_s *)nParam2)->evt_pipe.nSender = 0;
                    __sti ();
                    break;                    
            }
            return 0;
      default:
         printk("Unknown SWI call %d\n", nCmd);
		return ERR_NOT_IMPLEMENTED;
	}

	return 0;
}
