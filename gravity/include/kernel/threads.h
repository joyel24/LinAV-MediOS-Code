/* 
*   include/threads.h
*
*   AMOS project
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __THREADS_H
#define __THREADS_H

#include <types.h>
#include <kernel/errors.h>
#include <kernel/pipes.h>
#include <kernel/kfont.h>

#define TASK_STACK_SIZE 16384

typedef struct _TASK_INFO
{
	unsigned long*     pTopOfStack;
	unsigned long*     pStack;
	unsigned long      nStackSize;
	unsigned long      nTicks;
	struct _TASK_INFO* pNextTask;
	struct _TASK_INFO* pPrevTask;
	void*              pTaskCode;
	void*              pEntry;
	unsigned long      nBlockingState;
	unsigned long      nBlockingValue;
	PIPE*              pMessagePipe;
	GFX_CONTEXT*       pMemoryContext;
	GFX_CONTEXT*       pDrawingContext;
	char               cName [8];
	unsigned long      nPriority;
	unsigned long      nCurrentScore;
	FONT_HEADER*       pFont;
	short*             pRegionLeft;
	short*             pRegionRight;
	COLOR              nFontColor;
} TASK_INFO;

typedef struct _CRITSEC_INFO
{
	TASK_INFO* pOwnerTask;
	long nLockCounter;
} CRITSEC_INFO;

typedef struct _KERNEL_OBJECT_INFO
{
	void* pObject;
} KERNEL_OBJECT_INFO;

typedef struct _SYSTEM_CTRL_COMMAND
{
	unsigned long nCmdId;
	unsigned long nCmdParam1;
	unsigned long nCmdParam2;
	TASK_INFO* pSenderThread;
} SYSTEM_CTRL_COMMAND;

#define TASK_BLOCKED_BY_NONE     0
#define TASK_BLOCKED_BY_SLEEP    1
#define TASK_BLOCKED_BY_PIPE     2
#define TASK_BLOCKED_BY_MUTEX    3
#define TASK_BLOCKED_BY_MEMMGR   4
#define TASK_BLOCKED_BY_SUSPEND  5
#define TASK_BLOCKED_BY_DEBUGGER 6
#define TASK_BLOCKED_BY_GFXMGR   7
#define TASK_BLOCKED_BY_ATA      8

extern TASK_INFO* g_pTaskRing; // pointer to current element in ring list

extern KERNEL_ERROR_CODE kinit_tcb ();
extern TASK_INFO* kcreate_tcb (void* pvTaskCode, unsigned long nStackSize, void* pParams, const char* pszTaskName);

// Internal kernel functions to work with ring task lists:

extern int klist_size (TASK_INFO* pList);
extern KERNEL_ERROR_CODE kadd_tcb (TASK_INFO** pList, TASK_INFO* pTask);
extern TASK_INFO* kremove_tcb  (TASK_INFO** pList);
unsigned long* kInitialiseStack (unsigned long* pxTopOfStack, void* pvCode, void *pvParameters);
void kInitialiseTCBVariables (TASK_INFO* pTCB, unsigned long nStackSize, const char* pszTaskName);
#endif
