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

#endif
