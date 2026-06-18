/* 
*   include/gui/taskmgr.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __TASKMGR_H
#define __TASKMGR_H

#define TASK_BROWSER        0
#define TASK_STATUS_LINE    1

#define NB_INTERNAL_TASK    2

#define VIRTUAL_TASK        0
#define NORMAL_TASK         1

typedef struct task_list_s {    
    void (*evtHandler)(int evt);
    struct task_list_s * nxt;
    int task_type;
    int state;    
} TASKMGR;

void launchInternalTask(void(*handler),int ini_state,int task_num);
TASKMGR * createNewTask(void(*handler));
void evtLoop(unsigned int evtBuffer);
void init_taskmgr(void);
void disable_all_internal_tasks(int * state);
void enable_internal_tasks(int * state);

#endif
