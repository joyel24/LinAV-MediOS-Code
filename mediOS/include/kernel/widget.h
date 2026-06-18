/*
* include/kernel/widget.h
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

#ifndef __K_WIDGET_H
#define __K_WIDGET_H

#include <gui/widget.h>

#include <kernel/graphics.h>

WIDGET widget_create();
void widget_destroy(WIDGET w);
void widget_init(WIDGET w);
bool widget_handleEvent(WIDGET w,int evt);
void widget_paint(WIDGET w);
void widget_setRect(WIDGET w,int x,int y,int width,int height);

#endif
