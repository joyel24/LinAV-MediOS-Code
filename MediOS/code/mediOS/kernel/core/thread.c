/*
*   kernel/core/thread.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
1*/

#include <kernel/kernel.h>
#include <kernel/errors.h>
#include <kernel/malloc.h>
#include <kernel/irq.h>

#include <sys_def/string.h>

#include <kernel/thread.h>
#include <kernel/swi.h>

#include <kernel/malloc.h>

#define SYS_STACK_TOP ((void*)IRAM_SIZE-SVC_STACK_SIZE)
#define SYS_STACK_BTM ((void*)((char*)&_iram_end + 0x10))

void thread_exit(void);

int pid;
THREAD_INFO * sysThread ;
THREAD_INFO * idleThread ;

THREAD_INFO *threadSysStack;

THREAD_RES ini_ressources[THREAD_NB_RES];

/***********************************
* Idle thread fct
***********************************/
void thread_idleFct(void)
{
    while(1)
    {
        yield();
    }
}

/***********************************
* Init of thread routines
* Create and start first thread: SYS/kernel
***********************************/
MED_RET_T thread_init(void(*fct)(void))
{
    int retval;

    /* global var */
    pid=0;
    threadCurrent = sysThread = idleThread = NULL;
    /* creating initial thread */

    retval=thread_create(&sysThread,(void*)fct,(void*)thread_exit,NULL,NULL,0,THREAD_USE_SYS_STACK,
        SYS_STACK_BTM,PRIO_HIGH,"KERNEL",(unsigned long)NULL,(unsigned long)NULL);

    if(retval<0)
    {
        printk("Error creating KERNEL thread (error code = %d\n",-retval);
        return retval;
    }
    else
        printk("KERNEL thread created with pid %d\n",retval);

    threadSysStack=sysThread;

    sysThread->enable=1;

    retval=thread_create(&idleThread,(void*)thread_idleFct,(void*)thread_exit,NULL,NULL,0x100,THREAD_USE_OTHER_STACK,
        NULL,PRIO_HIGH,"IDLE",(unsigned long)NULL,(unsigned long)NULL);
    if(retval<0)
    {
        printk("Error creating IDLE thread (error code = %d\n",-retval);
        return retval;
    }
    else
        printk("Idle thread created with pid %d\n",retval);

    /* idleThread is disable*/
    idleThread->enable=0;

    return MED_OK;
}

/***********************************************************************************
************************************************************************************
Thread creation
************************************************************************************
***********************************************************************************/

/***********************************
* Init of med thread
* Create struct
* disable KERNEL thread
* yield
***********************************/
void thread_startMed(void * entry_fct,void * code_malloc,void * iram_top,char * name,int argc,char ** argv)
{
    THREAD_INFO * med_thread;
    int pid=thread_create(&med_thread,entry_fct,(void*)thread_exit,code_malloc,NULL,0,THREAD_USE_SYS_STACK,
        iram_top,PRIO_HIGH,name,(unsigned long)argc,(unsigned long)argv);
    if(pid<0)
    {
        printk("Error starting med, threadStart return: %d\n",-pid);
        kfree(code_malloc);
    }
    __cli();
    sysThread->enable=0;
    med_thread->enable=1;
    yield();
    __sti();
}

/***********************************
* Init of simple thread
* Create struct
* "enable" is used to enable/disable
* thread once created
***********************************/
int thread_startFct(THREAD_INFO ** ret_thread,void * entry_fct,char * name,int enable,int prio)
{
    THREAD_INFO * fct_thread;
    int pid=thread_create(&fct_thread,entry_fct,(void*)thread_exit,0,NULL,0,THREAD_USE_OTHER_STACK,NULL,prio,name,
        0,(unsigned long)NULL);
    printk("Fct thread created with pid %d\n",pid);
    if(ret_thread)
        *ret_thread=fct_thread;
    fct_thread->enable=enable;
    return pid;
}

