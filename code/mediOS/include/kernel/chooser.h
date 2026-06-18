/*
* include/kernel/chooser.h
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

#ifndef __K_CHOOSER_H
#define __K_CHOOSER_H

#include <gui/chooser.h>

#include <kernel/widget.h>

CHOOSER chooser_create();
void chooser_destroy(CHOOSER c);
void chooser_init(CHOOSER c);
bool chooser_handleEvent(CHOOSER c,int evt);
void chooser_paint(CHOOSER c);

#endif
