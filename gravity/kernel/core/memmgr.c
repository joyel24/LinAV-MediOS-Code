/* 
*   kernel/memmgr.c
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
#include <kernel/irq.h>
#include <kernel/swi.h>

__IRAM_CODE void* kmalloc (int nBytes)
{
	void* ptr = (void*)bget (nBytes);
	return ptr;
}

__IRAM_CODE void kfree (void* ptr)
{
	brel (ptr);
}

__IRAM_CODE unsigned long kmemavail ()
{
   long nAllocatedBytes, nFreeBytes, nMaxFree, nAllocCount, nFreeCount;
   bstats (&nAllocatedBytes, &nFreeBytes, &nMaxFree, &nAllocCount, &nFreeCount);
   return nFreeBytes;
}

// Memory manager runs as separate thread
// This thread serializes calls to memory allocation/free routines
// Made from diffrent threads.
__IRAM_CODE int kmemory_manager (void* pvParameters)
{
/*
   while (1)
   {
      API_TASK_YIELD ();
   };
*/

   while (1)
   {
      SYSTEM_CTRL_COMMAND* pSysCtrl = 0;
      __cli ();
      if (g_pSystemCtrlPipe->nReceiver != g_pSystemCtrlPipe->nSender)
      {
         pSysCtrl = (SYSTEM_CTRL_COMMAND*)(g_pSystemCtrlPipe->buffer + g_pSystemCtrlPipe->nReceiver);
         g_pSystemCtrlPipe->nReceiver = (g_pSystemCtrlPipe->nReceiver + sizeof(SYSTEM_CTRL_COMMAND)) & PIPE_SIZE_MASK;
      };
      __sti ();

      if (pSysCtrl)
      {
         switch (pSysCtrl->nCmdId)
         {
            case nAPI_MALLOC:
               *((void**)pSysCtrl->nCmdParam1) = kmalloc (pSysCtrl->nCmdParam2);
               break;
            case nAPI_FREE:
               kfree ((void*)pSysCtrl->nCmdParam1);
               break;
            case nAPI_MEMAVAIL:
               *((unsigned long*)pSysCtrl->nCmdParam1) = kmemavail ();
               break;
            case nAPI_TASK_TERMINATE:
               // TO DO:
               break;
         }
      }

      /// Unblock calling task...
      __cli ();
      pSysCtrl->pSenderThread->nBlockingState = TASK_BLOCKED_BY_NONE;
      pSysCtrl->pSenderThread->nBlockingValue = 0;

      API_TASK_YIELD ();
      __sti ();
   };

   return 0;
}
