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

__IRAM_DATA MEMORY_CONTEXT g_MainMemoryContext;

__IRAM_CODE void init_malloc (long beg, long size)
{
	init_memory_context (&g_MainMemoryContext);
	bpool (&g_MainMemoryContext, beg, size);
	printk("[init] malloc\n");
}

__IRAM_CODE void* kmalloc (int nBytes)
{
	void* ptr = (void*)bget (&g_MainMemoryContext, nBytes);
	return ptr;
}

__IRAM_CODE void kfree (void* ptr)
{
	brel (&g_MainMemoryContext, ptr);
}

__IRAM_CODE unsigned long kmemavail ()
{
   long nAllocatedBytes, nFreeBytes, nMaxFree, nAllocCount, nFreeCount;
   bstats (&g_MainMemoryContext, &nAllocatedBytes, &nFreeBytes, &nMaxFree, &nAllocCount, &nFreeCount);
   return nFreeBytes;
}

// Memory manager runs as separate thread
// This thread serializes calls to memory allocation/free routines
// Made from diffrent threads.
__IRAM_CODE int kmemory_manager (void* pvParameters)
{
	register long _r7 asm("r7");
	register long _r8 asm("r8");
	register long _r12 asm("r12");
	register long _r13 asm("r13");
	asm volatile ( "mrs r7, cpsr");
	asm volatile ( "mov r8, lr");
	__cli ();
	printk("*** kmemory_manager *** [CPSR:%08x, R12:%08x, SP:%08x, LR:%08x]\n", _r7, _r12, _r13, _r8);
	__sti ();

   SYSTEM_CTRL_COMMAND* pSysCtrl = 0;
   TASK_INFO* pTCB = 0;

   while (1)
   {
      pSysCtrl = 0;
      pTCB = 0;

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

               printk ("[memmgr] TERMINATE\n");

               pTCB = pSysCtrl->pSenderThread;

               // Remove task from task ring...
               __cli ();
               pTCB->pPrevTask->pNextTask = pTCB->pNextTask;
               pTCB->pNextTask->pPrevTask = pTCB->pPrevTask;
               __sti ();

               // Free task memory resources...
               if (pTCB->pTaskCode)
                  kfree (pTCB->pTaskCode); //code memory
               if (pTCB->pMessagePipe)
                  kfree (pTCB->pMessagePipe); //message pipe
               kfree (pTCB->pStack); //stack
               kfree (pTCB); //TCB record
               API_TASK_YIELD ();

               // We should never get here...
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

	__cli ();
	printk ("*** MEMORY MANAGER EXITED ***\n");
	__sti ();
	return 0;
}
