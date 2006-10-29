/*
*   include/kernel/thread.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __THREAD_H
#define __THREAD_H

#include <kernel/errors.h>

#define THREAD_NAME_SIZE  15
#define STACK_SIZE        0x4000
#define THREAD_INIT_CPSR  0x1F


typedef struct thread_info {
    unsigned long regs[17];
    unsigned int * stackMalloc; /* NULL if stack is not malloc eg stack in iram*/
    unsigned int stackSize;
    unsigned int * codeMalloc; /* NULL if code is part of medios, !NULL for med*/
    /* thread info */
    char name[THREAD_NAME_SIZE];
    int pid;
    
    /* flags */
    int enable;
    
    /* linkage */
    struct thread_info * nxt;
    struct thread_info * prev;
} THREAD_INFO;

MED_RET_T thread_init(void(*)(void));
void thread_loadContext(void);
void thread_print(void);
void thread_nxt(void);

unsigned long yield(void);

void thread_startMed(void * entry_fct,void * code_malloc,char * name,int argc,char ** argv);

int thread_create(THREAD_INFO ** ret_thread,void * entry_fct,void * exit_fct,
    void * code_malloc,void * stack_top,int stack_size,char * name,unsigned long arg1,unsigned long arg2);
MED_RET_T thread_insert(THREAD_INFO * thread);
MED_RET_T thread_remove(THREAD_INFO * thread);
MED_RET_T thread_kill(int pid);
void thread_doKill(THREAD_INFO * thread);
THREAD_INFO * thread_findPid(int pid);
void thread_medExit(void);

#endif
