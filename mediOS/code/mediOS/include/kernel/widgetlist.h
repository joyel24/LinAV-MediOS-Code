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

#ifndef __K_WIDGETLIST_H
#define __K_WIDGETLIST_H

#include <gui/widgetlist.h>

#include <kernel/widget.h>

//*****************************************************************************
// WIDGETLIST
//*****************************************************************************

WIDGETLIST widgetList_create();
void widgetList_destroy(WIDGETLIST l);
void widgetList_init(WIDGETLIST l);
bool widgetList_handleEvent(WIDGETLIST l,int evt);
void widgetList_paint(WIDGETLIST l);
void widgetList_addWidget(WIDGETLIST l,WIDGET w);
void widgetList_clearWidgets(WIDGETLIST l);
int widgetList_indexOf(WIDGETLIST l,WIDGET w);
void widgetList_setFocusedWidget(WIDGETLIST l,WIDGET w);
void widgetList_changeFocus(WIDGETLIST l,WL_DIRECTION dir);

#endif
