/* 
*   kernel/driver/ext_module.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/cpld.h>

#include <kernel/kernel.h>
#include <kernel/usb_fw.h>
#include <kernel/fm_remote.h>

#include <kernel/ext_module.h>

char * module_name[]= {
/* 0 */    "Unknown module",
/* 1 */    "MMC",
/* 2 */    "Unknown module",
/* 3 */    "SSFDC",
/* 4 */    "Unknown module",
/* 5 */    "MMC reader",
/* 6 */    "Unknown module",
/* 7 */    "SSFDC reader",
/* 8 */    "CFC",
/* 9 */    "Unknown module",
/* A */    "Video DVR",
/* B */    "Camera 3MG",
/* C */    "CFC reader",
/* D */    "Unknown module",
/* E */    "Unknown module",
/* F */    "NONE"
};

int known_module[]= {
/* 0 */    0,
/* 1 */    1,
/* 2 */    0,
/* 3 */    1,
/* 4 */    0,
/* 5 */    1,
/* 6 */    0,
/* 7 */    1,
/* 8 */    1,
/* 9 */    0,
/* A */    1,
/* B */    1,
/* C */    1,
/* D */    0,
/* E */    0,
/* F */    1
};

int connected_module=0;

struct module_actions *actions_array[NB_EXT_MODULES];

void process_ext_mod_chg(int res)
{
    if( 1 
#ifdef HAVE_FM_REMOTE   
    && !FM_is_connected()
#endif
     && !kFWIsConnected()
    )
    {
            if(res==AV_MODULE_NONE)
                do_mod_disconnect(connected_module);
            else
                do_mod_connect(res);
            connected_module=res;   
    }
    else
    {
        if(connected_module!=AV_MODULE_NONE)
        {
            do_mod_disconnect(connected_module);
            connected_module=AV_MODULE_NONE;
        }
    }
}

int get_connected_module(void)
{
    return connected_module;
}   

void do_mod_disconnect(int module_num)
{
    if(module_num==AV_MODULE_NONE)
    {
        printk("Error in module driver wrong disconnect\n");
    }
    else
    {
        printk("Module disconnected: %s (0x%02x)\n",module_name[module_num],module_num);
        if(actions_array[module_num])
            actions_array[module_num]->do_disconnection();
    }
}

void do_mod_connect(int module_num)
{
    if(module_num==AV_MODULE_NONE)
    {
        printk("Error in module driver wrong connect\n");
    }
    else
    {
        printk("New module detected: %s (0x%02x)\n",module_name[module_num],module_num);
        if(actions_array[module_num])
            actions_array[module_num]->do_connection();
    }
}

int ext_module_register_action(struct module_actions * action_struct,int module_num)
{
    if(module_num<0 || module_num>0xF)
        return 0;
    
    if(actions_array[module_num])
        printk("WARNING trying to register module 0x%02x while it is already register\n",module_num);
        
    actions_array[module_num]=action_struct;
    return 1;
}

int get_module(void)
{
    int res,res2;
    res=cpld_read(CPLD0);
    while((res2=(cpld_read(CPLD0)&0xF))!=res) /* wait for the value to become stable */
        res=res2;
    return (res&0xff);
}

void init_ext_module(void)
{
    /* initial state */
    int i;
    connected_module=0;
    for(i=0;i<NB_EXT_MODULES;i++)
        actions_array[i]=NULL;
        
    if( 1 
#ifdef HAVE_FM_REMOTE   
    && !FM_is_connected()
#endif
     && !kFWIsConnected()
    )
    {
        connected_module=get_module();        
    }
    else
        connected_module=AV_MODULE_NONE;

        
    printk("[init] external module (connected: %s)\n",module_name[connected_module]); 
}

