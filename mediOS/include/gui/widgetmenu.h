/*
* include/gui/widgetmenu.h
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

#ifndef __WIDGETMENU_H
#define __WIDGETMENU_H

#include <gui/menu.h>
#include <gui/textmenu.h>
#include <gui/checkbox.h>
#include <gui/trackbar.h>
#include <gui/chooser.h>

//*****************************************************************************
// WIDGETMENU_ITEM
//*****************************************************************************

// members of the WIDGETMENUITEM object
#define WIDGETMENU_ITEM_MEMBERS                            \
    /* we inherit from MENUITEM */                         \
    MENU_ITEM_MEMBERS                                      \
                                                           \
    WIDGETMENU_ITEM_CFGGETTER cfgToString;                 \
    WIDGETMENU_ITEM_CFGSETTER cfgFromString;               \
    WIDGET widget;                                         \
    bool cfgStored;                                        \
    char * cfgName;                                        \
    int widgetWidth;  /* in 1/256th of the item width */

typedef void(*WIDGETMENU_ITEM_CFGGETTER)(void *,char *);
typedef void(*WIDGETMENU_ITEM_CFGSETTER)(void *,char *);

typedef struct {
    WIDGETMENU_ITEM_MEMBERS
} * WIDGETMENU_ITEM;

//*****************************************************************************
// WIDGETMENU_CHECKBOX
//*****************************************************************************

// members of the WIDGETMENU_CHECKBOX object
#define WIDGETMENU_CHECKBOX_MEMBERS                        \
    /* we inherit from WIDGETMENU_ITEM */                  \
    WIDGETMENU_ITEM_MEMBERS                                \
                                                           \
    CHECKBOX checkbox;

typedef struct {
    WIDGETMENU_CHECKBOX_MEMBERS
} * WIDGETMENU_CHECKBOX;

//*****************************************************************************
// WIDGETMENU_TRACKBAR
//*****************************************************************************

// members of the WIDGETMENU_TRACKBAR object
#define WIDGETMENU_TRACKBAR_MEMBERS                        \
    /* we inherit from WIDGETMENU_ITEM */                  \
    WIDGETMENU_ITEM_MEMBERS                                \
                                                           \
    TRACKBAR trackbar;

typedef struct {
    WIDGETMENU_TRACKBAR_MEMBERS
} * WIDGETMENU_TRACKBAR;

//*****************************************************************************
// WIDGETMENU_CHOOSER
//*****************************************************************************

// members of the WIDGETMENU_CHOOSER object
#define WIDGETMENU_CHOOSER_MEMBERS                         \
    /* we inherit from WIDGETMENU_ITEM */                  \
    WIDGETMENU_ITEM_MEMBERS                                \
                                                           \
    CHOOSER chooser;

typedef struct {
    WIDGETMENU_CHOOSER_MEMBERS
} * WIDGETMENU_CHOOSER;

//*****************************************************************************
// WIDGETMENU
//*****************************************************************************

// members of the WIDGETMENU object
#define WIDGETMENU_MEMBERS                                 \
    /* we inherit from TEXTMENU */                         \
    TEXTMENU_MEMBERS                                       \
                                                           \
    WIDGETMENU_CHECKBOXGETTER getCheckbox;                 \
    WIDGETMENU_TRACKBARGETTER getTrackbar;                 \
    WIDGETMENU_CHOOSERGETTER getChooser;                   \
    WIDGETMENU_CFGLOADER cfgLoad;                          \
    WIDGETMENU_CFGSAVER cfgSave;

typedef CHECKBOX(*WIDGETMENU_CHECKBOXGETTER)(void *,int);
typedef TRACKBAR(*WIDGETMENU_TRACKBARGETTER)(void *,int);
typedef CHOOSER(*WIDGETMENU_CHOOSERGETTER)(void *,int);
typedef bool(*WIDGETMENU_CFGLOADER)(void *,char *);
typedef bool(*WIDGETMENU_CFGSAVER)(void *,char *);

typedef struct {
    WIDGETMENU_MEMBERS
} * WIDGETMENU;

#endif
