/*
*   kernel/core/cmd_line_fct.c
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
#include <kernel/cmd_line.h>

#include <kernel/med.h>
#include <kernel/thread.h>
#include <kernel/exit.h>
#include <kernel/malloc.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/io.h>
#include <kernel/disk.h>
#include <kernel/gio.h>
#include <kernel/cpld.h>

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
        help_str   : "Dumps memory from given address, usage: dump address size",
        cmd_action : do_memory_dump,
        nb_args    : 2
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
    {
        cmd        : "in",
        help_str   : "Reads a value from memory, usage: in address size",
        cmd_action : do_in,
        nb_args    : 2
    },
    {
        cmd        : "out",
        help_str   : "Writes a value to memory, usage: out address size value",
        cmd_action : do_out,
        nb_args    : 3
    },
    {
        cmd        : "ps",
        help_str   : "List threads",
        cmd_action : do_ps,
        nb_args    : 0
    },
    {
        cmd        : "kill",
        help_str   : "kill thread with given pid",
        cmd_action : do_kill,
        nb_args    : 1
    },
    {
        cmd        : "threadState",
        help_str   : "Enable/Disable a thread with given pid",
        cmd_action : do_ThreadState,
        nb_args    : 2
    },
    {
        cmd        : "threadInfo",
        help_str   : "Prints info on a thread with given pid",
        cmd_action : do_ThreadInfo,
        nb_args    : 1
    },
    {
        cmd        : "threadNice",
        help_str   : "Set thread priority for given pid",
        cmd_action : do_ThreadNice,
        nb_args    : 2
    },
    {
        cmd        : "gioDir",
        help_str   : "Set the direction DIR for a given gio GIO: gioDir GIO DIR",
        cmd_action : do_gioDir,
        nb_args    : 2
    },
    {
        cmd        : "gioSetState",
        help_str   : "set gio GIO to state STATE: gioSetState GIO STATE",
        cmd_action : do_gioSetState,
        nb_args    : 2
    },
    {
        cmd        : "gioGetState",
        help_str   : "Print state of gio GIO: gioGetState GIO STATE",
        cmd_action : do_gioGetState,
        nb_args    : 1
    },
    {
        cmd        : "diskInfo",
        help_str   : "Print info on disk D: diskInfo D",
        cmd_action : do_diskInfo,
        nb_args    : 1
    },
    {
        cmd        : "cpldRead",
        help_str   : "Read cpld port N: cpldRead N",
        cmd_action : do_cpldRead,
        nb_args    : 1
    },
    {
        cmd        : "cpldWrite",
        help_str   : "Write VAL to cpld port N: cpldWrite N VAL",
        cmd_action : do_cpldWrite,
        nb_args    : 2
    },
    /* this has to be the last entry */
    {
        cmd        : NULL,
        help_str   : NULL,
        cmd_action : NULL,
        nb_args    : 0
    }
};

void do_help(unsigned char ** params)
{
    int i;
    printk("Available cmd:\n");
    for(i=0;cmd_tab[i].cmd!=NULL;i++)
        printk("%s: %s\n",cmd_tab[i].cmd,cmd_tab[i].help_str);
}

void do_mem (unsigned char ** params)
{
    mem_printStat();
}

