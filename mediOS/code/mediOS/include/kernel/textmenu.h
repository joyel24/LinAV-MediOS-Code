/*
* include/kernel/textmenu.h
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

#ifndef __K_TEXTMENU_H
#define __K_TEXTMENU_H

#include <gui/textmenu.h>

#include <kernel/menu.h>

//*****************************************************************************
// TEXTMENU_ITEM
//*****************************************************************************

TEXTMENU_ITEM textMenuItem_create();
void textMenuItem_destroy(TEXTMENU_ITEM mi);
void textMenuItem_init(TEXTMENU_ITEM mi);
void textMenuItem_paint(TEXTMENU_ITEM mi);

//*****************************************************************************
// TEXTMENU
//*****************************************************************************

TEXTMENU textMenu_create();
void textMenu_destroy(TEXTMENU m);
void textMenu_init(TEXTMENU m);
bool textMenu_handleEvent(TEXTMENU m,int evt);
void textMenu_addItem(TEXTMENU m, TEXTMENU_ITEM item);
void textMenu_updateItems(TEXTMENU m, bool fast);

#endif
