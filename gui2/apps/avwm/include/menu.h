/*
* menu.h
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

#ifndef __MENU_H
#define __MENU_H

struct menu_item {
    void * data;
    struct menu_item * nxt;
    struct menu_item * prev;
    struct menu_item * sub;
    struct menu_item * up;
};

struct menu_data {
	struct menu_item * root;
        void (*right_action) (void * data);
        void (*on_action)    (void * data);
        void (*off_action)   (void * data);
        void (*item_str)     (void * data,char * str);
        void (*submenu_str)  (void * data,char * str);
};

void menuEvtHandler  (int evt);
void doPrint         (struct menu_item * ptr,int level);
void printAName      (struct menu_item * pos, int posY, int clear, int selected);
void printAllName    (struct menu_item * pos,int nselect);
int  printName       (struct menu_item * item,int x,int y,int clear,int selected);

#endif
