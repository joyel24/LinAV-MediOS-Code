/*
* include/gui/textmenu.h
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

#ifndef __TEXTMENU_H
#define __TEXTMENU_H

#include<gui/menu.h>

typedef enum {TMA_LEFT, TMA_CENTER, TMA_RIGHT} TM_ALIGNMENT;

//*****************************************************************************
// TEXTMENU_ITEM
//*****************************************************************************

// members of the TEXTMENUITEM object
#define TEXTMENU_ITEM_MEMBERS          \
    /* we inherit from MENUITEM */     \
    MENU_ITEM_MEMBERS                  \
                                       \
    TM_ALIGNMENT alignment;

typedef struct {
    TEXTMENU_ITEM_MEMBERS
} * TEXTMENU_ITEM;

TEXTMENU_ITEM textMenuItem_create();
void textMenuItem_destroy(TEXTMENU_ITEM mi);
void textMenuItem_init(TEXTMENU_ITEM mi);
void textMenuItem_paint(TEXTMENU_ITEM mi);

//*****************************************************************************
// TEXTMENU
//*****************************************************************************

// members of the TEXTMENU object
#define TEXTMENU_MEMBERS               \
    /* we inherit from MENU */         \
    MENU_MEMBERS                       \
                                       \
    int itemHeight;

typedef struct {
    TEXTMENU_MEMBERS
} * TEXTMENU;

TEXTMENU textMenu_create();
void textMenu_destroy(TEXTMENU m);
void textMenu_init(TEXTMENU m);
bool textMenu_handleEvent(TEXTMENU m,int evt);
void textMenu_addItem(TEXTMENU m, TEXTMENU_ITEM item);
void textMenu_updateItems(TEXTMENU m, bool fast);

#endif
