/* 
*   kernel/core/timer.c
*
*   MediOS project
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
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/kernel.h>

#include <kernel/buttons.h>
#include <kernel/bat_power.h>
#include <kernel/usb_fw.h>
#include <kernel/ext_module.h>
#include <kernel/fm_remote.h>

unsigned long tick __IRAM_DATA;

struct timer_s * timer_head __IRAM_DATA;



__IRAM_CODE void main_timer_action(int irq)
{
    struct timer_s * ptr=timer_head;
    
    tick++;
    
    /* HW check */
    BTN_CHK;
#ifdef HAVE_BAT_POWER 
    BAT_POWER_CHK;
#endif
#ifdef HAVE_USB_FW
    USB_FW_CHK;
#endif
#ifdef HAVE_EXT_MODULE
    EXT_MODULE_CHK;    
#endif   
#ifdef HAVE_FM_REMOTE
    FM_REMOTE_CHK;    
#endif

    while(ptr!=NULL)
    {
        if(ptr->trigger && ptr->expires<=tick)
        {
            ptr->trigger=0;
            if(ptr->action)
                ptr->action();
        }
        ptr=ptr->nxt;
    }    
}

__IRAM_CODE void setup_timer(struct timer_s * timer_data,char * name)
{   
    timer_data->expires=0;
    timer_data->trigger=0;
    timer_data->name=name;
    
    timer_data->nxt=timer_head;
    timer_data->prev=NULL;
    timer_head=timer_data;
    
}

__IRAM_CODE void rm_timer(struct timer_s * timer_data)
{
    
    if(!timer_data->prev)
        timer_head=timer_data->nxt;    
    else
        timer_data->prev->nxt=timer_data->nxt;
        
    if(timer_data->nxt)
            timer_data->nxt->prev=timer_data->prev;
}

__IRAM_CODE void stop_timer(struct timer_s * timer_data)
{
    timer_data->trigger=0;
}

__IRAM_CODE void start_timer(struct timer_s * timer_data)
{
    if(timer_data->expires>tick)
        timer_data->trigger=1;
    else
        timer_data->trigger=0;
}

void init_timer(void)
{
    /* init the timer structure */
    timer_head=NULL;
    tick=0;
    
    /* disable all timer */
    SET_TIMER_MODE(TMR_MODE_STOP,TMR0);
    SET_TIMER_MODE(TMR_MODE_STOP,TMR1);
    SET_TIMER_MODE(TMR_MODE_STOP,TMR2);
    SET_TIMER_MODE(TMR_MODE_STOP,TMR3);
        
    arch_init_timer();
    
    printk("[init] timer\n");
}


void print_timer(void)
{
    struct timer_s * ptr=timer_head;
    int tmrnr=0;
    printk("Timer list: (cur tick:%d)\n",tick);
    while(ptr!=NULL)
    {
        printk("%d: %s, expire:%d\n",tmrnr,ptr->name!=NULL?ptr->name:"UNDEF",ptr->expires);
        tmrnr++;
        ptr=ptr->nxt;
    }
}

