/*
* include/kernel/menu.h
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

#ifndef __K_MENU_H
#define __K_MENU_H

#include <gui/menu.h>

#include <kernel/widget.h>

//*****************************************************************************
// MENU_ITEM
//*****************************************************************************

MENU_ITEM menuItem_create();
void menuItem_destroy(MENU_ITEM mi);
void menuItem_init(MENU_ITEM mi);

//*****************************************************************************
// MENU
//*****************************************************************************

MENU menu_create();
void menu_destroy(MENU m);
void menu_init(MENU m);
bool menu_handleEvent(MENU m,int evt);
void menu_paint(MENU m);
void menu_addItem(MENU m, MENU_ITEM item);
void menu_clearItems(MENU m);
int menu_indexOf(MENU m, MENU_ITEM item);
int menu_indexFromCaption(MENU m, char * caption);

#endif
