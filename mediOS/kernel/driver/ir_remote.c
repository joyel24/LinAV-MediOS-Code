/* 
*   kernel/driver/ir_remote.c
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
#include <kernel/kernel.h>

#include <kernel/irq.h>
#include <kernel/gio.h>
#include <kernel/timer.h>
#include <kernel/ir_remote.h>
#include <kernel/bat_power.h>
#include <evt.h>
#include <kernel/evt.h>

int state=0;
int dataPos=0; // current pos in data
int IRdata=0;
int lastTmrVal=0;

#define NB_CODE 10

int nb_code=NB_CODE;

unsigned int IR_code[NB_CODE][2] = {
    { BTN_F1,        0x00ff6a95 },
    { BTN_F2,        0x00ffaa55 },
    { BTN_F3,        0x00ffea15 },
    { BTN_ON,        0x00ff4ab5 },
    { BTN_LEFT,      0x00ff728d },
    { BTN_RIGHT,     0x00fff20d },
    { BTN_UP,        0x00ff926d },
    { BTN_DOWN,      0x00ff52ad },
    { BTN_1,         0x00ffb24d },
    { BTN_OFF,       0x00ffd22d }
};

int last_code=-1;
int repeat_code=0;


void ir_remote_interrupt(int irq,struct pt_regs * regs)
{
    int val,delta;
    switch(state)
    {
        case 0:
            restartTimer();
            state=1;
            lastTmrVal=0;
            break;
        case 1:
            val=TMR_GET_CNT(TMR2);
            delta=val-lastTmrVal;
            lastTmrVal=val;
            if(delta<0x7d0)
            {
                if(delta>0x514 && delta<0x578)
                {
                    /* let's start a new code */
                    state=2;
                    dataPos=0;
                    IRdata=0;
                }
                else if(delta>0x44c && delta<0x4b0)
                {
                    /* last code repeated */
                    processCode(IRdata);
                }
            }
            else
            {
                restartTimer();
                lastTmrVal=0;
            }
            break;
        case 2:
            val=TMR_GET_CNT(TMR2);
            delta=val-lastTmrVal;
            lastTmrVal=val;
            if (delta>0x64 && delta<0x82)
            {
                dataPos++;
                IRdata = (IRdata << 1);
            }
            else if (delta>0xC8 && delta<0xF0)
            {
                dataPos++;
                IRdata = (IRdata << 1) | 1;
            }
            else
            {
                dataPos++;
                IRdata = (IRdata << 1);
            }
            
            if(dataPos==32)
            {
                state=0;
                processCode(IRdata);
            }
    }
}


void processCode(int code)
{
    int i;
    int stop=0;
    for(i=0;i<nb_code && !stop;i++)
    {
        if(IR_code[i][1] == code)
        {            
            if(last_code==code)
            {
                if(repeat_code >= 2)
                {
                    struct evt_t _evt;
                    if(lcd_get_state()==0)
                    {
                        /* the lcd is off => turn on and discard the event */
                        lcd_keyPress();
                        break;
                    }
                    else
                        lcd_launchTimer(); /* postpone the lcd timer */
                    halt_launchTimer(); /* postpone the poweroff timer */
                    _evt.evt= IR_code[i][0];
                    _evt.evt_class=BTN_CLASS;
                    _evt.data=NULL;
                    evt_send(&_evt);              
                    repeat_code=0;
                }
                else
                    repeat_code++;
            }
            else
            {
                last_code=code;
                repeat_code=1;
            }
            stop=1;
        }
    }
    
    /*if(!stop)
    {*/
        printk("IR code: %x\n",code);
    //}
}

void restartTimer(void)
{
    TMR_SET_SEL(TMR_SEL_EXT,TMR2);                    /* Ext clk */
    TMR_SET_SCAL(0x10D, TMR2);                        /* prescale  */
    TMR_SET_DIV(0xFFFF, TMR2);                        /* div  */
    TMR_SET_MODE(TMR_MODE_FREERUN, TMR2);             /* freerun */
}

void ir_remote_tmr_interrupt(int irq,struct pt_regs * regs)
{
    /* timeout in reception => stop current code */
    TMR_SET_MODE(TMR_MODE_STOP,TMR2);
    state=0;
    
}


void start_ir_remote(void)
{

    GIO_DIRECTION(GIO_IR,GIO_IN);
    GIO_IRQ_ENABLE(GIO_IR,GIO_IRQ);
    irq_enable(IRQ_IR);

    TMR_SET_MODE(TMR_MODE_STOP,TMR2);
    irq_enable(IRQ_TMR_2);

    state=0;
    dataPos=0;
    IRdata=0;

    printk("IR remote started\n");
}

void stop_ir_remote(void)
{
    TMR_SET_MODE(TMR_MODE_STOP,TMR2);
    irq_disable(IRQ_IR);
    irq_disable(IRQ_TMR_2);
    printk("IR remote stoped\n");
}



void init_ir_remote(void)
{
    irq_disable(IRQ_IR);
    irq_disable(IRQ_TMR_2);

    /* setting up GIO */
    GIO_DIRECTION(GIO_IR,GIO_IN);
    GIO_IRQ_ENABLE(GIO_IR,GIO_IRQ);

    /* setting up IRQ handler */

    TMR_SET_MODE(TMR_MODE_STOP,TMR2);
    irq_changeHandler(IRQ_TMR_2,ir_remote_tmr_interrupt);

    irq_disable(IRQ_IR);
    irq_disable(IRQ_TMR_2);

    printk("[init] IR remote\n");
}
