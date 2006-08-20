/* 
*   kernel/driver/cf_module.c
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

#include <kernel/kernel.h>

#include <kernel/ext_module.h>
#include <kernel/cf_module.h>
#include <kernel/evt.h>

struct evt_t cf_evt;

void cf_connected(void)
{
    cf_evt.evt=EVT_CF_IN;
    cf_evt.evt_class=CONNECT_CLASS;
    cf_evt.data=0;
    evt_send(&cf_evt);
}

void cf_disconnected(void)
{
    cf_evt.evt=EVT_CF_OUT;
    cf_evt.evt_class=CONNECT_CLASS;
    cf_evt.data=0;
    evt_send(&cf_evt);
}

struct module_actions cf_module_actions = {
    do_connection:cf_connected,
    do_disconnection:cf_disconnected
};

void cf_initModule(void)
{
    if(!ext_module_register_action(&cf_module_actions,AV_MODULE_CFC))
    {
        printk("[CF module] error registering to module driver\n");
    }
    printk("[init] cf module\n");
}
