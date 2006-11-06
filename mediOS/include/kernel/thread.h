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

#define THREAD_NO_FORCE  0
#define THREAD_FORCE     1

#define THREAD_DISABLE  0
#define THREAD_ENABLE   1

#define MEM_RESSOURCE 0
#define THREAD_NB_RES 1

typedef struct thread_list {
    struct thread_list * nxt;
    struct thread_list * prev;
    int pid;
} THREAD_LIST;

typedef struct thread_ressource {
    void(*clean_fct)(void*ptr);
    THREAD_LIST * head_list;
    int offset;
    void(*print_fct)(void*ptr);
} THREAD_RES;

extern THREAD_RES ini_ressources[THREAD_NB_RES];

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
    
    /* ressource */
    THREAD_RES ressources[THREAD_NB_RES];
    
} THREAD_INFO;

extern THREAD_INFO * threadCurrent;

#define THREAD_LINKS THREAD_LIST __thread_list;
#define THREAD_PTR_2_LIST(PTR)     (&PTR->__thread_list)

void thread_listIni(THREAD_LIST * ptr);
MED_RET_T thread_listRm(THREAD_LIST * ptr,int res_id,int force);
void thread_listAdd(THREAD_LIST * ptr,int res_id,int force);

#define THREAD_INIT_RES(TYPE,FCT,ID,PRINT) { \
    ini_ressources[ID].clean_fct=(void(*)(void*))FCT;  \
    ini_ressources[ID].head_list=NULL; \
    ini_ressources[ID].offset = (unsigned long)(&(((TYPE*)0)->__thread_list)); \
    ini_ressources[ID].print_fct=(void(*)(void*))PRINT; \
}

void thread_listFree(THREAD_INFO * thread,int res_id);

MED_RET_T thread_init(void(*)(void));
void thread_loadContext(void);

void thread_nxt(void);

MED_RET_T thread_enable(int pid);
MED_RET_T thread_disable(int pid);

unsigned long yield(void);

void thread_startMed(void * entry_fct,void * code_malloc,char * name,int argc,char ** argv);
int thread_startFct(THREAD_INFO ** ret_thread,void * entry_fct,char * name,int enable);

int thread_create(THREAD_INFO ** ret_thread,void * entry_fct,void * exit_fct,
    void * code_malloc,void * stack_top,int stack_size,char * name,unsigned long arg1,unsigned long arg2);
MED_RET_T thread_insert(THREAD_INFO * thread);
MED_RET_T thread_remove(THREAD_INFO * thread);
MED_RET_T thread_kill(int pid);
void thread_doKill(THREAD_INFO * thread);
THREAD_INFO * thread_findPid(int pid);
void thread_medExit(void);

void thread_listPrintAll(int res_id);
void thread_listPrintPtr(int res_id,THREAD_INFO * thread);
void thread_ps(void);

#endif
