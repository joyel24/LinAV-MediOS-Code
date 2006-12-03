/* 
*   kernel/driver/fm_remote_v2.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*
* Seconde version of the remot control
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/string.h>

#include <kernel/kernel.h>
#include <kernel/io.h>
#include <kernel/irqs.h>
#include <kernel/irq.h>
#include <kernel/hardware.h>
#include <kernel/cpld.h>
#include <kernel/gio.h>
#include <kernel/uart.h>
#include <kernel/cmd_line.h>

#include <kernel/fm_remote.h>
#include <kernel/timer.h>

int FM_connected=0;
int nbPingSend=0;
int nbNonGet=0;
int nbPongGet=0,minPongGet=0xFFFF;
char cmd=0;
char radio_param[5];
int fm_index;
int getPong=0,firstPong=0,pingSend=0;

struct tmr_s fmRemote_tmr;

void fm_remote_INT(int irq_num,struct pt_regs * reg)
{
    unsigned char c;
    while(uart_in(&c,FM_REMOTE_UART))
    {
        if(FM_connected)
        {
            switch(c)
            {
                case 0x0:
                    continue;                
                case 'V':
                    cmd=3;
                    fm_index=0;
                    break;
                case 'K':
                    cmd=1;
                    fm_index=0;
                    break;
                case 'R':
                    cmd=2;
                    fm_index=0;
                default:
                    if(cmd==0)
                    {
                        cmd=4;
                        fm_index=0;
                    }
                    break;
            }
            
            switch(cmd)
            {
                case 4:
                    printk("[FM Remote] get unknown char: %x\n",c);
                    cmd=0;
                    break;
                case 1:
                    fm_index++;
                    if(fm_index>1)
                    {
                        if(c!=0)
                        {
                            //printk("[FM Remote] get key : %x\n",c);
                            processKey(c);
                        }
                        cmd=0;
                    }
                    break;
                case 2:
                    radio_param[fm_index++]=c;
                    if(fm_index==5)
                    {
                        printk("[FM Remote] get radio cmd: %x%x%x%x%x\n",radio_param[0],radio_param[1]
                            ,radio_param[2],radio_param[3],radio_param[4]);
                        cmd=0;
                    }
                    break;
                case 3: /* get pong */
                    fm_index++;
                    if(fm_index>1)
                    {
                        if(c == 0x3)
                        {
                            nbPingSend=0;
                        }
                        cmd=0;
                    }
                    break;
                default:
                    cmd=0;
                    break;
            }
        }
        else
        {
            switch(c)
            {
                case 'V':
                    cmd=1;
                    break;
                case 0x3:
                    cmd=0;
                    fm_index=0;
                    FM_connected=1;
                    nbPingSend=0;
                    inHold=0;
                    FM_do_ini_call();
                    printk("[FM Remote] connected\n");
                    break;
                default:
                    cmd=0;
                    break;
            }
        }
    }        
}

int FM_is_connected(void)
{
    return FM_connected;
}

void fmRemote_chk(void)
{
    if(FM_connected)
    {
        nbPingSend++;
        if(nbPingSend>MAX_PING)
        {
            FM_connected=0;
            nbPingSend=0;
            inHold=0;
            printk("[FM Remote] disconnected\n");
        }
     }
     uart_out('v',FM_REMOTE_UART);
}

void FM_versionInit(void)
{
    /*setting up the UART port */
    
    fm_index=0;
    FM_connected=0;
    
    tmr_setup(&fmRemote_tmr,"FM remote remove chk");
    fmRemote_tmr.action = fmRemote_chk;
    fmRemote_tmr.freeRun = 1;
    fmRemote_tmr.stdDelay=HZ>>2; /* 0.5s period */
}

void FM_enable(void)
{
    char c;
    
    if(FM_REMOTE_UART==CMD_LINE_UART || FM_REMOTE_UART== DEBUG_UART)
    {
        cmd_line_disable();
        printk_uartDisable();
    }
    
    uart_changeSpeed(9600,FM_REMOTE_UART);
    
    /* initiale state */
    
    FM_connected=0;
    
    irq_changeHandler(UART_IRQ_NUM(FM_REMOTE_UART),fm_remote_INT);
    /* clear uart buffer in */
    while(uart_in(&c,FM_REMOTE_UART)) /*nothing*/;
    irq_enable(UART_IRQ_NUM(FM_REMOTE_UART));
    
    /* launch fm remote tmr */   
    tmr_start(&fmRemote_tmr);
}

void FM_disable(void)
{
    tmr_stop(&fmRemote_tmr);
    uart_restoreIrqHandler(UART_IRQ_NUM(FM_REMOTE_UART));
    irq_disable(UART_IRQ_NUM(FM_REMOTE_UART));
    if(FM_REMOTE_UART==CMD_LINE_UART || FM_REMOTE_UART== DEBUG_UART)
    {
        cmd_line_enable();
        printk_uartEnable();
    }
}
