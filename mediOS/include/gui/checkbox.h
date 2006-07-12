/*
* include/gui/checkbox.h
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

#ifndef __CHECKBOX_H
#define __CHECKBOX_H

#include <gui/widget.h>

// paint parameters
#define CHECKBOX_SPACING 2 // space between text and box

// members of the CHECKBOX object
#define CHECKBOX_MEMBERS               \
    /* we inherit from WIDGET */       \
    WIDGET_MEMBERS                     \
                                       \
    char * caption;                    \
    bool checked;

typedef struct {
    CHECKBOX_MEMBERS
} * CHECKBOX;

CHECKBOX checkbox_create();
void checkbox_destroy(CHECKBOX c);
void checkbox_init(CHECKBOX c);
bool checkbox_handleEvent(CHECKBOX c,int evt);
void checkbox_paint(CHECKBOX c);

#endif
