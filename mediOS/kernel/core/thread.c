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

extern THREAD_INFO * threadCurrent;
void thread_exit(void);

int pid;

MED_RET_T thread_init(void(*fct)(void))
{
    THREAD_INFO * sysThread = (THREAD_INFO*)malloc(sizeof(THREAD_INFO));
    int i;
    
    pid=0;
    
    /* creating initial thread */
    if(!sysThread)
    {
        printk("No mem left for SYS task\n");
        threadCurrent = NULL;
        return -MED_ENOMEM;
    }
    /* Clear thread struct content */
    memset(sysThread,0,sizeof(THREAD_INFO));
    /* stack is in iram */
    sysThread->stackMalloc=NULL;
    strcpy(sysThread->name,"KERNEL");
    sysThread->stackSize=0;
    sysThread->pid=pid++;
    sysThread->regs[0]=(unsigned long)THREAD_SYSINIT_CPSR; //CPSR
    for(i=0;i<=12;i++)
        sysThread->regs[i+1]=(unsigned long)(i|(i<<8)|(i<<16)|(i<<24));

    sysThread->regs[14]=(unsigned long)(IRAM_SIZE-0x300);//SP
    
    sysThread->regs[15]=(unsigned long)thread_exit; //LR=thread exit trap
    sysThread->regs[16]=(unsigned long)fct; //PC=entry of thread    
    
    /* inserting sys thread in list */
    threadCurrent = sysThread;
    sysThread->nxt=sysThread;
    sysThread->prev=sysThread;
    
    /* loading context */
    thread_loadContext();
    
    /* we should never get here */
    return MED_OK;
}

void thread_exit(void)
{
    printk("Exit from %s\n",threadCurrent->name!=NULL?threadCurrent->name:"NO NAME");
    __cli();
    thread_remove(threadCurrent);
    thread_print();
    #warning need free for stack and struct here
    
    thread_loadContext();
}

MED_RET_T thread_kill(int pid)
{
    /* searching the PID */
    MED_RET_T ret_val=MED_OK;
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
        {
            __cli();
            thread_remove(ptr);
            thread_print();
            #warning need free for stack and struct here
            thread_loadContext();
        }
        else
            ret_val=-MED_ENOENT;
    }
    else
        ret_val=-MED_EINVAL;
    return ret_val;
}

MED_RET_T thread_create(THREAD_INFO ** ret_thread,void * entry,char * name)
{
    int stackSize=STACK_SIZE; /* maybe we'll need this as a param soon */
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
    newThread->stackMalloc=(unsigned int *)malloc(stackSize);
    if(!newThread->stackMalloc)
    {
        printk("No mem left for task's stack: %s\n",name!=NULL?name:"NO NAME");
        free(newThread);
        return -MED_ENOMEM;
    }
    /* Init thread struct data */
/* name */
    strncpy(newThread->name,name,THREAD_NAME_SIZE);
    newThread->name[THREAD_NAME_SIZE-1]=0;
    newThread->pid=pid++;
/* stack */
    newThread->stackSize=stackSize;

    newThread->regs[0]=(unsigned long)THREAD_INIT_CPSR; //CPSR
    for(i=0;i<=12;i++)
        newThread->regs[i+1]=(unsigned long)(i|(i<<8)|(i<<16)|(i<<24));

    newThread->regs[14]=((unsigned long)(newThread->stackMalloc))+stackSize-4;//SP
    newThread->regs[15]=(unsigned long)thread_exit; //LR=thread exit trap
    newThread->regs[16]=(unsigned long)entry; //PC=entry of thread


    printk("new thread created at %x, stack malloc at %x (top=%x), entry=%x exit=%x mode=%x (regs pos in struct %s)\n",
        newThread,newThread->stackMalloc,newThread->regs[14],
        entry,(unsigned long)thread_exit,THREAD_INIT_CPSR,
        ((unsigned long)newThread->regs==(unsigned long)newThread)?"OK":"KO");

    if(ret_thread)
        *ret_thread=newThread;

    return MED_OK;
}

MED_RET_T thread_insert(THREAD_INFO * thread)
{
    THREAD_INFO * nxt;
    if(!thread)
        return -MED_EINVAL;
    __cli();
    /*empty list*/
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
    __sti();
    return MED_OK;
}

MED_RET_T thread_remove(THREAD_INFO * thread)
{
    if(!threadCurrent)
    {
        printk("No thread\n");
        return -MED_EINVAL;
    }

    /* only one thread left */
    if(threadCurrent && threadCurrent->nxt==threadCurrent)
    {
        printk("Last thread\n");
        thread->nxt=NULL;
        thread->prev=NULL;
        threadCurrent=NULL;
        #warning what to do here => halt device ?
    }
    else
    {
        printk("at least 1 thread left\n");
        if(threadCurrent==thread)
        {
            printk("Trying to remove cur thread\n");
            threadCurrent=thread->nxt;
        }
        thread->prev->nxt=thread->nxt;
        thread->nxt->prev=thread->prev;
        thread->prev=NULL;
        thread->nxt=NULL;
        
        printk("rm= %x | cur = %x prev= %x nxt = %x\n", thread,threadCurrent , threadCurrent->prev,threadCurrent->nxt);
               
    }

    return MED_OK;

}

__IRAM_CODE void thread_nxt(void)
{
    threadCurrent=threadCurrent->nxt;    
}

void thread_print(void)
{
    THREAD_INFO * ptr=threadCurrent;
    int i=0;
    if(ptr)
    {
        printf("Thread list:\n");
        do
        {
            printf("%d: %s - pid=%d\n",i++,ptr->name!=NULL?ptr->name:"NO Name",ptr->pid);
            ptr=ptr->nxt;
        } while(ptr!=threadCurrent);
    }
    else
        printf("Thread list empty!!\n");
}


