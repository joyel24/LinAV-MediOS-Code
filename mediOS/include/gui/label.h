/*
* include/gui/label.h
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

#ifndef __LABEL_H
#define __LABEL_H

#include<gui/widget.h>

typedef enum {LA_LEFT, LA_CENTER, LA_RIGHT} LABEL_ALIGNMENT;

//*****************************************************************************
// LABEL
//*****************************************************************************

// members of the LABEL object
#define LABEL_MEMBERS          \
    /* we inherit from MENUITEM */     \
    WIDGET_MEMBERS                     \
                                       \
    char * caption;                    \
    LABEL_ALIGNMENT alignment;

typedef struct {
    LABEL_MEMBERS
} * LABEL;

LABEL label_create();
void label_init(LABEL l);
void label_paint(LABEL l);

#endif
