/* 
*   kernel/core/evt.c
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

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/pipes.h>
#include <kernel/timer.h>

#include <kernel/evt.h>
#include <kernel/errors.h>

struct evt_pipes evt_pipe_tab[NB_EVT_PIPES];



#if 0

struct tmr_s evt_timer;
void evt_timer_action(void)
{
    send_evt(EVT_TIMER);
    evt_timer.expires = tick + EVT_DELAY; /* 1s timer */
    tmr_start(&evt_timer);
}
#endif

int evt_getHandler(unsigned int mask,int * result)
{
    int i;
    
    if(result == NULL)
        return -MED_EINVAL;
    
    /* look for a free evt_pipe */
    for(i=0;i<NB_EVT_PIPES;i++)
        if(!evt_pipe_tab[i].used)
            break;
    
    if(i==NB_EVT_PIPES)
        return -MED_EMOBJ;

    evt_pipe_tab[i].evt_pipe.nIN = evt_pipe_tab[i].evt_pipe.nOUT = 0;
    evt_pipe_tab[i].used = 1;
    evt_pipe_tab[i].mask = mask;
    
    *result = i;
    
    printk("[evt handling] register: %d (mask=%x)\n",i,mask);
    return MED_OK;
}

int evt_freeHandler(int num_evt_pipe)
{
    if(num_evt_pipe >= 0 && num_evt_pipe < NB_EVT_PIPES)
    {
        if(evt_pipe_tab[num_evt_pipe].used!=1)
            return -MED_ENBUSY;
        evt_pipe_tab[num_evt_pipe].used = 0;
        printk("[evt handling] UNregister: %d",num_evt_pipe);
    }
    else
        return -MED_EINVAL;    
    return MED_OK;
}

void evt_send(struct evt_t * evt)
{
    int i;
    for(i=0;i<NB_EVT_PIPES;i++)
    {
        if(evt_pipe_tab[i].used && (evt_pipe_tab[i].mask&evt->evt_class))
        {
            pipeWrite(&(evt_pipe_tab[i].evt_pipe), evt, sizeof(struct evt_t));
        }
    }
    
}

int  evt_getStatus(int num_evt_pipe, int * result)
{
    struct evt_t evt;
    evt.evt=0;
    evt.evt_class=0;
    evt.data=0;
    if(num_evt_pipe >= 0 && num_evt_pipe < NB_EVT_PIPES && result != NULL)
    {
        if(evt_pipe_tab[num_evt_pipe].used!=1)
            return -MED_ENBUSY;
        pipeRead(&(evt_pipe_tab[num_evt_pipe].evt_pipe), &evt, sizeof(struct evt_t));
        *result=evt.evt;        
    }
    else
        return -MED_EINVAL;    
    return MED_OK;
}

void evt_init(void)
{
    int i;
    for(i=0;i<NB_EVT_PIPES;i++)
    {
        evt_pipe_tab[i].evt_pipe.nIN=evt_pipe_tab[i].evt_pipe.nOUT=0;
        evt_pipe_tab[i].used=0;
    }
    
    /*setup_timer(&evt_timer,"EVT");
    evt_timer.action = evt_timer_action;
    evt_timer.expires = tick + EVT_DELAY;
    start_timer(&evt_timer);*/

    printk("[init] evt\n");
}