/***********************************
* Main create and init new thread
* also create its stack if needed
* on error returns a MED_RET <0
* returns PID otherwise
***********************************/
int thread_create(THREAD_INFO ** ret_thread,void * entry_fct,void * exit_fct,
    void * code_malloc,void * stack_top,unsigned int stack_size,int useSysStack,
    void * stack_bottom,int prio,char * name,unsigned long arg1,unsigned long arg2)
{
    int i;

    /* Malloc thread struct */
    THREAD_INFO * newThread = (THREAD_INFO*)kmalloc(sizeof(THREAD_INFO));
    if(!newThread)
    {
        printk("No mem left for task: %s\n",name!=NULL?name:"NO NAME");
        return -MED_ENOMEM;
    }
    /* Clear thread struct content */
    memset(newThread,0,sizeof(THREAD_INFO));
    /* init ressources array */
    for(i=0;i<THREAD_NB_RES;i++)
        memcpy(&newThread->ressources[i],&ini_ressources[i],sizeof(struct thread_ressource));

    newThread->useSysStack=useSysStack;
    newThread->priority=prio;
    newThread->idleCnt=0;

    /* Malloc stack */
    if(useSysStack)
    {
        stack_top=SYS_STACK_TOP;
        newThread->stackBottom=stack_bottom;
        stack_size=(unsigned int)(stack_top-stack_bottom);
        newThread->saveStack=kmalloc(stack_size);
        if(!newThread->saveStack)
        {
            printk("No mem left for stack save (needs %x,top=%x,btm=%x)\n",stack_size,stack_top,stack_bottom);
            free(newThread);
            return -MED_ENOMEM;
        }
        printk("Save stack malloc, size = %x\n",stack_size);
    }
    else
    {
        if(!stack_top)
        {
            if(stack_size==0)
                stack_size=STACK_SIZE;
            newThread->stackBottom=(unsigned int *)kmalloc(stack_size);
            stack_top = (void*)(((unsigned long)(newThread->stackBottom))+stack_size-4);
            if(!newThread->stackBottom)
            {
                printk("No mem left for task's stack: %s\n",name!=NULL?name:"NO NAME");
                free(newThread);
                return -MED_ENOMEM;
            }
        }
        else
        {
            stack_size=0;
        }
    }
    /* Init thread struct data */
/* name */
    strncpy(newThread->name,name,THREAD_NAME_SIZE);
    newThread->name[THREAD_NAME_SIZE-1]=0;
/* flag / data */
    newThread->pid=pid++;
    newThread->enable = 0;
    newThread->codeMalloc=code_malloc;
/* stack */
    newThread->stackSize=stack_size;
/* regs */
    newThread->regs[0]=(unsigned long)THREAD_INIT_CPSR; //CPSR
    newThread->regs[1]=arg1;  /*R0 = 1st param */
    newThread->regs[2]=arg2;  /*R1 = 2nd param */
    /* dummy init of reg : reg[i]=iiii*/
    for(i=2;i<=12;i++)
        newThread->regs[i+1]=(unsigned long)(i|(i<<8)|(i<<16)|(i<<24));

    newThread->regs[14]=(unsigned long)stack_top;//SP
    newThread->regs[15]=(unsigned long)exit_fct; //LR=thread exit trap
    newThread->regs[16]=(unsigned long)entry_fct; //PC=entry of thread

    thread_insert(newThread);

    printk("new thread created at %x, stack malloc at %x (top=%x), entry=%x exit=%x mode=%x\n",
        newThread,newThread->stackBottom,newThread->regs[14],
        (unsigned long)entry_fct,(unsigned long)exit_fct,THREAD_INIT_CPSR);

    if(ret_thread)
        *ret_thread=newThread;

    return newThread->pid;
}

/***********************************************************************************
************************************************************************************
Thread ring ins/rm
************************************************************************************
***********************************************************************************/

/***********************************
* add thread from thread list
***********************************/
MED_RET_T thread_insert(THREAD_INFO * thread)
{
    THREAD_INFO * nxt;
    int needIrqEnable=0;

    if(!thread)
        return -MED_EINVAL;

    if(irq_globalEnabled())
    {
        __cli();
        needIrqEnable=1;
    }

    if(!threadCurrent)
    {
        threadCurrent=thread;
        thread->nxt=thread;
        thread->prev=thread;
    }
    else
    {
        nxt=threadCurrent->nxt;
        threadCurrent->nxt=thread;
        thread->nxt=nxt;
        nxt->prev=thread;
        thread->prev=threadCurrent;
    }

    if(needIrqEnable)
        __sti();

    return MED_OK;
}

/***********************************
* Remove thread from thread list
***********************************/
MED_RET_T thread_remove(THREAD_INFO * thread)
{
    if(!threadCurrent)
    {
        return -MED_EINVAL;
    }

    /* only one thread left */
    if(threadCurrent && threadCurrent->nxt==threadCurrent)
    {
        thread->nxt=NULL;
        thread->prev=NULL;
        threadCurrent=idleThread;
        idleThread->enable=1;
    }
    else
    {
        if(threadCurrent==thread)
            threadCurrent=thread->nxt;
        thread->prev->nxt=thread->nxt;
        thread->nxt->prev=thread->prev;
        thread->prev=NULL;
        thread->nxt=NULL;
    }

    return MED_OK;

}

