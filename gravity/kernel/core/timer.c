/* 
*   kernel/timer.c
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
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/kernel.h>

unsigned long tick __IRAM_DATA;

struct timer_s * timer_head __IRAM_DATA;

__IRAM_CODE void main_timer_action(void)
{
    struct timer_s * ptr=timer_head;
    
    tick++;

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
    outw(TMR_MODE_STOP, TIMER0_BASE+TIMER_MODE);
    outw(TMR_MODE_STOP, TIMER1_BASE+TIMER_MODE);
    outw(TMR_MODE_STOP, TIMER2_BASE+TIMER_MODE);
    outw(TMR_MODE_STOP, TIMER3_BASE+TIMER_MODE);

    /*
    * System clock formula:
    *         freq = clock / (div * scale)
    *  freq need to be 100Hz
    */

    outw(TMR_SEL_ARM, TIMER0_BASE+TIMER_SEL);

    /* prescale  */
    outw(9, TIMER0_BASE+TIMER_SCAL);

    /* div  */
    outw((CONFIG_ARM_CLK/1000)-1, TIMER0_BASE+TIMER_DIV);

    /* freerun */
    outw(TMR_MODE_FREERUN, TIMER0_BASE+TIMER_MODE);
       
    /* adding the timer action in irq struct */
    add_irq_handler(IRQ_TMR_0,main_timer_action,"Tick_timer");
    enable_irq(IRQ_TMR_0);
    
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

