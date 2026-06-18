/*
* include/gui/trackbar.h
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

#ifndef __TRACKBAR_H
#define __TRACKBAR_H

#include <gui/widget.h>

// members of the TRACKBAR object
#define TRACKBAR_MEMBERS               \
    /* we inherit from WIDGET */       \
    WIDGET_MEMBERS                     \
                                       \
    TRACKBAR_CHANGEEVENT onChange;     \
    int value;                         \
    int minimum;                       \
    int maximum;                       \
    int increment;                     \
    int numTicks;                      \
    int numValueDigits;

typedef void(*TRACKBAR_CHANGEEVENT)(void *);

typedef struct {
    TRACKBAR_MEMBERS
} * TRACKBAR;

#endif
