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
#include <sys_def/malloc.h>

#include <kernel/kernel.h>
#include <kernel/threads.h>
#include <kernel/errors.h>
#include <kernel/irq.h>

//TASK_INFO* g_pActiveTask  __IRAM_DATA = 0; // pointer to current element in ring list
TASK_INFO* g_pBlockedTask __IRAM_DATA = 0; // pointer to current element in ring list

__IRAM_CODE void* kmalloc (int nBytes)
{
	void* ptr = (void*)bget (nBytes);
	return ptr;
}

__IRAM_CODE void kfree (void* ptr)
{
	brel (ptr);
}

__IRAM_CODE KERNEL_ERROR_CODE kinit_tcb ()
{
	g_pActiveTask  = 0;
	g_pBlockedTask = 0;
	return eOK;
}

__IRAM_CODE void kthread_final_trap (int nRetCode)
{
   cli();
   printk("THREAD FINAL TRAP. RETCODE: %08x\n", nRetCode);
   sti();

   //TODO: Here we should delete thread control block and free resources...
   while (1);
}

__IRAM_CODE void kInitialiseTCBVariables (TASK_INFO* pTCB, unsigned long nStackSize, const char* pszTaskName)
{
	pTCB->nStackSize = nStackSize;
	pTCB->pPrevTask  = 0;
	pTCB->pNextTask  = 0;
	pTCB->nTicks     = 0;
	pTCB->pBlocker   = 0;
	pTCB->pBlockerParameter = 0;
//	pTCB->nActiveBackColor = COLOR_BLACK;//0x0000;
//	pTCB->nActiveForeColor = COLOR_WHITE;//0xFFFF;
	pTCB->nActivationTime = 0;
	char* pszTarget = pTCB->cName;
	while (*pszTaskName)
		*pszTarget++ = *pszTaskName++;
	*pszTarget++ = *pszTaskName++;
}

__IRAM_CODE unsigned long* kInitialiseStack (unsigned long* pxTopOfStack, void* pvCode, void *pvParameters)
{
	unsigned long *pxOriginalTOS = pxTopOfStack;

   //TODO: Here we need to load "kthread_final_trap" address to enable thread safe termination...
   *pxTopOfStack = (unsigned long)kthread_final_trap;
   pxTopOfStack--;

	*pxTopOfStack = (unsigned long)pvCode + 4;//portINSTRUCTION_SIZE;
	pxTopOfStack--;

	*pxTopOfStack = (unsigned long) pxOriginalTOS; /* Stack used when task starts goes in R13. */
	pxTopOfStack -= 13;
	*pxTopOfStack = (unsigned long) pvParameters; // R0
	pxTopOfStack--;

	*pxTopOfStack = (unsigned long) 0x1F; // System mode, ARM mode, interrupts enabled

	return pxTopOfStack;
}

__IRAM_CODE TASK_INFO* kAllocateTCBAndStack (unsigned long nStackSize)
{
	TASK_INFO* pNewTCB = (TASK_INFO*)kmalloc(sizeof(TASK_INFO));
	if (pNewTCB)
		pNewTCB->pStack = (unsigned long*)kmalloc(nStackSize);
	return pNewTCB;
}

__IRAM_CODE TASK_INFO* kcreate_tcb (void* pvTaskCode, unsigned long nStackSize, void* pParams, const char* pszTaskName)
{
	TASK_INFO* pTCB = kAllocateTCBAndStack (nStackSize);
	if (pTCB)
	{
		kInitialiseTCBVariables (pTCB, nStackSize, pszTaskName);
		unsigned char* pTopOfStack = (unsigned char*)pTCB->pStack;
		pTopOfStack += pTCB->nStackSize - 4;
		pTCB->pTopOfStack = kInitialiseStack ((unsigned long*)pTopOfStack, pvTaskCode, pParams);
	}

	return pTCB;
}

__IRAM_CODE int klist_size (TASK_INFO* pList)
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

__IRAM_CODE KERNEL_ERROR_CODE kadd_tcb (TASK_INFO** pList, TASK_INFO* pTask)
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

__IRAM_CODE TASK_INFO* kremove_tcb  (TASK_INFO** pList)
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
    g_pActiveTask = g_pActiveTask->pNextTask;

//    printk("CURRENT TCB: %s\n", g_pActiveTask->cName);

//    asm volatile ("MSR CPSR_c, #0x92");
}
