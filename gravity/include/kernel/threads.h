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

#include <kernel/errors.h>

#define TASK_STACK_SIZE 16384

typedef struct _TASK_INFO
{
	unsigned long*     pTopOfStack;
	unsigned long*     pStack;
	unsigned long      nStackSize;
	unsigned long      nTicks;
	unsigned long      nActivationTime;
	struct _TASK_INFO* pNextTask;
	struct _TASK_INFO* pPrevTask;
	void*              pBlocker;
	void*              pBlockerParameter;
	unsigned long      nActiveBackColor;
	unsigned long      nActiveForeColor;
	void*              pFont;
	char               cName [8];
} TASK_INFO;

typedef struct _CRITSEC_INFO
{
	unsigned long nBlocked;
} CRITSEC_INFO;

typedef struct _SYSTEM_CTRL_COMMAND
{
	unsigned long nCmdId;
	unsigned long nCmdParam1;
	unsigned long nCmdParam2;
	TASK_INFO* pSenderThread;
} SYSTEM_CTRL_COMMAND;

#define KERNEL_CMD_NONE      0
#define KERNEL_CMD_SLEEP     1
#define KERNEL_CMD_TERMINATE 2
#define KERNEL_CMD_BLOCK     3
#define KERNEL_CMD_SUSPEND   4
#define KERNEL_CMD_CONTINUE  5

extern TASK_INFO* g_pActiveTask; // pointer to current element in ring list
extern TASK_INFO* g_pBlockedTask; // pointer to current element in ring list

#define kload_context()													\
{																		\
	/* Set the LR to the task stack. */									\
	asm volatile ( "LDR		R0, %0" : : "m" (g_pActiveTask) );			\
	asm volatile ( "LDR		LR, [R0]" );								\
																		\
	/* Get the SPSR from the stack. */									\
	asm volatile ( "LDMFD	LR!, {R0}" );								\
	asm volatile ( "MSR		SPSR, R0" );								\
																		\
	/* Restore all system mode registers for the task. */				\
	asm volatile ( "LDMFD	LR, {R0-R14}^" );							\
	asm volatile ( "NOP" );												\
																		\
	/* And return - correcting the offset in the LR to obtain the */	\
	/* correct address. */												\
	asm volatile ( "SUBS	PC, LR, #4" );								\
}

extern KERNEL_ERROR_CODE kinit_tcb ();
extern TASK_INFO* kcreate_tcb (void* pvTaskCode, unsigned long nStackSize, void* pParams, const char* pszTaskName);

// Internal kernel functions to work with ring task lists:

extern int klist_size (TASK_INFO* pList);
extern KERNEL_ERROR_CODE kadd_tcb (TASK_INFO** pList, TASK_INFO* pTask);
extern TASK_INFO* kremove_tcb  (TASK_INFO** pList);
extern void kremove_tcb_ex  (TASK_INFO** pList, TASK_INFO* pTask);

#endif
