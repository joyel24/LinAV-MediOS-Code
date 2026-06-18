/* 
*   kernel/driver/cpld.c
*
*   AMOS project
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

#warning we should lock the access to cpld see lock comment below

__IRAM_DATA int cpld_port_state[4]=
{
    0x0,
    0x0,
    0x4,
    0x8,
};

__IRAM_DATA int cpld_port_array[4] = {
    CPLD_PORT0,
    CPLD_PORT1,
    CPLD_PORT2,
    CPLD_PORT3
};

void init_cpld(void)
{
    int version;
    
    cpld_do_select(); 
    outw(cpld_port_state[CPLD1],CPLD_PORT1);
    outw(cpld_port_state[CPLD2],CPLD_PORT2);
    outw(cpld_port_state[CPLD3],CPLD_PORT3);
    
    version=(cpld_read(CPLD2) & 0x000F) | ((cpld_read(CPLD3) & 0x0007)<<4);
    
    /* everything is ok */
    printk("[init] CPLD Ver:0x%x\n",version);
}

__IRAM_CODE void cpld_chg_state(int cpld_port,int bit_num,int direction)
{
    int tmp;
    
    /* LOCK */    
    tmp=cpld_port_state[cpld_port];
    if(direction)
        tmp |= (0x1 << bit_num);
    else
        tmp &= ~(0x1 << bit_num);
        
    if(tmp!=cpld_port_state[cpld_port])
    {
        cpld_port_state[cpld_port]=tmp;
        outw(cpld_port_state[cpld_port],cpld_port_array[cpld_port]);
    }
    /* UNLOCK */
}

__IRAM_CODE int cpld_read(int cpld_port)
{
    int val;
    /* LOCK */
    val=inw(cpld_port_array[cpld_port]);
    /* UNLOCK */
    return val;
}

__IRAM_CODE void cpld_select(int bit_num,int direction)
{
    int val;
    /* LOCK */
    if(direction)
        val = cpld_port_state[CPLD0] | (0x1 << bit_num);
    else
        val = cpld_port_state[CPLD0] & ~(0x1 << bit_num);
    if(val != cpld_port_state[CPLD0])
    {
        cpld_port_state[CPLD0]=val;
        cpld_do_select();
    }
    /* UNLOCK */
}

__IRAM_CODE void cpld_do_select(void)
{  
    int res,res2;
    /* LOCK */
    printk("changing cpld select : %d\n",cpld_port_state[CPLD0]);
    outw(cpld_port_state[CPLD0],CPLD_PORT0);
    outw(cpld_port_state[CPLD0],CPLD_PORT0);
    outw(cpld_port_state[CPLD0],CPLD_PORT0);
    outw(cpld_port_state[CPLD0],CPLD_PORT0);
    outw(cpld_port_state[CPLD0],CPLD_PORT0);
    
    res=inw(cpld_port_array[CPLD0]);
    while((res2=inw(cpld_port_array[CPLD0]))!=res) /* wait for the value to become stable */
        res=res2;
    /* UNLOCK */
}