void do_run (unsigned char ** params)
{
    med_load (params[0]);
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

void do_memory_dump (unsigned char ** params)
{
    int Address = atoi (params[0]);
    int Size = atoi (params[1]);

    printk("Memory dump from %i (0x%0.8x), %d bytes:\n", Address, Address, Size);

    unsigned char* Memory = (unsigned char*)Address;

    print_data(Memory,Size);
}

void do_in (unsigned char ** params)
{
    int Address = atoi (params[0]);
    int Size = atoi (params[1]);
    int Value=0;

    switch (Size){
        case 1:
            Value=inb(Address);
            printk("0x%0.8x=%d (0x%0.2x)\n",Address,Value,Value);
            return;
        case 2:
            Value=inw(Address);
            printk("0x%0.8x=%d (0x%0.4x)\n",Address,Value,Value);
            return;
        case 4:
            Value=inl(Address);
            printk("0x%0.8x=%d (0x%0.8x)\n",Address,Value,Value);
            return;
        default:
            printk("Size should be 1, 2 or 4\n");
            return;
    }
}

void do_out (unsigned char ** params)
{
    int Address = atoi (params[0]);
    int Size = atoi (params[1]);
    int Value = atoi (params[2]);

    switch (Size){
        case 1:
            outb(Value,Address);
            Value=inb(Address);
            printk("0x%0.8x=%d (0x%0.2x)\n",Address,Value,Value);
            return;
        case 2:
            outw(Value,Address);
            Value=inw(Address);
            printk("0x%0.8x=%d (0x%0.4x)\n",Address,Value,Value);
            return;
        case 4:
            outl(Value,Address);
            Value=inl(Address);
            printk("0x%0.8x=%d (0x%0.8x)\n",Address,Value,Value);
            return;
        default:
            printk("Size should be 1, 2 or 4\n");
            return;
    }
}

void do_ps (unsigned char ** params)
{
    thread_ps();
}

void do_kill (unsigned char ** params)
{
    int pid = atoi (params[0]);
    MED_RET_T ret_val;
    printk("Killing pid %d\n",pid);
    ret_val=thread_kill(pid);
    if(ret_val!=MED_OK)
        printk("Error killing %d : %d\n",pid,-ret_val);
}

void do_ThreadState (unsigned char ** params)
{
    int pid = atoi (params[0]);
    int state = atoi (params[1]);
    if(state)
    {
        printk("Enable thread %d\n",pid);
        thread_enable(pid);
    }
    else
    {
        printk("Disable thread %d\n",pid);
        thread_disable(pid);
    }
}

void do_ThreadInfo (unsigned char ** params)
{
    int pid = atoi (params[0]);
    int i;
    THREAD_INFO * ptr=thread_findPid(pid);
    if(!ptr)
    {
        printk("thread with pid %d not found\n",pid);
    }
    else
    {
        thread_printInfo(ptr);
        if(pid!=0)
        {
            for(i=0;i<THREAD_NB_RES;i++)
            {
                printk("Res %d\n",i);
                thread_listPrintPtr(i,ptr);
            }
        }
    }
}
void do_ThreadNice (unsigned char ** params)
{
    int pid = atoi (params[0]);
    int prio = atoi (params[1]);

    THREAD_INFO * ptr=thread_findPid(pid);
    if(!ptr)
    {
        printk("thread with pid %d not found\n",pid);
    }
    else
    {
        thread_printInfo(ptr);
        if(thread_nice(ptr,prio)==MED_OK)
        {
            printk("Priority %d set for PID=%d\n",prio,pid);
        }
        else
        {
            printk("Bad priority %d\n",prio);
        }
    }
}

void do_gioDir (unsigned char ** params)
{
    int gio = atoi (params[0]);
    int dir = atoi (params[1]);
    
    if(dir!=0 && dir!=1)
        return;
    GIO_DIRECTION(gio,dir);
}

void do_gioSetState (unsigned char ** params)
{
    int gio = atoi (params[0]);
    int state = atoi (params[1]);
    
    switch(state)
    {
        case 0:
            GIO_CLEAR(gio);
            break;
        case 1:
            GIO_SET(gio);
            break;
    }
}

void do_gioGetState (unsigned char ** params)
{
    int gio = atoi (params[0]);
    printk("Gio 0x%x is %s (DIR=%s)\n",gio,GIO_IS_SET(gio)?"set":"not set",GIO_GET_DIR(gio)?"IN":"OUT");
}

void do_diskInfo (unsigned char ** params)
{
    int disk = atoi (params[0]);
    printk("disk=%d\n",disk);
    if(disk !=0 && disk !=1)
        return;
    disk_readInfo(disk,1);
}

void do_cpldRead (unsigned char ** params)
{
    int portNum=atoi (params[0]);
    printk("cpld_%d=%x\n",portNum,cpld_read(portNum));
}

void do_cpldWrite (unsigned char ** params)
{
    int portNum=atoi (params[0]);
    int val=atoi (params[1]);
    cpld_write(portNum,val);
    printk("wrote %x in cpld_%d\n",val,portNum);    
}
