/* 
*   kernel/core/cmd_line.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/kernel.h>
#include <kernel/uart.h>
#include <kernel/hw_chk.h>

#include <kernel/cmd_line.h>

#define MAX_CMD_LEN     100

__IRAM_DATA char cur_cmd[MAX_CMD_LEN];
__IRAM_DATA int cur_pos;

__IRAM_DATA struct hw_chk_s cmdline_chker;

__IRAM_CODE void chk_uart_in(void)
{
    char c='\0';
    
    //printk(" c ");
    
    while(1)
    {
        //printk(" d ");
    
        if(!uartIn(&c,UART_0))                /* nothing in uart buffer */
            break;
        
        if(c=='\n' || c=='\r')               /* end of line => add \0 to end the line */
        {
            cur_cmd[cur_pos++]='\0';
            break;
        }
        
        cur_cmd[cur_pos++]=c;               /* we have a char => add it in the cmd string */
        
        if(cur_pos>MAX_CMD_LEN)            /* can't add more chars => reset everything */
        {
            cur_cmd[0]='\0';
            cur_pos=0;
            c='\0';
            break;
        }
        
        c='\0';                            /* prepare char c for a new loop */
    }
    
    if(c!='\0')                            /* a cmd was send */
    {
        if(cur_pos==1)
        {
            printk("\ngrv> ");
        }
        else
        {
            printk("[cmd_line] get cmd %s\n",cur_cmd);
            printk("grv> ");
        }
        
        /* Ready to get a new cmd */
        cur_cmd[0]='\0';
        cur_pos=0;
    }    
}


void init_cmd_line(void)
{
    cur_pos=0;
    cur_cmd[0]='\0';
    
    ini_hw_chker(&cmdline_chker);
    
    cmdline_chker.name="Cmd line";
    cmdline_chker.action=chk_uart_in;
    add_hw_chker(&cmdline_chker);
            
    printk("[init] cmd line\n");
}