/***********************************************************************************
************************************************************************************
Exit/Kill functions
************************************************************************************
***********************************************************************************/

/***********************************
* Common exit function of thread
* when a thread starts its return
* address is this function
***********************************/
void thread_exit(void)
{
   printk("Exit from %s\n",threadCurrent->name!=NULL?threadCurrent->name:"NO NAME");
   thread_doKill(threadCurrent);
}

/***********************************
* Kills a thread from its PID
***********************************/
MED_RET_T thread_kill(int pid)
{
    /* searching the PID */
    THREAD_INFO * ptr = thread_findPid(pid);
    if(!ptr)
        return -MED_ENOENT;
    thread_doKill(ptr);
    /* never return here */
    return MED_OK;
}

/***********************************
* Kills a thread from the pointeur
* to thread struct
***********************************/
int thread_doKill(THREAD_INFO * thread)
{
    THREAD_INFO * ptr;
    int i;
    if(thread == idleThread)
    {
        printk("Error trying to kill idle thread\n");
        return 0;
    }
    __cli();
    ptr=threadCurrent;
    thread_remove(thread);

    for(i=0;i<THREAD_NB_RES;i++)
        thread_listFree(thread,i);

    if(thread->stackBottom && !thread->useSysStack)
        kfree(thread->stackBottom);
    if(thread->useSysStack)
        kfree(thread->saveStack);
    if(thread->codeMalloc)
    {
        kfree(thread->codeMalloc);
        sysThread->enable=1; /* be sure to enable KERNEL thread */
        threadSysStack = NULL;
    }
    kfree(thread);
    if(thread==ptr)
    {
        swi_call(nTHREAD_NXT);
    }
    return 0; /* dummy return only here because swi_cause is a macro with a return*/
}

/***********************************************************************************
************************************************************************************
State related functions
************************************************************************************
***********************************************************************************/

/***********************************
* Enable a thread
***********************************/
MED_RET_T thread_enable(int pid)
{
    THREAD_INFO * ptr=thread_findPid(pid);
    if(!ptr)
    {
        printk("Can't find thread %d\n",pid);
        return -MED_ENOENT;
    }
    __cli();
    ptr->enable=1;
    __sti();
    return MED_OK;
}

/***********************************
* Disable a thread
***********************************/
MED_RET_T thread_disable(int pid)
{
    THREAD_INFO * ptr=thread_findPid(pid);
    if(!ptr)
    {
        printk("Can't find thread %d\n",pid);
        return -MED_ENOENT;
    }
    __cli();
    ptr->enable=0;
    __sti();
    return MED_OK;
}

/***********************************
* Set priority of a thread
***********************************/
MED_RET_T thread_nice(THREAD_INFO * ptr,int prio)
{
    if(prio>=0 && prio <= THREAD_MAX_PRIO)
        ptr->priority=prio;
    else
        return -MED_EINVAL;
    return MED_OK;
}

/***********************************************************************************
************************************************************************************
Context switch
************************************************************************************
***********************************************************************************/

/***********************************
* Standard yield
* !!uses swi
***********************************/
unsigned long yield(void)
{
    swi_call(nYIELD);
}

