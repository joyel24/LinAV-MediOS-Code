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

#define THREAD_NAME_SIZE  10
#define STACK_SIZE        0x4000
#define THREAD_INIT_CPSR  0x10
#define THREAD_SYSINIT_CPSR 0x1F

typedef struct thread_info {
    unsigned long regs[17];
    unsigned int * stackMalloc;
    unsigned int stackSize;
    char name[THREAD_NAME_SIZE];

    /* linkage */
    struct thread_info * nxt;
    struct thread_info * prev;
} THREAD_INFO;

MED_RET_T thread_init(void(*)(void));
void thread_loadContext(void);

MED_RET_T thread_create(THREAD_INFO ** ret_thread,void * entry,char * name);
MED_RET_T thread_insert(THREAD_INFO * thread);
MED_RET_T thread_remove(THREAD_INFO * thread);

#endif
