/*
* include/gui/chooser.h
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

#ifndef __CHOOSER_H
#define __CHOOSER_H

#include <gui/widget.h>

// members of the CHECKBOX object
#define CHOOSER_MEMBERS                \
    /* we inherit from WIDGET */       \
    WIDGET_MEMBERS                     \
                                       \
    CHOOSER_CHANGEEVENT onChange;      \
    char ** items;                     \
    int itemCount;                     \
    int index;

typedef void(*CHOOSER_CHANGEEVENT)(void *);

typedef struct {
    CHOOSER_MEMBERS
} * CHOOSER;

#endif
