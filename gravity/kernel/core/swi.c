/* 
*   kernel/swi.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <sys_def/stddef.h>

#include <kernel/kernel.h>
#include <kernel/threads.h>
#include <kernel/pipes.h>
#include <api.h>
#include <kernel/swi.h>
#include <kernel/irq.h>
#include <kernel/rtc.h>
#include <kernel/usb_fw.h>
#include <kernel/bat_power.h>
#include <kernel/sound.h>
#include <kernel/evt.h>

extern int gfx_swi_handler(int cmd,GFX_DATA * gfxD, void * pvData);
extern int fs_swi(int cmd,void * data1, void * data2);
extern void user_printf(const char * fmt, va_list args);

#define MAX(a,b) (a>b?a:b)
#define MIN(a,b) (a<b?a:b)

__IRAM_CODE int kcswi_handler (
	unsigned long nCmd,
	unsigned long nParam2,
	unsigned long nParam3,
	unsigned long nParam1)
{
   switch (nCmd)
   {
	case nAPI_TASK_CREATE:      //(void* pvCode, void* pParam, HTASK* phTask)                     { SAVE; asm("swi 1"); LOAD; }
	{
		TASK_INFO* pTCB = 0;
		API_MALLOC (&pTCB, sizeof(TASK_INFO));
		if (!pTCB)
			return ERR_NOMEMORY;

		API_MALLOC (&pTCB->pStack, 16384);

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

        case nAPI_HEAP_CREATE:      //(HEAP* phHeap, unsigned long nBytes);
	{
		// TO DO:
	}
	break;

        case nAPI_HEAP_DESTROY:     //(HEAP hHeap);
	{
		// TO DO:
	}
	break;

        case nAPI_HEAP_MALLOC:      //(HEAP hHeap, void** ppvBuffer, unsigned long nBytes);
	{
		// TO DO:
	}
	break;

        case nAPI_HEAP_FREE:        //(HEAP hHeap, void* pvBuffer);
	{
		// TO DO:
	}
	break;

        case nAPI_HEAP_AVAIL:       //(HEAP hHeap, unsigned long* pnBytes);
	{
		// TO DO:
	}
	break;

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

	case nAPI_CRITSEC_TRYENTER: //(HCRITSEC hCritSec);
	{
		// TO DO:
	}
	break;

	case nAPI_GFX_FASTBLIT:     //(GFX_DATA* pDst, GFX_DATA* pSrc, GFX_POINT* pOrigin);
	{
		GFX_DATA* pDst     = (GFX_DATA*)nParam1;
		GFX_DATA* pSrc     = (GFX_DATA*)nParam2;
		GFX_POINT* pOrigin = (GFX_POINT*)nParam3;

		unsigned char nSrcElementSize = pSrc->pixel_size;
		unsigned char nDstElementSize = pDst->pixel_size;

		int bDoReverse = 0;
		if (pSrc->direction != pDst->direction)
			bDoReverse = 1;

		int xmin = MAX(pOrigin->x,0);
		int ymin = MAX(pOrigin->y,0);
		int xmax = MIN(pDst->w, pSrc->w + pOrigin->x);
		int ymax = MIN(pDst->h, pSrc->h + pOrigin->y);
		int dx = xmax - xmin;
		int dy = ymax - ymin;

		if ((!dx) || (!dy))
			break;

		int i,j;

		long st = pSrc->delta;
		long dt = pDst->delta;
		unsigned char* sptr;
		unsigned char* dptr = (unsigned char*)(pDst->pixels + dt*ymin + xmin*nDstElementSize);

		if (bDoReverse)
		{
			if (pOrigin->y < 0)
				sptr = (unsigned char*)(pSrc->pixels + MAX(0,-pOrigin->x)*nSrcElementSize + (dy-1)*st);
			else
				sptr = (unsigned char*)(pSrc->pixels + MAX(0,-pOrigin->x)*nSrcElementSize + (pSrc->h-1)*st);
			st = -st;
		}
		else
		{
			sptr = (unsigned char*)(pSrc->pixels + MAX(0,-pOrigin->x)*nSrcElementSize + MAX(0,-pOrigin->y)*st);
		}

		if (nSrcElementSize == nDstElementSize)
		{
			int nRow = dx * nDstElementSize;
			for (i=0;i<dy;i++)
			{
				memcpy (dptr, sptr, nRow);
				dptr += dt;
				sptr += st;
			}
		}
		else
			return ERR_INVALID_PARAM;
	}
	break;

	case nAPI_GFX_STRETCHBLIT: //GFX_DATA* pDst, GFX_DATA* pSrc, unsigned long* pWorkBuffer
	{
		GFX_DATA* pDst = (GFX_DATA*)nParam1;
		GFX_DATA* pSrc = (GFX_DATA*)nParam2;

		if ((pSrc->pixel_size != 4) || (pDst->pixel_size != 4))
			return ERR_INVALID_PARAM;

		AlphaBilinearInterpolatedStretch (
			pDst,
			pSrc,
			(unsigned long*)nParam3,
			(unsigned long*)nParam3 + pDst->w);
	}
	break;

	case nAPI_GFX_PATTERNBLIT: //(GFX_DATA* pDst, GFX_DATA* pSrc);
	{
		GFX_DATA* pDst    = (GFX_DATA*)nParam1;
		GFX_DATA* pSrc    = (GFX_DATA*)nParam2;

		GFX_POINT org;
		for (org.y=0;org.y<pDst->h+pSrc->h;org.y+=pSrc->h)
		for (org.x=0;org.x<pDst->w+pSrc->w;org.x+=pSrc->w)
			kcswi_handler (nParam1, nParam2, (unsigned long)&org, nAPI_GFX_FASTBLIT);
	}
	break;

	case nAPI_RUN_GRV:          //(const char* pGRVPath, HTASK* phTask)
	{
		TASK_INFO* pTCB = 0;

		API_MALLOC (&pTCB, sizeof(TASK_INFO));
		if (!pTCB)
			return ERR_NOMEMORY;

		kInitialiseTCBVariables (pTCB, 16384 , "USER");

		ERROR_CODE code = load_bflat ((const char *)nParam1, pTCB);

		API_MALLOC (&pTCB->pStack, 16384);//nStackSize

		unsigned char* pTopOfStack = (unsigned char*)pTCB->pStack;
		pTopOfStack += pTCB->nStackSize - 4;
		pTCB->pTopOfStack = kInitialiseStack ((unsigned long*)pTopOfStack, pTCB->pEntry, 0);
		API_MALLOC (&pTCB->pMessagePipe, sizeof(PIPE));
		pTCB->pMessagePipe->nReceiver = 0;
		pTCB->pMessagePipe->nSender = 0;

                *((TASK_INFO**)nParam2) = pTCB;

		printk ("TASK READY TO START. INCLUDING IN TASK RING...\n");

		// Include new task in task ring...
		__cli ();
		pTCB->pPrevTask = g_pTaskRing;
		pTCB->pNextTask = g_pTaskRing->pNextTask;
		pTCB->pNextTask->pPrevTask = pTCB;
		g_pTaskRing->pNextTask = pTCB;
		__sti ();

                
		return code;
	}
	break;

        case nAPI_GFX:
            return gfx_swi_handler((int)nParam1,(GFX_DATA *)nParam2, (void *)nParam3);

        case nAPI_PRINTF:
            user_printf((const char *)nParam1, (va_list) nParam2);
            return 0;

        case nAPI_FILE:
            return fs_swi((int)nParam1,(void *)nParam2, (void *)nParam3);
        
        case nAPI_MIXER:
            mixer_ctl((int)nParam1,(int)nParam2,(void *)nParam3);
            return 0;
        
        case nAPI_DSP:
            dsp_ctl((int)nParam1,(void *)nParam2);
            return 0;
        
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
         return 0;
   }

   return 0;
}
