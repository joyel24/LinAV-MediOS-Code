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

// paint parameters
#define TRACKBAR_SPACING 2 // space between trackbar and value
#define TRACKBAR_CURSOR_RATIO (1/1.618) // h/w ratio

// members of the TRACKBAR object
#define TRACKBAR_MEMBERS               \
    /* we inherit from WIDGET */       \
    WIDGET_MEMBERS                     \
                                       \
    int value;                         \
    int minimum;                       \
    int maximum;                       \
    int increment;                     \
    int numTicks;                      \
    int numValueDigits;

typedef struct {
    TRACKBAR_MEMBERS
} * TRACKBAR;

TRACKBAR trackbar_create();
void trackbar_destroy(TRACKBAR t);
void trackbar_init(TRACKBAR t);
bool trackbar_handleEvent(TRACKBAR t,int evt);
void trackbar_paint(TRACKBAR t);
void trackbar_setValue(TRACKBAR t,int value);

#endif
