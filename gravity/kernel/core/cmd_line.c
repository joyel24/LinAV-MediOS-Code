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

#include <sys_def/string.h>

#include <kernel/kernel.h>
#include <kernel/uart.h>
#include <kernel/hw_chk.h>
#include <kernel/memmgr.h>

#include <kernel/cmd_line.h>

#define MAX_CMD_LEN     100
#define MAX_ARGS        10

struct cmd_line_s cmd_tab[] = {
    {
        cmd        : "help",
        help_str   : "This is help",
        cmd_action : do_help,
        nb_args    : 0
    },    
    {
        cmd        : "printInfo",
        help_str   : "print the list of args",
        cmd_action : do_print_info,
        nb_args    : 3
    },
    /* this has to be the last entry */
    {
        cmd        : NULL,
        help_str   : NULL,
        cmd_action : NULL,
        nb_args    : 0
    }
};

#define RM_HEAD_SPC(ptr)      ({while(*ptr && *ptr==' ') ptr++;})
#define FIND_NXT_TOKEN(ptr)   ({char * __v=ptr; while(*__v && *__v!=' ') __v++; __v;})
#define FIND_CMD_LINE(ptr)                                                                 \
        ({                                                                                 \
            int __v=0;                                                                     \
            struct cmd_line_s * __w=NULL;                                                  \
            for(__v=0;cmd_tab[__v].cmd!=NULL && strcmp(cmd_tab[__v].cmd,ptr);__v++) ;    \
            if(cmd_tab[__v].cmd!=NULL) __w=&cmd_tab[__v];                                 \
            __w;                                                                           \
        })
        
#define UNKNOW_CMD(cmd)                                                                       \
        ({                                                                                 \
            printk("Unknown command: %s\nType help to have the list of command\n",cmd);        \
            cur_cmd[0]='\0';                                                               \
            cur_pos=0;                                                                      \
            printk("grv> ");                                                                  \
            return;                                                                        \
        })

__IRAM_DATA char * cur_cmd;
__IRAM_DATA int cur_pos;

__IRAM_DATA struct hw_chk_s cmdline_chker;

__IRAM_DATA char ** arg_list;

__IRAM_CODE void chk_uart_in(void)
{
    char c='\0';
    int nb_args=0;
    int i;
    struct cmd_line_s * cmd_line;
    char * ptr;
    
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
            /* processing the cmd */
            printk("[cmd_line] get cmd |%s|\n",cur_cmd);
            
            RM_HEAD_SPC(cur_cmd);
            
            /* let's find the cmd name */
            printk("[cmd_line] 1 - |%s|\n",cur_cmd);
            
            ptr=FIND_NXT_TOKEN(cur_cmd);
            
            if(!*ptr)   /* cmd with no args */
            {
                cmd_line=FIND_CMD_LINE(cur_cmd);
                if(!cmd_line)  UNKNOW_CMD(cur_cmd);
                printk("[cmd_line] 2 - find cmd |%s|, no args\n",cmd_line->cmd);
            }
            else
            {
                *ptr='\0';
                cmd_line=FIND_CMD_LINE(cur_cmd);
                if(!cmd_line)  UNKNOW_CMD(cur_cmd);
                cur_cmd = ptr+1;
                printk("[cmd_line] 2 - find cmd |%s|, args: |%s|\n",cmd_line->cmd,cur_cmd);
                /* parse args */
                while(*cur_cmd)
                {
                    RM_HEAD_SPC(cur_cmd);
                    if(*cur_cmd)
                    {
                        arg_list[nb_args]=cur_cmd;
                        nb_args++;
                        cur_cmd=FIND_NXT_TOKEN(cur_cmd);
                        if(*cur_cmd)
                        {
                           *cur_cmd='\0';
                           cur_cmd++;
                        }                         
                    }
                }
                printk("[cmd_line] 3 - args :|");
                for(i=0;i<nb_args;i++)
                    printk("%s|",arg_list[i]);
                printk("\n");
                
            }
            
            /* launch the cmd if we have enough params */
            if(nb_args>=cmd_line->nb_args)
                cmd_line->cmd_action(arg_list);
            else
                printk("%s need more args:\n%s\n",cmd_line->cmd,cmd_line->help_str);
            
            /* put back the prompt */
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
    
    arg_list = (char**)kmalloc(sizeof(char*)*MAX_ARGS);
    cur_cmd = (char*)kmalloc(sizeof(char)*MAX_CMD_LEN);
    
    cur_cmd[0]='\0';
    
    if(!arg_list)
    {
        printk("[init] cmd line, can't allocate memory for args\n");
        return;
    }
    
    ini_hw_chker(&cmdline_chker);
    
    cmdline_chker.name="Cmd line";
    cmdline_chker.action=chk_uart_in;
    add_hw_chker(&cmdline_chker);
            
    printk("[init] cmd line\n");
}

void do_help(char ** params)
{
    int i;
    printk("Available cmd:\n");
    for(i=0;cmd_tab[i].cmd!=NULL;i++)
        printk("%s: %s\n",cmd_tab[i].cmd,cmd_tab[i].help_str);
}

void do_print_info(char ** params)
{
    printk("Param received: %s %s %s\n",params[0],params[1],params[2]);
}
