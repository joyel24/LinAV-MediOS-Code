/*
* avwm-menu.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __AVWM_MENU_H
#define __AVWM_MENU_H

#include "menu.h"
#include "plugin.h"

#define TYPE_STD    0
#define TYPE_BACK   1

struct cfg_menu {
	char name[MAX_TOKEN];
	char parent[MAX_TOKEN];
	char link[MAX_TOKEN];
	char param[MAX_TOKEN];
        int type;
	struct cfg_menu * nxt;
};

int                 ini_menu       (char * path,struct plugin * plug);
int                 loadMenu       (char * filename);
int                 do_parse       (struct cfg_menu ** cfg,char * filename);
void                cfgCleanMenu   (struct cfg_menu * cfg);
void                addItem        (struct cfg_menu ** cfg);
int                 insertItem     (struct menu_item * item);
void                cleanMenu      (struct menu_item * root);
struct menu_item *  findParent     (struct menu_item * ptr, char * name);
struct menu_item *  newItem        (struct cfg_menu * data);
void                printMenu      (void);
void                mk_submenu_str (void * data,char * str);
void                mk_item_str    (void * data,char * str);
void                do_right       (void * data);
void                do_on          (void * data);
void                do_off         (void * data);
void                do_F1          (void * data);
void                do_F2          (void * data);
void                do_F3          (void * data);
void                enableMenu     (void);
void                disableMenu    (void);
int                 menuStatus     (void);

#endif

