/* 
*   kernel/driver/dvr_module.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/kernel.h>

#include <kernel/cpld.h>
#include <kernel/irq.h>
#include <kernel/i2c.h>
#include <kernel/timer.h>
#include <kernel/ext_module.h>
#include <kernel/ir_remote.h>

#include <kernel/dvr_module.h>
int init_done=0;

void dvr_connected(void)
{
    if(init_done)
    {
        CPLD_SET_PORT0(CPLD_DVR);
        CPLD_SET_PORT3(CPLD3_DVR);
        CPLD_SET_PORT2(CPLD2_DVR);
        start_ir_remote();
        printk("DVR connect done (chip ver %x)\n",dvr_chip_version());
    }
    else
        printk("DVR can't do connect => wait for init\n");
}

void dvr_disconnected(void)
{
    if(init_done)
    {
        CPLD_CLEAR_PORT0(CPLD_DVR);
        CPLD_CLEAR_PORT3(CPLD3_DVR);
        CPLD_CLEAR_PORT2(CPLD2_DVR);
        stop_ir_remote();
        printk("DVR disconnect done\n");
    }
    else
        printk("DVR can't do disconnect => wait for init\n");
}

struct module_actions dvr_module_actions = {
    do_connection:dvr_connected,
    do_disconnection:dvr_disconnected
};

void init_dvr_module(void)
{
    if(!ext_module_register_action(&dvr_module_actions,AV_MODULE_DVR))
    {
        printk("[DVR module] error registering to module driver\n");
    }
    
    init_ir_remote();
    
    init_done=1;
    /* check if connected at boot*/
    if(get_connected_module()==AV_MODULE_DVR)
        dvr_connected();
       
    printk("[init] dvr module\n");
}

/* saa7113h chip code */

int dvr_chip_version(void)
{
    char c;
    i2c_read(DVR_DEVICE,0,&c,1);
    /* this returns 0 while the char is egal to 3 
    => is chip version in first half of byte?
    => do we have to sawp each byte? */
    return (int)((c>>4)&0xF);
}

