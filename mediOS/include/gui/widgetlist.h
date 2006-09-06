/*
* include/gui/widgetlist.h
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

#ifndef __WIDGETLIST_H
#define __WIDGETLIST_H

#include <gui/widget.h>

typedef enum {WLD_NEXT, WLD_PREVIOUS} WL_DIRECTION;

//*****************************************************************************
// WIDGETLIST
//*****************************************************************************

// members of the WIDGETLIST object
#define WIDGETLIST_MEMBERS                                 \
    /* we inherit from WIDGET */                           \
    WIDGET_MEMBERS                                         \
                                                           \
    WIDGETLIST_WIDGETADDER addWidget;                      \
    WIDGETLIST_WIDGETSCLEARER clearWidgets;                \
    WIDGETLIST_INDEXGETTER indexOf;                        \
    WIDGETLIST_FOCUSEDWIDGETSETTER setFocusedWidget;       \
    WIDGETLIST_FOCUSCHANGER changeFocus;                   \
    WIDGET * widgets;                                      \
    int widgetCount;                                       \
    bool ownWidgets;                                       \
    WIDGET previousWidget;                                 \
    WIDGET focusedWidget;                                  \
    bool fastRepaint;                                      \
    bool repaintAllWidgets;

typedef void(*WIDGETLIST_WIDGETADDER)(void *,void *);
typedef void(*WIDGETLIST_WIDGETSCLEARER)(void *);
typedef int(*WIDGETLIST_INDEXGETTER)(void *,void *);
typedef void(*WIDGETLIST_FOCUSEDWIDGETSETTER)(void *,void *);
typedef void(*WIDGETLIST_FOCUSCHANGER)(void *,WL_DIRECTION);

typedef struct {
    WIDGETLIST_MEMBERS
} * WIDGETLIST;

#endif
