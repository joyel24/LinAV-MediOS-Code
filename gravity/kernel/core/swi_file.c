/* 
*   kernel/swi_file.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/kernel.h>
#include <kernel/swi.h>
#include <kernel/threads.h>

__IRAM_CODE int swi_file_handler (
	unsigned long nCmd,
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3)
{
	switch (nCmd)
	{
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

        case nAPI_FILE:
            return fs_swi((int)nParam1,(void *)nParam2, (void *)nParam3);
	}

	return 0;
}
