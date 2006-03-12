/*
* include/gui/menu.h
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

#ifndef __MENU_H
#define __MENU_H

#include <gui/icons.h>

struct menu_item {
    void * data;
    struct menu_item * nxt;
    struct menu_item * prev;
    struct menu_item * sub;
    struct menu_item * up;
};

struct menu_data {
	struct menu_item * root;
        int useOwnDisp;
        int isTxtMenu;
        int width;int height;int x;int y;
        int dx;int dy;
        int txt_color;
        int bg_color;
        int select_color;
        int sub_color;
        int border_color;
        char * title;
        int has_border;
        int font;
        void (*do_action) (void * data);
        void (*on_action)    (void * data);
        void (*off_action)   (void * data);
        void (*f1_action)   (void * data);
        void (*f2_action)   (void * data);
        void (*f3_action)   (void * data);
        void (*item_str)     (void * data,char * str);
        void (*submenu_str)  (void * data,char * str);
        BITMAP * (*getSubIcon) (void * data);
        BITMAP * (*getItemIcon) (void * data);
};

void stop_menu(void);
void start_menu(struct menu_data * client_menu);

void menu_EvtHandler   (int evt);
void normMenu_handler (int evt);
void iconMenu_handler (int evt);

void doPrint          (struct menu_item * ptr,int level);

void dispAName_norm   (struct menu_item * pos, int posY, int clear, int selected);
void dispAllName_norm (struct menu_item * pos,int nselect);
int  dispName_norm    (struct menu_item * item,int x,int y,int clear,int selected);

void dispAllName_icon (struct menu_item * pos,int nselect);
int  dispName_icon    (struct menu_item * item,int i,int j,int clear_txt,int clear_icon,int selected);

#endif

