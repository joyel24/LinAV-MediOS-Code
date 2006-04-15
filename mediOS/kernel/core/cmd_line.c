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
#include <kernel/io.h>
#include <kernel/uart.h>

#include <kernel/irq.h>
#include <kernel/timer.h>

#include <kernel/bflat.h>
#include <kernel/malloc.h>

#include <kernel/cmd_line.h>

#include <kernel/exit.h>

/*******************************************************/
#if 1
#undef __IRAM_CODE
#define __IRAM_CODE
#undef __IRAM_DATA
#define __IRAM_DATA
#endif
/*******************************************************/

#define MAX_CMD_LEN     100
#define MAX_ARGS        10

struct pt_regs * cur_regs;

struct cmd_line_s cmd_tab[] = {
    {
        cmd        : "help",
        help_str   : "This is help",
        cmd_action : do_help,
        nb_args    : 0
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
        cmd        : "handlerInfo",
        help_str   : "Prints IRQ, timer, HW chk handler",
        cmd_action : print_handler_info,
        nb_args    : 0
    },
    {
        cmd        : "dump",
        help_str   : "Dumps memory from given address",
        cmd_action : do_memory_dump,
        nb_args    : 1
    },
    {
        cmd        : "reg",
        help_str   : "Print current regs value",
        cmd_action : do_reg_print,
        nb_args    : 0
    },
    {
        cmd        : "halt",
        help_str   : "Halts the device",
        cmd_action : do_halt,
        nb_args    : 0
    },
    {
        cmd        : "reload",
        help_str   : "Reloads the firmware",
        cmd_action : do_reload,
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
#define FIND_CMD_LINE(PTR,CMD_TAB)                                                                 \
        ({                                                                                 \
            int __v=0;                                                                     \
            struct cmd_line_s * __w=NULL;                                                  \
            for(__v=0;CMD_TAB[__v].cmd!=NULL && strcmp(CMD_TAB[__v].cmd,PTR);__v++) ;    \
            if(CMD_TAB[__v].cmd!=NULL) __w=&CMD_TAB[__v];                                 \
            __w;                                                                           \
        })
        
__IRAM_DATA unsigned char * cur_cmd;
__IRAM_DATA int cur_pos;

__IRAM_DATA unsigned char ** arg_list;

__IRAM_CODE void cmd_line_INT(int irq_num,struct pt_regs * regs)
{
    unsigned char c;
    int uart = irq_num - IRQ_UART0;
    
    while(uart_in(&c,uart))
    {
        if(c=='\n' || c=='\r')               /* end of line => add \0 to end the line */
        {
            cur_cmd[cur_pos++]='\0';
            printk("\n"); /* local echo */
            process_cmd(regs);
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
                    if(uart_in(&c,uart))
                    {
                        if(c=='[')
                        {
                            if(uart_in(&c,uart))
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
                        printk("\nmediOS> %s",cur_cmd);
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
}

__IRAM_CODE void process_cmd(struct pt_regs * regs)
{        
    int nb_args=0;    
    struct cmd_line_s * cmd_line;
    unsigned  char * ptr;
    
    cur_regs = regs;
    
    if(cur_pos==1)
    {
        printk("\nmediOS> ");
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
                printk("mediOS> ");
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
                printk("mediOS> ");
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
        printk("mediOS> ");
    }
            
    /* Ready to get a new cmd */
    cur_cmd[0]='\0';
    cur_pos=0;

}

void init_cmd_line(void)
{
    cur_pos=0;
    
    arg_list = (unsigned char**)malloc(sizeof(unsigned char*)*MAX_ARGS);
    cur_cmd = (unsigned char*)malloc(sizeof(unsigned char)*MAX_CMD_LEN);
    
    cur_cmd[0]='\0';

    irq_changeHandler(IRQ_UART0,cmd_line_INT);

    if(!arg_list)
    {
        printk("[init] cmd line, can't allocate memory for args\n");
        return;
    }
          
    printk("[init] cmd line\n");
}

void do_help(unsigned char ** params)
{
    int i;
    printk("Available cmd:\n");
    for(i=0;cmd_tab[i].cmd!=NULL;i++)
        printk("%s: %s\n",cmd_tab[i].cmd,cmd_tab[i].help_str);
}

void do_mem (unsigned char ** params)
{
    unsigned int nAlloc_u, nAlloc_k,nFree,MaxFree;
    mem_stat (&nAlloc_u, &nAlloc_k,&nFree,&MaxFree);
    printk("Free memory: %x - Alloc: user: %x kernel: %x\n", nFree , nAlloc_u, nAlloc_k);
}

void do_run (unsigned char ** params)
{
    load_bflat (params[0]);
}

void do_halt (unsigned char ** params)
{
    halt_device();
}

void do_reload (unsigned char ** params)
{
    reload_firmware();
}

void print_handler_info (unsigned char ** params)
{
    irq_print();
    tmr_print();
}

void do_reg_print (unsigned char ** params)
{
    int i=0,j=0;
    
    for(i=0;i<13;i++)
    {
         printk("R%02d=0x%08x ",i,cur_regs->uregs[i]);
         j++;
         if(j==4)
         {
            j=0;
            printk("\n");
         }
    }
    printk("\n");

    printk("sp_SVC=0x%08x lr_SVC=0x%08x pc=0x%08x\n",cur_regs->uregs[13],cur_regs->uregs[14],cur_regs->uregs[15]);
    printk("cpsr=0x%08x old_ro==0x%08x\n",cur_regs->uregs[16],cur_regs->uregs[17]);
}

void do_memory_dump (unsigned char ** params)
{
   int nAddress = atoi (params[0]);
   printk("Memory dump from %i:\n", nAddress);

   unsigned char* pMemory = (unsigned char*)nAddress;

   int i, j;
   for (i=0;i<16;i++)
   {
      printk ("%08X:  ", (unsigned long)pMemory);
      for (j=0;j<16;j++)
         printk ("%02X ", pMemory[j]);
      printk ("  |  ");
      for (j=0;j<16;j++)
      {
         unsigned char c = pMemory[j];
         if (c < 32)
            c = '?';
         if (c >= 128)
            c = '?';
         printk ("%c", c);
      }
      pMemory += 16;
      printk ("\n");
   }
}
