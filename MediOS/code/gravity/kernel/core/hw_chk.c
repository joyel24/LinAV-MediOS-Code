/* 
*   kernel/hw_chk.c
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
#include <kernel/timer.h>
#include <kernel/hw_chk.h>
#include <kernel/kernel.h>

__IRAM_DATA struct timer_s hw_chk_timer;

__IRAM_DATA struct hw_chk_s * hw_chk_head;

__IRAM_CODE void hw_chk_loop(void)
{
    struct hw_chk_s * ptr;
    for(ptr=hw_chk_head;ptr!=NULL;ptr=ptr->nxt)
        if(ptr->action)
            ptr->action();
    hw_chk_timer.expires = tick + HW_FREQ;
    start_timer(&hw_chk_timer);
}

void init_HW_chk(void)
{
    hw_chk_head=NULL;
    setup_timer(&hw_chk_timer,"HW_CHK");
    hw_chk_timer.action = hw_chk_loop;
    hw_chk_timer.expires = tick + HW_FREQ;
    start_timer(&hw_chk_timer);
    
    printk("[init] HW chker\n");
}

__IRAM_CODE void del_hw_chker(struct hw_chk_s * hw_chk_data)
{
    if(!hw_chk_data->prev)
        hw_chk_head=hw_chk_data->nxt;    
    else
        hw_chk_data->prev->nxt=hw_chk_data->nxt;
        
    if(hw_chk_data->nxt)
            hw_chk_data->nxt->prev=hw_chk_data->prev;    
}

__IRAM_CODE void add_hw_chker(struct hw_chk_s * hw_chk_data)
{
    hw_chk_data->nxt=hw_chk_head;
    hw_chk_data->prev=NULL;
    hw_chk_head=hw_chk_data;
}

__IRAM_CODE void ini_hw_chker(struct hw_chk_s * hw_chk_data)
{
    hw_chk_data->prev=NULL;
    hw_chk_data->nxt=NULL;
    hw_chk_data->action=NULL;
    hw_chk_data->name=NULL;    
}

void print_HW_chk(void)
{
    struct hw_chk_s * ptr;
    int i=0;
    printk("HW chk:\n");
    for(ptr=hw_chk_head;ptr!=NULL;ptr=ptr->nxt)
        printk("%d: %s, %s\n",i++,ptr->name!=NULL?ptr->name:"NO NAME",ptr->action!=NULL?"has action":"no action");
}