/***********************************
* Choose the next thread to run
***********************************/
__IRAM_CODE void thread_nxt(void)
{
    THREAD_INFO * ptr=threadCurrent;
    int topScore=0;
    THREAD_INFO * topThread=NULL;
    int has_top=0;

    idleThread->enable=0;

    if(threadCurrent==NULL)
    {
        printk("No thread in ring\n");
        printk("Let's loop\n");
        while(1)/*NOTHING*/;
    }

    threadCurrent->idleCnt = 0;

/*****************************************
Prio system: process the whole task ring
Consider only enable thread
-> nxt thread is the one with highest score: current formula: idleCnt-Prio
    * first: run thread that has wait enough
    * second: in thread that could still wait take the one closer to its allowed wait
-> all enale thread get idleCnt incremented
-> nxt thread get tickCnt incremented
RQ: idleThread never considered as it is always disable here

******************************************/
    do
    {
        if(ptr->enable)
        {
            if(!has_top)
            {
                has_top=1;
                topScore=COMPUTE_SCORE(ptr);
                topThread=ptr;
            }
            else
            {
                if(topScore<=COMPUTE_SCORE(ptr))
                {
                    topScore=COMPUTE_SCORE(ptr);
                    topThread=ptr;
                }
            }
            ptr->idleCnt++;
        }
        ptr=ptr->nxt;
    } while(ptr != threadCurrent);

    if(!has_top)   /* no thread found */
    {
        threadCurrent = idleThread;
        idleThread->enable=1;
        idleThread->tickCnt++;
    }
    else
    {
        topThread->tickCnt++;
        threadCurrent = topThread;
    }

    if(threadSysStack != threadCurrent && threadSysStack->useSysStack == 1 && threadCurrent->useSysStack == 1)
    {
        /* saving prev sys stack */
        memcpy(threadSysStack->saveStack,threadSysStack->stackBottom,threadSysStack->stackSize);
        memcpy(threadCurrent->stackBottom,threadCurrent->saveStack,threadCurrent->stackSize);
        threadSysStack=threadCurrent;
    }

    if(threadSysStack==NULL && threadCurrent->useSysStack == 1)
    {
        memcpy(threadCurrent->stackBottom,threadCurrent->saveStack,threadCurrent->stackSize);
        threadSysStack=threadCurrent;
    }

}

/***********************************************************************************
************************************************************************************
Ressources functions
************************************************************************************
***********************************************************************************/

/***********************************
* Init of a dummy ressource item
* defaulting pid to -1
***********************************/
void thread_listIni(THREAD_LIST * ptr)
{
    ptr->nxt=NULL;
    ptr->prev=NULL;
    ptr->pid=-1;
}

/***********************************
* Remove ressource item from its list
* trying curThread or PID thread
***********************************/
MED_RET_T thread_listRm(THREAD_LIST * ptr,int res_id,int force)
{
    if(ptr->pid == -1) /* is it a dummy item ? */
    {
        if(!threadCurrent)
        {
            /* nothing to do as no thread exist */
            return MED_OK;
        }

        if( threadCurrent->pid == 0 || force)
        {
            /* ok KERNEL thread is allowed to do this */
            return MED_OK;
        }
        else
        {
            /* error as we are not SYS or trying to force */
            return -MED_EINVAL;
        }
    }

    if(threadCurrent)
    {
        /* we have a current thread */
        THREAD_INFO * threadPtr;
        if(threadCurrent->pid!=ptr->pid)
        {
            /* cur thread pid <> item pid */
            if( threadCurrent->pid == 0 || force)
            {
                /* ok KERNEL thread is allowed to do this */
                /* trying to find thread struct using PID==0 */
                threadPtr=thread_findPid(0);
                if(!threadPtr)
                {
                    /* can't find a thread with this PID */
                    return -MED_EINVAL;
                }
            }
            else
            {
                /* not SYS nor force => not allowed to remove mem */
                printk("[THREAD LIST] RM failed, res= %d, pid is diff from current\n",res_id);
                return -MED_EINVAL;
            }
        }
        else /* using cur thread */
            threadPtr = threadCurrent;

        if(threadPtr->ressources[res_id].head_list == ptr)
        {
            /* trying to rm head */
            threadPtr->ressources[res_id].head_list = threadPtr->ressources[res_id].head_list->nxt;
            if(threadPtr->ressources[res_id].head_list)
                threadPtr->ressources[res_id].head_list->prev=NULL;
        }
        else
        {
            /* removing somewhere else */
            ptr->prev->nxt=ptr->nxt;
            if(ptr->nxt)
                ptr->nxt->prev=ptr->prev;
        }
    }
    return MED_OK;
}

/***********************************
* Add ressource item to cur thread
* list
***********************************/
void thread_listAdd(THREAD_LIST * ptr,int res_id,int force)
{
    if(threadCurrent)
    {
        THREAD_INFO * threadPtr;
        /* we have a cur thread */
        /* inserting it at the head of list */
        if(force)
        {
            /* using SYS thread => need to find PID 0*/
            threadPtr=thread_findPid(0);
            if(!threadPtr)
            {
                /* can't find SYS thread */
                ptr->pid=-1;
                ptr->nxt=ptr->prev=NULL;
                return;
            }
        }
        else
            threadPtr=threadCurrent;

        ptr->pid=threadPtr->pid;
        ptr->prev=NULL;
        ptr->nxt=threadPtr->ressources[res_id].head_list;
        if(ptr->nxt)
            ptr->nxt->prev=ptr;
        threadPtr->ressources[res_id].head_list=ptr;
    }
    else
    {
        /* no thread */
        ptr->pid=-1;
        ptr->nxt=ptr->prev=NULL;
    }
}

