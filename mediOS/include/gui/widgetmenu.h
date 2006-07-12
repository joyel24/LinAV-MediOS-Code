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

#define WIDGETMENU_WIDGET_MARGIN 1 // margin around a widget in a menu item

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

WIDGETMENU_ITEM widgetMenuItem_create();
void widgetMenuItem_destroy(WIDGETMENU_ITEM mi);
void widgetMenuItem_init(WIDGETMENU_ITEM mi);
bool widgetMenuItem_handleEvent(WIDGETMENU_ITEM mi,int evt);
void widgetMenuItem_paint(WIDGETMENU_ITEM mi);
void widgetMenuItem_cfgToString(WIDGETMENU_ITEM mi,char * s);
void widgetMenuItem_cfgFromString(WIDGETMENU_ITEM mi,char * s);

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

WIDGETMENU_CHECKBOX widgetMenuCheckbox_create();
void widgetMenuCheckbox_destroy(WIDGETMENU_CHECKBOX mc);
void widgetMenuCheckbox_init(WIDGETMENU_CHECKBOX mc);
void widgetMenuCheckbox_cfgToString(WIDGETMENU_CHECKBOX mc,char * s);
void widgetMenuCheckbox_cfgFromString(WIDGETMENU_CHECKBOX mc,char * s);

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

WIDGETMENU_TRACKBAR widgetMenuTrackbar_create();
void widgetMenuTrackbar_destroy(WIDGETMENU_TRACKBAR mt);
void widgetMenuTrackbar_init(WIDGETMENU_TRACKBAR mt);
void widgetMenuTrackbar_cfgToString(WIDGETMENU_TRACKBAR mt,char * s);
void widgetMenuTrackbar_cfgFromString(WIDGETMENU_TRACKBAR mt,char * s);

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

WIDGETMENU_CHOOSER widgetMenuChooser_create();
void widgetMenuChooser_destroy(WIDGETMENU_CHOOSER mc);
void widgetMenuChooser_init(WIDGETMENU_CHOOSER mc);
void widgetMenuChooser_cfgToString(WIDGETMENU_CHOOSER mc,char * s);
void widgetMenuChooser_cfgFromString(WIDGETMENU_CHOOSER mc,char * s);

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

WIDGETMENU widgetMenu_create();
void widgetMenu_destroy(WIDGETMENU m);
void widgetMenu_init(WIDGETMENU m);
bool widgetMenu_handleEvent(WIDGETMENU m,int evt);
CHECKBOX widgetMenu_getCheckbox(WIDGETMENU m,int index);
TRACKBAR widgetMenu_getTrackbar(WIDGETMENU m,int index);
CHOOSER widgetMenu_getChooser(WIDGETMENU m,int index);
bool widgetMenu_cfgLoad(WIDGETMENU m,char * filename);
bool widgetMenu_cfgSave(WIDGETMENU m,char * filename);

#endif
