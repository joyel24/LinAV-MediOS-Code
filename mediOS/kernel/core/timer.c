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

#include <sys_def/string.h>

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
#include <kernel/bat_power.h>
#include <kernel/ata.h>
#include <kernel/exit.h>

unsigned long tick __IRAM_DATA;

struct tmr_s * tmr_head __IRAM_DATA;


__IRAM_CODE void tmr_intAction(int irq,struct pt_regs * regs)
{
    struct tmr_s * ptr=tmr_head;
    
    
    tick++;
    
    /* HW check */
    BTN_CHK;
    
#ifdef CHK_BAT_POWER 
    BAT_POWER_CHK;
#endif

#ifdef CHK_USB_FW
    USB_FW_CHK;
#endif

#ifdef HAVE_EXT_MODULE
    EXT_MODULE_CHK;    
#endif 
  
#ifdef HAVE_FM_REMOTE
    FM_REMOTE_CHK;    
#endif

    ATA_PWR_OFF_TASK;
    
    
    
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

unsigned int tmr_getTick(void)
{
    return tick;
}

void tmr_setup(struct tmr_s * tmr_data,char * name)
{
    tmr_data->expires=0;
    tmr_data->trigger=0;
    tmr_data->name=name;

    tmr_data->nxt=tmr_head;
    tmr_data->prev=NULL;
    tmr_head=tmr_data;

}

void tmr_remove(struct tmr_s * tmr_data)
{

    if(!tmr_data->prev)
        tmr_head=tmr_data->nxt;
    else
        tmr_data->prev->nxt=tmr_data->nxt;

    if(tmr_data->nxt)
            tmr_data->nxt->prev=tmr_data->prev;
}

void tmr_stop(struct tmr_s * tmr_data)
{
    tmr_data->trigger=0;
}

void tmr_start(struct tmr_s * tmr_data)
{
    if(tmr_data->expires>tick)
        tmr_data->trigger=1;
    else
        tmr_data->trigger=0;
}

void tmr_init(void)
{
    /* init the timer structure */
    tmr_head=NULL;
    tick=0;

    /* disable all timer */
    TMR_SET_MODE(TMR_MODE_STOP,TMR0);
    TMR_SET_MODE(TMR_MODE_STOP,TMR1);
    TMR_SET_MODE(TMR_MODE_STOP,TMR2);
    TMR_SET_MODE(TMR_MODE_STOP,TMR3);

    arch_tmr_init();
    
    printk("[init] timer\n");
}


void tmr_print(void)
{
    struct tmr_s * ptr=tmr_head;
    int tmrnr=0;
    printk("Timer list: (cur tick:%d)\n",tick);
    while(ptr!=NULL)
    {
        printk("%d: %s, expire:%d\n",tmrnr,ptr->name!=NULL?ptr->name:"UNDEF",ptr->expires);
        tmrnr++;
        ptr=ptr->nxt;
    }
}

