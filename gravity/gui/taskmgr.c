/* 
*   gui/taskmgr.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <sys_def/stddef.h>
#include <api.h>
#include <gui/taskmgr.h>

TASKMGR * head_task_mgr;

TASKMGR * internal_tsk[NB_INTERNAL_TASK];

void launchInternalTask(void(*handler),int ini_state,int task_num)
{
      TASKMGR * ptr = createNewTask(handler);
      if(ptr)
      {
          ptr->task_type=VIRTUAL_TASK;
          ptr->state=ini_state;
      }
      
      internal_tsk[task_num]=ptr;
      printf("New int task %d, %08x\n",task_num,ptr);
}

TASKMGR * createNewTask(void(*handler))
{
    TASKMGR * ptr=(struct task_list_s *)malloc(sizeof(struct task_list_s));
    if(!ptr)
    {
        printf("Can't allocate mem for task mgr\n");
        return NULL;
    }
    
    ptr->state=0;
    ptr->task_type=NORMAL_TASK;
    ptr->evtHandler=handler;
    
    ptr->nxt=head_task_mgr;
    head_task_mgr=ptr;
    
    return ptr;
}

void evtLoop(unsigned int evt_buffer)
{
    int evt;
    
    TASKMGR * ptr;
    
    while(1)
    {
        printk("before wait\n");
        evt=waitEvt(evt_buffer);
        printk("get evt %d\n",evt);
        for(ptr=head_task_mgr;ptr!=NULL;ptr=ptr->nxt)
            if(ptr->state && ptr->evtHandler)
            {
                printk("send to task ");
                ptr->evtHandler(evt);   
                printk("done\n");     
            }
    }
}

void init_taskmgr(void)
{
    int i;
    head_task_mgr=NULL;
    for(i=0;i<NB_INTERNAL_TASK;i++)
        internal_tsk[i]=NULL;
}

void disable_all_internal_tasks(int * state)
{
    int i;
    *state=0;
    for(i=0;i<NB_INTERNAL_TASK;i++)
        if(internal_tsk[i] && internal_tsk[i]->state)            
        {
            *state &= 0x1 << i;
            internal_tsk[i]->state=0;
        }
}

void enable_internal_tasks(int * state)
{
    int i;
    for(i=0;i<NB_INTERNAL_TASK;i++)
        if(internal_tsk[i] && (*state & (0x1<<i)))
            internal_tsk[i]->state=1;    
}