/***********************************
* freeing all alloc buffer of thread
***********************************/
void thread_listFree(THREAD_INFO * thread,int res_id)
{
    THREAD_LIST * ptr;
    for(ptr=thread->ressources[res_id].head_list;ptr!=NULL;ptr=ptr->nxt)
    {
        thread->ressources[res_id].clean_fct((void*)((unsigned int)ptr-thread->ressources[res_id].offset));
    }
}

/***********************************************************************************
************************************************************************************
Printing functions
************************************************************************************
***********************************************************************************/

/***********************************
* Prints the list of know thread
***********************************/
void thread_ps(void)
{
    THREAD_INFO * ptr;
    ptr=threadCurrent;
    if(ptr)
    {
        printk("Thread list:\n");
        do
        {
            printk("%s - pid %d - %s - Tick cnt %d\n",ptr->name!=NULL?ptr->name:"NO Name",ptr->pid,
            ptr->enable?"enable":"disable",ptr->tickCnt);
            ptr=ptr->nxt;
        } while(ptr!=threadCurrent);
    }
    else
        printk("Thread list empty!!\n");
}


/***********************************
* Print list of item of res_id for
* a given thread
***********************************/
void thread_listPrintPtr(int res_id,THREAD_INFO * thread)
{
    THREAD_LIST * ptr;
    printk("Item for pid=%d, res=%d\n",thread->pid,res_id);
    for(ptr=thread->ressources[res_id].head_list;ptr!=NULL;ptr=ptr->nxt)
        thread->ressources[res_id].print_fct((void*)((unsigned int)ptr-thread->ressources[res_id].offset));
}

/***********************************
* Print list of item of res_id for
* all thread in the ring
***********************************/
void thread_listPrintAll(int res_id)
{
    THREAD_INFO * threadHead,*ptr;
    threadHead=ptr=threadCurrent;
    if(ptr!=NULL)
    {
        do
        {
            //if(ptr->pid!=0)
                thread_listPrintPtr(res_id,ptr);
            ptr=ptr->nxt;
        } while(ptr!=threadHead);
    }
}

/***********************************
* Print info on a given thread
***********************************/
void thread_printInfo(THREAD_INFO * thread)
{
    if(thread)
    {
        printk("Pid: %d : %s is %s\n",thread->pid,thread->name,thread->enable?"Enable":"Disable");
        printk("Tick cnt=%d,\n",thread->tickCnt);
        if(thread->useSysStack)
        {
            printk("Stack is SYS iram stack (bottom=%x, save=%x, size=%x)\n",thread->stackBottom,
                thread->saveStack,thread->stackSize);
        }
        else
        {
            if(thread->stackBottom)
            {
                printk("Stack was malloc at: %x, size=%x\n",thread->stackBottom,thread->stackSize);
            }
            else
            {
                printk("Initial top of stack was pass as args\n");
            }
        }
        if(thread->codeMalloc)
        {
            printk("Code of thread was malloc at %x\n",thread->codeMalloc);
        }
        printk("Save regs: PC=%x, LR=%x, SP=%x CPSR=%x\n",thread->regs[16],
            thread->regs[15],thread->regs[14],thread->regs[0]);
        printk("R0=%x, R1=%x, R2=%x, R3=%x, R4=%x\n",thread->regs[1],
            thread->regs[2],thread->regs[3],thread->regs[4],thread->regs[5]);
        printk("R5=%x, R6=%x, R7=%x, R8=%x, R9=%x\n",thread->regs[6],
            thread->regs[7],thread->regs[8],thread->regs[9],thread->regs[10]);
        printk("R10=%x, R11=%x, R12=%x\n",thread->regs[11],
            thread->regs[12],thread->regs[13]);
    }
    else
        printk("Error printing info on thread\n");
}

/***********************************************************************************
************************************************************************************
helper functions
************************************************************************************
***********************************************************************************/

/***********************************
* Find thread struct from pid
***********************************/
THREAD_INFO * thread_findPid(int pid)
{
    THREAD_INFO * ptr=threadCurrent;
    if(ptr)
    {
        /* searching the PID */
        do
        {
            if(ptr->pid==pid)
                break;
            ptr=ptr->nxt;
        } while(ptr!=threadCurrent);
        /* have we found the thread ?*/
        if(ptr->pid==pid)
            return ptr;
        else
            return NULL;
    }
    else
        return NULL;
}

THREAD_INFO * thread_self(void)
{
    return threadCurrent;
}
