/*
* include/gui/widget.h
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

#ifndef __WIDGET_H
#define __WIDGET_H

#include <gui/arch_widget.h>

#include <sys_def/types.h>
#include <sys_def/colordef.h>
#include <sys_def/font.h>

#include <kernel/graphics.h>

// temp (should be arch dependant)
#define WIDGET_ACTION_BTN BTN_1
#define WIDGET_BACK_BTN BTN_2

// default parameters
#define WIDGET_DEFAULT_FORECOLOR COLOR_BLACK
#define WIDGET_DEFAULT_BACKCOLOR COLOR_WHITE
#define WIDGET_DEFAULT_FOCUSCOLOR COLOR_BLUE
#define WIDGET_DEFAULT_MARGIN 0
#define WIDGET_DEFAULT_FONT STD6X9

// members of the WIDGET object
#define WIDGET_MEMBERS                 \
    WIDGET_DESTROYER destroy;          \
    WIDGET_EVENTHANDLER handleEvent;   \
    WIDGET_PAINTHANDLER paint;         \
    int x;                             \
    int y;                             \
    int width;                         \
    int height;                        \
    int margin;                        \
    bool clearBackground;              \
    bool canFocus;                     \
    int focusPosition;                 \
    bool focused;                      \
    int foreColor;                     \
    int backColor;                     \
    int focusColor;                    \
    int font;

typedef bool(*WIDGET_EVENTHANDLER)(void *,int);
typedef void(*WIDGET_PAINTHANDLER)(void *);
typedef void(*WIDGET_DESTROYER)(void *);

typedef struct {
    WIDGET_MEMBERS
} * WIDGET;

WIDGET widget_create();
void widget_destroy(WIDGET w);
void widget_init(WIDGET w);
bool widget_handleEvent(WIDGET w,int evt);
void widget_paint(WIDGET w);

#endif
