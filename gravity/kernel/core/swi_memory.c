/* 
*   kernel/swi_memory.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/kernel.h>
#include <kernel/swi.h>
#include <kernel/threads.h>

__IRAM_CODE int swi_memory_handler (
	unsigned long nCmd,
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3)
{
	switch (nCmd)
	{
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

	case nAPI_MEMSET: //(void* pvBuffer, int fill, size_t size);
	{
		int i;
		unsigned char* pDst = (unsigned char*)nParam1;
		unsigned char fill = nParam2;
		for (i=0;i<nParam3;i++)
			pDst[i] = fill;
	}
	break;

	case nAPI_MEMCPY: //(void* pvBuffer, const void* pvSrc, size_t size);
	{
		int i;
		unsigned char* pDst = (unsigned char*)nParam1;
		unsigned char* pSrc = (unsigned char*)nParam2;
		for (i=0;i<nParam3;i++)
			pDst[i] = pSrc[i];
	}
	break;

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
	}

	return 0;
}
