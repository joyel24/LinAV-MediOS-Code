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

struct evt_pipes_s * head;

struct timer_s evt_timer;

void evt_timer_action(void)
{
    send_evt(EVT_TIMER);
    evt_timer.expires = tick + EVT_DELAY; /* 1s timer */
    start_timer(&evt_timer);
}

struct evt_pipes_s * get_evt_handling(void)
{
    struct evt_pipes_s * ptr = (struct evt_pipes_s *)malloc(sizeof(struct evt_pipes_s));
    if(!ptr)
    {
        printk("[evt handling] can't alloc\n");
        return NULL;
    }
    
    ptr->evt_pipe.nReceiver = 0;
    ptr->evt_pipe.nSender   = 0;
    
    ptr->nxt=head;
    head=ptr;
    
    printk("[evt handling] register: %08x\n",ptr);
    
    return ptr;
}

void rm_evt_handling(struct evt_pipes_s * evt_pipes)
{
    struct evt_pipes_s * ptr=head;
    
    printk("[evt handling] UNregister: %08x",evt_pipes);
    
    if(evt_pipes==head)
    {
        head=head->nxt;
    }
    else
    {    
        while(ptr!=NULL && ptr->nxt!=evt_pipes) ptr=ptr->nxt;
        ptr=ptr->nxt;
    }
    printk(" done\n");
}

void do_send_evt(int evt)
{
    struct evt_pipes_s * ptr;
    unsigned char c=(unsigned char)evt;
    
    for(ptr=head;ptr!=NULL;ptr=ptr->nxt)
        kpipe_write (&(ptr->evt_pipe), &c, 1);
}

void init_evt(void)
{
    head=NULL;
    
    setup_timer(&evt_timer,"EVT");
    evt_timer.action = evt_timer_action;
    evt_timer.expires = tick + EVT_DELAY; 
    start_timer(&evt_timer);
    
    printk("[init] evt\n");
}
