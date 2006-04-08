/* 
*   include/kernel/ext_module.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __EXT_MODULE_H
#define __EXT_MODULE_H

#define AV_MODULE_NONE      0x0f
#define AV_MODULE_CFC_R     0x0c
#define AV_MODULE_CAM       0x0b
#define AV_MODULE_DVR       0x0a
#define AV_MODULE_CFC       0x08
#define AV_MODULE_SSFDC_R   0x07
#define AV_MODULE_MMC_R     0x05
#define AV_MODULE_SSFDC     0x03
#define AV_MODULE_MMC       0x01

#define NB_EXT_MODULES      0x10

/********************************************* user functions */

int  get_connected_module(void);

struct module_actions {
    void (*do_connection)(void);
    void (*do_disconnection)(void);
};

int  ext_module_register_action(struct module_actions * action_struct,int module_num);

/****************************************** end user functions */

/****************************************** private functions */

void do_mod_connect(int module_num);
void do_mod_disconnect(int module_num);
int  get_module(void);
void init_ext_module(void);
void process_ext_mod_chg(int res);

extern int known_module[];
extern int connected_module;

#define EXT_MODULE_CHK    {   \
    int __res=get_module();   \
    if(known_module[__res] && __res!=connected_module) \
        process_ext_mod_chg(__res);  \
}

/************************************** end private functions */

#endif
