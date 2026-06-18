/*
* include/kernel/iconmenu.h
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

#ifndef __K_ICONMENU_H
#define __K_ICONMENU_H

#include <gui/iconmenu.h>

#include <kernel/menu.h>

//*****************************************************************************
// ICONMENU_ITEM
//*****************************************************************************

// paint parameters
#define ICONMENU_ITEM_SPACING 2 // space between icon and text in IMIP_LEFT

ICONMENU_ITEM iconMenuItem_create();
void iconMenuItem_destroy(ICONMENU_ITEM mi);
void iconMenuItem_init(ICONMENU_ITEM mi);
void iconMenuItem_paint(ICONMENU_ITEM mi);

//*****************************************************************************
// ICONMENU
//*****************************************************************************

ICONMENU iconMenu_create();
void iconMenu_destroy(ICONMENU m);
void iconMenu_init(ICONMENU m);
bool iconMenu_handleEvent(ICONMENU m,int evt);
void iconMenu_addItem(ICONMENU m, ICONMENU_ITEM item);
void iconMenu_updateItems(ICONMENU m, bool fast);

#endif
