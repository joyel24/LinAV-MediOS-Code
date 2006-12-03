/*
*   kernel/core/cmd_line.c
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

#include <kernel/kernel.h>
#include <kernel/uart.h>
#include <kernel/irq.h>
#include <kernel/malloc.h>

#include <kernel/cmd_line.h>


#define MAX_CMD_LEN     100
#define MAX_ARGS        10

#define MEDIOS_PROMPT "mediOS> "

extern struct cmd_line_s cmd_tab[];

#define RM_HEAD_SPC(ptr)      ({while(*ptr && *ptr==' ') ptr++;})
#define FIND_NXT_TOKEN(ptr)   ({char * __v=ptr; while(*__v && *__v!=' ') __v++; __v;})
#define FIND_CMD_LINE(PTR,CMD_TAB)                                                                 \
        ({                                                                                 \
            int __v=0;                                                                     \
            struct cmd_line_s * __w=NULL;                                                  \
            for(__v=0;CMD_TAB[__v].cmd!=NULL && strcmp(CMD_TAB[__v].cmd,PTR);__v++) ;    \
            if(CMD_TAB[__v].cmd!=NULL) __w=&CMD_TAB[__v];                                 \
            __w;                                                                           \
        })

unsigned char * cur_cmd;
int cur_pos;

unsigned char ** arg_list;

THREAD_INFO * cmdLineThread;

int cmd_line_init_ok;

void cmd_line_INT(int irq_num,struct pt_regs * regs)
{
    if(!cmdLineThread->enable)
        cmdLineThread->enable=1;    
}

void cmd_line_thread(void)
{
    unsigned char c;
    
    
    while(1)
    {
        while(uart_in(&c,CMD_LINE_UART))
        {
            if(c=='\n' || c=='\r')               /* end of line => add \0 to end the line */
            {
                cur_cmd[cur_pos++]='\0';
                printk("\n"); /* local echo */
                process_cmd();
                continue;
            }
    
            if(c>=0x20 && c<0xFF && c!=0x7F)
            {
                    cur_cmd[cur_pos++]=c;               /* we have a char => add it in the cmd string */
                    printk("%c",c); /* local echo */
            }
            else
            {                                       /* special chars */
                switch(c)
                {
                    case 0x1B:                      /* ESC */
                        if(uart_in(&c,CMD_LINE_UART))
                        {
                            if(c=='[')
                            {
                                if(uart_in(&c,CMD_LINE_UART))
                                {
                                    /*switch(c)
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
                                    }*/
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
                            printk("\n"MEDIOS_PROMPT"%s",cur_cmd);
                        }
                        break;
                    default:
                        break;
                }
            }
    
            if(cur_pos>MAX_CMD_LEN)            /* can't add more chars => reset everything */
            {
                cur_cmd[0]='\0';
                cur_pos=0;
            }
        }
        THREAD_PAUSE();
    }
}

void process_cmd(void)
{
    int nb_args=0;
    struct cmd_line_s * cmd_line;
    unsigned  char * ptr;

    if(cur_pos==1)
    {
        printk("\n"MEDIOS_PROMPT);
    }
    else
    {
        /* processing the cmd */
        RM_HEAD_SPC(cur_cmd);

        /* let's find the cmd name */
        ptr=FIND_NXT_TOKEN(cur_cmd);

        if(!*ptr)   /* cmd with no args */
        {
            cmd_line=FIND_CMD_LINE(cur_cmd,cmd_tab);
            if(!cmd_line)
            {
                printk("Unknown command: %s\nType help to have the list of command\n",cur_cmd);
                cur_cmd[0]='\0';
                cur_pos=0;
                printk(MEDIOS_PROMPT);
                return;
            }
        }
        else
        {
            *ptr='\0';
            cmd_line=FIND_CMD_LINE(cur_cmd,cmd_tab);
            if(!cmd_line)
            {
                printk("Unknown command: %s\nType help to have the list of command\n",cur_cmd);
                cur_cmd[0]='\0';
                cur_pos=0;
                printk(MEDIOS_PROMPT);
                return;
            }

            cur_cmd = ptr+1;
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
        }

        /* launch the cmd if we have enough params */
        if(nb_args>=cmd_line->nb_args)
            cmd_line->cmd_action(arg_list);
        else
            printk("%s need more args:\n%s\n",cmd_line->cmd,cmd_line->help_str);

        /* put back the prompt */
        printk(MEDIOS_PROMPT);
    }

    /* Ready to get a new cmd */
    cur_cmd[0]='\0';
    cur_pos=0;

}

void init_cmd_line(void)
{
    int pid;
    cur_pos=0;
    cmd_line_init_ok=0;
    arg_list = (unsigned char**)kmalloc(sizeof(unsigned char*)*MAX_ARGS);
    
    if(!arg_list)
    {
        printk("[init] cmd line, can't allocate memory for args\n");
        return;
    }
    
    cur_cmd = (unsigned char*)kmalloc(sizeof(unsigned char)*MAX_CMD_LEN);
    if(!cur_cmd)
    {
        kfree(arg_list);
        printk("[init] cmd line, can't allocate memory for args\n");
        return;
    }
    
    
    cur_cmd[0]='\0';

    
    pid=thread_startFct(&cmdLineThread,cmd_line_thread,"cmd line",THREAD_DISABLE_STATE,PRIO_HIGH);
    
    if(pid<0)
    {
        printk("Error creating cmd line thread: %d\n",-pid);
        
        kfree(cur_cmd);
        kfree(arg_list);
        return;
    }
    cmd_line_init_ok=1;
    cmd_line_enable();
    printk("[init] cmd line (thread pid:%d)\n",pid);    
}

void cmd_line_enable(void)
{
    char c;
    
    if(cmd_line_init_ok)
    {
        uart_need(CMD_LINE_UART);
        uart_changeSpeed(115200,CMD_LINE_UART);
    
        irq_changeHandler(UART_IRQ_NUM(CMD_LINE_UART),cmd_line_INT);
            /* clear uart1 buffer in */
        while(uart_in(&c,CMD_LINE_UART)) /*nothing*/;
        irq_enable(UART_IRQ_NUM(CMD_LINE_UART));
    }
}

void cmd_line_disable(void)
{
    if(cmd_line_init_ok)
    {
        uart_restoreIrqHandler(UART_IRQ_NUM(CMD_LINE_UART));
        irq_disable(UART_IRQ_NUM(CMD_LINE_UART));
    }
}

