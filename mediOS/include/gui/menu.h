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

#include <gui/widget.h>
#include <gui/widgetlist.h>

// forward declaration needed because MENU_ITEM has a MENU member and vice-versa
typedef struct MENU_STRUCT * MENU;

//*****************************************************************************
// MENU_ITEM
//*****************************************************************************

// members of the MENU_ITEM object
#define MENU_ITEM_MEMBERS              \
    /* we inherit from WIDGET */       \
    WIDGET_MEMBERS                     \
                                       \
    char * caption;                    \
    MENU subMenu;

typedef struct {
    MENU_ITEM_MEMBERS
} * MENU_ITEM;

//*****************************************************************************
// MENU
//*****************************************************************************

// members of the MENU object
#define MENU_MEMBERS                   \
    /* we inherit from WIDGET */       \
    WIDGET_MEMBERS                     \
                                       \
    MENU_CLICKEVENT onClick;           \
    MENU_ITEMADDER addItem;            \
    MENU_ITEMSCLEARER clearItems;      \
    MENU_INDEXGETTER indexOf;          \
    MENU_INDEXFROMCAPTIONGETTER indexFromCaption;\
    MENU_ITEM * items;                 \
    int itemCount;                     \
    bool ownItems;                     \
    int index;                         \
    int previousIndex;                 \
    int topIndex;                      \
    int visibleCount;                  \
    bool fastRepaint;                  \
    WIDGETLIST menuList;               \
    MENU parentMenu;


typedef void(*MENU_CLICKEVENT)(void *,void *);
typedef void(*MENU_ITEMADDER)(void *,void *);
typedef void(*MENU_ITEMSCLEARER)(void *);
typedef int(*MENU_INDEXGETTER)(void *,void *);
typedef int(*MENU_INDEXFROMCAPTIONGETTER)(void *,char *);

struct MENU_STRUCT {
    MENU_MEMBERS
};

#endif
