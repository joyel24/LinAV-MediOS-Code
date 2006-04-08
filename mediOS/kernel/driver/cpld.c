/*
*   kernel/driver/cpld.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/io.h>
#include <kernel/kernel.h>
#include <kernel/hardware.h>
#include <kernel/cpld.h>

int cpld_portState[4]=
{
    CPLD_PORT0_INIT,
    CPLD_PORT1_INIT,
    CPLD_PORT2_INIT,
    CPLD_PORT3_INIT,
};

int cpld_portArray[4] = {
    CPLD_PORT0,
    CPLD_PORT1,
    CPLD_PORT2,
    CPLD_PORT3
};

void cpld_init(void)
{
    int version;

    cpld_doSelect();
    outw(cpld_portState[CPLD1],CPLD_PORT1);
    outw(cpld_portState[CPLD2],CPLD_PORT2);
    outw(cpld_portState[CPLD3],CPLD_PORT3);

    version=cpld_getVersion();

    /* everything is ok */
    printk("[init] CPLD Ver:0x%x\n",version);
}

void cpld_changeState(int cpld_port,int bit_num,int direction)
{
    int tmp;

    tmp=cpld_portState[cpld_port];
    if(direction)
        tmp |= (0x1 << bit_num);
    else
        tmp &= ~(0x1 << bit_num);

    if(tmp!=cpld_portState[cpld_port])
    {
        cpld_portState[cpld_port]=tmp;
        outw(cpld_portState[cpld_port],cpld_portArray[cpld_port]);
    }
}

int cpld_read(int cpld_port)
{
    return inw(cpld_portArray[cpld_port]);
}

void cpld_select(int bit_num,int direction)
{
    int val;
    if(direction)
        val = cpld_portState[CPLD0] | (0x1 << bit_num);
    else
        val = cpld_portState[CPLD0] & ~(0x1 << bit_num);
    if(val != cpld_portState[CPLD0])
    {
        cpld_portState[CPLD0]=val;
        cpld_doSelect();
    }
}

void cpld_doSelect(void)
{
    int res,res2;
    printk("changing cpld select : %d\n",cpld_portState[CPLD0]);
    outw(cpld_portState[CPLD0],CPLD_PORT0);
    outw(cpld_portState[CPLD0],CPLD_PORT0);
    outw(cpld_portState[CPLD0],CPLD_PORT0);
    outw(cpld_portState[CPLD0],CPLD_PORT0);
    outw(cpld_portState[CPLD0],CPLD_PORT0);

    res=inw(cpld_portArray[CPLD0]);
    while((res2=inw(cpld_portArray[CPLD0]))!=res) /* wait for the value to become stable */
        res=res2;
}

int cpld_getVersion(void){
    return arch_cpld_getVersion();
}


