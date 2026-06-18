/*
* include/gui/button.h
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

#ifndef __BUTTON_H
#define __BUTTON_H

#include <gui/widget.h>

// members of the CHECKBOX object
#define BUTTON_MEMBERS               \
    /* we inherit from WIDGET */       \
    WIDGET_MEMBERS                     \
                                       \
    BUTTON_CLICKEVENT onClick;         \
    char * caption;

typedef void(*BUTTON_CLICKEVENT)(void *);

typedef struct {
    BUTTON_MEMBERS
} * BUTTON;

#endif
