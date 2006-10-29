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

extern THREAD_INFO * threadCurrent;
void thread_exit(void);

int pid;
THREAD_INFO * sysThread ;
THREAD_INFO * idleThread ;

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
    
    retval=thread_create(&sysThread,(void*)fct,(void*)thread_exit,NULL,(void*)IRAM_SIZE-SVC_STACK_SIZE,0,
        "KERNEL",NULL,NULL);
    
    if(retval<0)
    {
        printk("Error creating KERNEL thread (error code = %d\n",-retval);
        return retval;
    }
    else
        printk("KERNEL thread created with pid %d\n",retval);
    
    sysThread->enable=1;
    
    retval=thread_create(&idleThread,(void*)thread_idleFct,(void*)thread_exit,NULL,NULL,0x100,"IDLE",NULL,NULL);
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

/***********************************
* Init of med thread
* Create struct
* disable KERNEL thred
* yield
***********************************/
void thread_startMed(void * entry_fct,void * code_malloc,char * name,int argc,char ** argv)
{
    THREAD_INFO * med_thread;
    int pid=thread_create(&med_thread,entry_fct,(void*)thread_exit,code_malloc,NULL,0,name,
        (unsigned long)argc,(unsigned long)argv);
    printk("MED thread created with pid %d\n",pid);
    __cli();
    sysThread->enable=0;
    med_thread->enable=1;
    yield();
    __sti();
}

/***********************************
* Main create and init new thread
* also create its stack if needed
* on error returns a MED_RET <0
* returns PID otherwise
***********************************/
int thread_create(THREAD_INFO ** ret_thread,void * entry_fct,void * exit_fct,
    void * code_malloc,void * stack_top,int stack_size,char * name,unsigned long arg1,unsigned long arg2)
{
    int i;

    /* Malloc thread struct */
    THREAD_INFO * newThread = (THREAD_INFO*)malloc(sizeof(THREAD_INFO));
    if(!newThread)
    {
        printk("No mem left for task: %s\n",name!=NULL?name:"NO NAME");
        return -MED_ENOMEM;
    }
    /* Clear thread struct content */
    memset(newThread,0,sizeof(THREAD_INFO));
    /* Malloc stack */
    if(!stack_top)
    {
        if(stack_size==0)
            stack_size=STACK_SIZE;
        newThread->stackMalloc=(unsigned int *)malloc(stack_size);
        stack_top = (void*)(((unsigned long)(newThread->stackMalloc))+stack_size-4);
        if(!newThread->stackMalloc)
        {
            printk("No mem left for task's stack: %s\n",name!=NULL?name:"NO NAME");
            free(newThread);
            return -MED_ENOMEM;
        }
    }
    else
        stack_size=0;        
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
        newThread,newThread->stackMalloc,newThread->regs[14],
        (unsigned long)entry_fct,(unsigned long)exit_fct,THREAD_INIT_CPSR);

    if(ret_thread)
        *ret_thread=newThread;

    return newThread->pid;
}

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
void thread_doKill(THREAD_INFO * thread)
{
    THREAD_INFO * ptr;
    if(thread == idleThread)
    {
        printk("Error trying to kill idle thread\n");
        return;
    }
    __cli();
    ptr=threadCurrent;
    thread_remove(thread);
    if(thread->stackMalloc)
        free(thread->stackMalloc);
    if(thread->codeMalloc)
    {
        free(thread->codeMalloc);
        sysThread->enable=1; /* be sure to enable KERNEL thread */
    }   
    free(thread);
    if(thread==ptr)
    {
        thread_nxt();
        thread_loadContext();
    }
}

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
    
    idleThread->enable=0;
    
    if(threadCurrent==NULL)
    {
        printk("No thread in ring\n");
        printk("Let's loop\n");
        while(1)/*NOTHING*/;
    }
        
    do
    {
        threadCurrent=threadCurrent->nxt;
    } while(!threadCurrent->enable && ptr != threadCurrent);
    
    if(ptr == threadCurrent && !ptr->enable)
    {
        threadCurrent = idleThread;
        idleThread->enable=1;
    }
}

/***********************************
* Prints the list of know thread
***********************************/
void thread_print(void)
{
    THREAD_INFO * ptr=threadCurrent;

    if(ptr)
    {
        printk("Thread list:\n");
        do
        {
            printk("%s - pid %d - %s\n",ptr->name!=NULL?ptr->name:"NO Name",ptr->pid,
            ptr->enable?"enable":"disable");
            ptr=ptr->nxt;
        } while(ptr!=threadCurrent);
    }
    else
        printk("Thread list empty!!\n");
}


