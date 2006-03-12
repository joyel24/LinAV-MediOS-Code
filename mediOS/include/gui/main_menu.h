/*
* include/gui/main_menu.h
*
* MediOS project
* Copyright (c) 2005 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __MAIN_MENU_H
#define __MAIN_MENU_H

#include <gui/gui.h>
#include <gui/menu.h>
#include <gui/icons.h>



struct cfg_menu {
    char name[MAX_TOKEN];
    char parent[MAX_TOKEN];
    char link[MAX_TOKEN];
    char param[MAX_TOKEN];
    int type;
    struct icon_elem * icon;
    struct cfg_menu * nxt;
};

void               mainMenu_doOff(void * data);
void               mainMenu_doOn(void * data);
void               mainMenu_doAction(void * data);
void               mainMenu_doF1(void * data);
void               mainMenu_doF2(void * data);
void               mainMenu_doF3(void * data);
void               mainMenu_mkSubmenuStr(void * data,char * str);
void               mainMenu_mkItemStr(void * data,char * str);
BITMAP *           mainMenu_mkSubIcon(void * data);
BITMAP *           mainMenu_mkItemIcon(void * data);
int                mainMenu_ini(void);
void               mainMenu_clean(struct menu_item * root);
struct menu_item * mainMenu_newItem(struct cfg_menu * data);
struct menu_item * mainMenu_findParent(struct menu_item * ptr, char * name);
int                mainMenu_insertItem(struct menu_item * item);
void               mainMenu_addItem(struct cfg_menu ** cfg);
void               mainMenu_cleanCfg(struct cfg_menu * cfg);
int                mainMenu_parse(struct cfg_menu ** cfg,char * filename);
int                mainMenu_doAddBackEntry(char * name,struct menu_item * up,struct menu_item *cur);
int                mainMenu_load(char * filename);
void               mainMenu_print(void);
void               mainMenu_dispose(void);
void               mainMenu_start(void);
void               mainMenu_loop(void);
#endif

