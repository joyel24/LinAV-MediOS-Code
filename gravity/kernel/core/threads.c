/* 
*   kernel/threads.c
*
*   AMOS project
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>
#include <kernel/malloc.h>

#include <kernel/kernel.h>
#include <kernel/threads.h>
#include <kernel/pipes.h>
#include <kernel/errors.h>
#include <kernel/irq.h>
#include <api.h>

KERNEL_ERROR_CODE kinit_tcb ()
{
	g_pTaskRing = 0;

	g_pKernelCtrlPipe = kmalloc (sizeof(PIPE));
	g_pKernelCtrlPipe->nReceiver = 0;
	g_pKernelCtrlPipe->nSender = 0;

	g_pSystemCtrlPipe = kmalloc (sizeof(PIPE));
	g_pSystemCtrlPipe->nReceiver = 0;
	g_pSystemCtrlPipe->nSender = 0;

	return eOK;
}

void kthread_final_trap (int nRetCode)
{
	TASK_INFO* pTCB;
	__cli ();
	pTCB = g_pTaskRing;
	__sti ();

	__cli ();
	printk("%s[%08x]: THREAD FINAL TRAP. RETCODE: %08x\n", pTCB->cName, pTCB, nRetCode);
	__sti ();

	register long _r12 asm("r12");
	register long _r13 asm("r13");
	register long _lr asm("lr");
	__cli ();
	printk("%s[%08x]: REGISTERS: r12=%08x r13=%08x lr=%08x\n", pTCB->cName, pTCB, _r12, _r13, _lr);
	__sti ();

	API_TASK_TERMINATE ();
}

void kInitialiseTCBVariables (TASK_INFO* pTCB, unsigned long nStackSize, const char* pszTaskName)
{
	pTCB->nStackSize = nStackSize;
	pTCB->pPrevTask  = 0;
	pTCB->pNextTask  = 0;
	pTCB->nTicks     = 0;

	pTCB->nBlockingState = TASK_BLOCKED_BY_NONE;
	pTCB->nBlockingValue = 0;

	char* pszTarget = pTCB->cName;
	while (*pszTaskName)
		*pszTarget++ = *pszTaskName++;
	*pszTarget++ = *pszTaskName++;

	pTCB->pMessagePipe    = 0;
	pTCB->pMemoryContext  = 0;
	pTCB->pDrawingContext = 0;
	pTCB->pTaskCode       = 0;
	pTCB->pEntry          = 0;
	pTCB->nPriority       = 10;
	pTCB->nCurrentScore   = 10;
	pTCB->pFont           = 0;
	pTCB->pRegionLeft     = 0;
	pTCB->pRegionRight    = 0;
}

unsigned long* kInitialiseStack (unsigned long* pxTopOfStack, void* pvCode, void *pvParameters)
{
	printk ("TCB stack set to %08x\n", pxTopOfStack);

	pxTopOfStack --;

	// store [pc]
	*pxTopOfStack = (unsigned long)pvCode;
	pxTopOfStack--;
	// store [lr]
	*pxTopOfStack = (unsigned long)kthread_final_trap;
	pxTopOfStack --;
	// store [r12]
	*pxTopOfStack = (unsigned long)0x12121212;
	pxTopOfStack -= 12;
	// store [r0-r12]
	*pxTopOfStack = (unsigned long)pvParameters; // R0
	pxTopOfStack--;
	// store [spsr]
	*pxTopOfStack = (unsigned long)0x1F;
	return pxTopOfStack;
}

TASK_INFO* kAllocateTCBAndStack (unsigned long nStackSize)
{
	TASK_INFO* pNewTCB = (TASK_INFO*)kmalloc(sizeof(TASK_INFO));
	if (pNewTCB)
		pNewTCB->pStack = (unsigned long*)kmalloc(nStackSize);
	return pNewTCB;
}

TASK_INFO* kcreate_tcb (void* pvTaskCode, unsigned long nStackSize, void* pParams, const char* pszTaskName)
{
	TASK_INFO* pTCB = kAllocateTCBAndStack (nStackSize);
	if (!pTCB)
		return 0;

	kInitialiseTCBVariables (pTCB, nStackSize, pszTaskName);
	unsigned char* pTopOfStack = (unsigned char*)pTCB->pStack;
	pTopOfStack += pTCB->nStackSize - 4;
	pTCB->pTopOfStack = kInitialiseStack ((unsigned long*)pTopOfStack, pvTaskCode, pParams);

	pTCB->pMessagePipe = kmalloc (sizeof(PIPE));
	pTCB->pMessagePipe->nReceiver = 0;
	pTCB->pMessagePipe->nSender = 0;

	return pTCB;
}

int klist_size (TASK_INFO* pList)
{
	if (!pList)
		return 0;

	int nSize = 0;

	TASK_INFO* pCurr = pList;
	do
	{
		pCurr = pCurr->pNextTask;
		nSize ++;
	}
	while (pCurr != pList);

	return nSize;
}

KERNEL_ERROR_CODE kadd_tcb (TASK_INFO** pList, TASK_INFO* pTask)
{
	if (!pTask)
		return ePOINTER;

	if (!pList)
		return ePOINTER;

	if (!*pList)
	{
		*pList = pTask;
		(*pList)->pNextTask = (struct _TASK_INFO*)pTask;
		(*pList)->pPrevTask = (struct _TASK_INFO*)pTask;
		return eOK;
	}

	TASK_INFO* pNext = (*pList)->pNextTask;

	// Retrace forward links:
	(*pList)->pNextTask = pTask;
	pTask->pNextTask = pNext;

	// Retrace backward links:
	pNext->pPrevTask = pTask;
	pTask->pPrevTask = (struct _TASK_INFO*)(*pList);
	return eOK;
}

TASK_INFO* kremove_tcb  (TASK_INFO** pList)
{
	if (!pList)
		return 0;

	TASK_INFO* pDeleted = *pList;

	if (klist_size (*pList) == 1)
	{
		pDeleted->pNextTask = 0;
		pDeleted->pPrevTask = 0;
		*pList = 0;
		return pDeleted;
	}

	TASK_INFO* pPrev = pDeleted->pPrevTask;
	TASK_INFO* pNext = pDeleted->pNextTask;

	pPrev->pNextTask = pNext;
	pNext->pPrevTask = pPrev;

	pDeleted->pNextTask = 0;
	pDeleted->pPrevTask = 0;

	*pList = pNext;

	return pDeleted;
}

__IRAM_CODE void kset_next_ready_task ()
{
	TASK_INFO* pStart = g_pTaskRing;
	TASK_INFO* pOptimalTask = 0;
	unsigned long nOptimalScore = 0;

	g_pTaskRing->nCurrentScore = g_pTaskRing->nPriority;

	do
	{
		g_pTaskRing = g_pTaskRing->pNextTask;

		switch (g_pTaskRing->nBlockingState)
		{
			case TASK_BLOCKED_BY_NONE:
				break;

			case TASK_BLOCKED_BY_SLEEP:
				if (tick > g_pTaskRing->nBlockingValue)
					g_pTaskRing->nBlockingState = TASK_BLOCKED_BY_NONE;
				break;

			case TASK_BLOCKED_BY_PIPE:
				{
					PIPE* pPipe = (PIPE*)g_pTaskRing->nBlockingValue;
					if (pPipe->nReceiver != pPipe->nSender)
						g_pTaskRing->nBlockingState = TASK_BLOCKED_BY_NONE;
				}
				break;

			case TASK_BLOCKED_BY_MUTEX:
				{
					CRITSEC_INFO* pCS = (CRITSEC_INFO*)g_pTaskRing->nBlockingValue;
					if (pCS->pOwnerTask == 0)
					{
						pCS->pOwnerTask = g_pTaskRing;
						g_pTaskRing->nBlockingState = TASK_BLOCKED_BY_NONE;
					}
				}
				break;

			case TASK_BLOCKED_BY_MEMMGR:
				break;
		}

		if (g_pTaskRing->nBlockingState == TASK_BLOCKED_BY_NONE)
		{
			if (g_pTaskRing->nCurrentScore > nOptimalScore)
			{
				nOptimalScore = g_pTaskRing->nCurrentScore;
				pOptimalTask = g_pTaskRing;
			}
			else
			{
				g_pTaskRing->nCurrentScore ++;
			}
		}
		else
		{
		}
	}
	while (pStart != g_pTaskRing);

	g_pTaskRing = pOptimalTask;
}
