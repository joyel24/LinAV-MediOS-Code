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
#include <kernel/memmgr.h>
#include <api.h>
#include <kernel/pipes.h>
#include <kernel/threads.h>


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
        help_str   : "print the list of args (3 args needed)",
        cmd_action : do_print_info,
        nb_args    : 3
    },
    {
        cmd        : "mem",
        help_str   : "shows free memory",
        cmd_action : do_mem,
        nb_args    : 0
    },
    {
        cmd        : "run",
        help_str   : "runs specified file",
        cmd_action : do_run,
        nb_args    : 1
    },
    {
        cmd        : "tasks",
        help_str   : "shows running tasks",
        cmd_action : do_tasks,
        nb_args    : 0
    },
    {
        cmd        : "restart",
        help_str   : "restarts device",
        cmd_action : do_restart,
        nb_args    : 0
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
            goto loop;                                                                        \
        })

__IRAM_DATA unsigned char * cur_cmd;
__IRAM_DATA int cur_pos;

__IRAM_DATA unsigned char ** arg_list;

__IRAM_CODE void cmd_line_task(PIPE * uart_pipe)
{
    unsigned char c='\0';
    int nb_args=0;
    int i;
    struct cmd_line_s * cmd_line;
    unsigned  char * ptr;
    
    //printk(" c ");
    while(1)
    {
        c='\0';
        nb_args=0;
        while(1)
        {
            //printk(" d ");
        
            API_PIPE_RECV(uart_pipe,&c,1);
            
            if(c=='\n' || c=='\r')               /* end of line => add \0 to end the line */
            {
                cur_cmd[cur_pos++]='\0';
                break;
            }
            
            if(c>=0x20 && c<0xFF && c!=0x7F)
                cur_cmd[cur_pos++]=c;               /* we have a char => add it in the cmd string */
            else
            {                                       /* special chars */
                switch(c)
                {
                    case 0x1B:                      /* ESC */
                        if(uartIn(&c,UART_0))
                        {
                            if(c=='[')
                            {
                                if(uartIn(&c,UART_0))
                                {
                                    switch(c)
                                    {
                                        case 0x41:
                                            printk("\nUP\n");
                                            break;
                                        case 0x42:
                                            printk("\nDOWN\n");
                                            break;
                                        case 0x43:
                                            printk("\nRIGHT\n");
                                            break;
                                        case 0x44:
                                            printk("\nLEFT\n");
                                            break;                                    
                                    }
                                }
                            }
                        }
                        break;
                    case 0x08:                      /* bckspc */
                    case 0x7F:                      /* del */
                        if(cur_pos>0)
                        {
                            cur_pos--;
                            cur_cmd[cur_pos]='\0';
                            printk("\ngrv> %s",cur_cmd);
                        }
                        break;
                    default:
                        break;                    
                }
                c='\0';
                break;
                
            }
            //printk("read %c %d\n",c,(int)c);
            
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
                //printk("[cmd_line] get cmd |%s|\n",cur_cmd);
                
                RM_HEAD_SPC(cur_cmd);
                
                /* let's find the cmd name */
                //printk("[cmd_line] 1 - |%s|\n",cur_cmd);
                
                ptr=FIND_NXT_TOKEN(cur_cmd);
                
                if(!*ptr)   /* cmd with no args */
                {
                    cmd_line=FIND_CMD_LINE(cur_cmd);
                    if(!cmd_line)  UNKNOW_CMD(cur_cmd);
                    //printk("[cmd_line] 2 - find cmd |%s|, no args\n",cmd_line->cmd);
                }
                else
                {
                    *ptr='\0';
                    cmd_line=FIND_CMD_LINE(cur_cmd);
                    if(!cmd_line)  UNKNOW_CMD(cur_cmd);
                    cur_cmd = ptr+1;
                    //printk("[cmd_line] 2 - find cmd |%s|, args: |%s|\n",cmd_line->cmd,cur_cmd);
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
                    /*printk("[cmd_line] 3 - args :|");
                    for(i=0;i<nb_args;i++)
                        printk("%s|",arg_list[i]);
                    printk("\n");*/
                    
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
loop:
           
    }
}

extern PIPE * UART_0_Pipe;

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
        
    API_TASK_CREATE (cmd_line_task, UART_0_Pipe, NULL);
        
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

void do_mem (char ** params)
{
    unsigned long nBytes;
    API_MEMAVAIL (&nBytes);
    printk("Free memory: %i KB\n", nBytes >> 10);
}

void do_run (char ** params)
{
    API_RUN_GRV (params[0], 0);
}

void do_tasks (char ** params)
{
	int nTasks = 0;

	__cli ();
	TASK_INFO* pStart = g_pTaskRing;
	TASK_INFO* pWork = pStart;
	do
		pWork = pWork->pNextTask, nTasks ++;
			while (pStart != pWork);
	__sti ();

	printk("Tasks running: %i\n", nTasks);
}

void do_restart (char ** params)
{
    int (*restart_restart)(void);
    restart_restart = 0;
    restart_restart ();
}
