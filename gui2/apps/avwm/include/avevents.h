/*
* avevents.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __AVEVENTS_H
#define __AVEVENTS_H

#include "plugin.h"

#define EVT_REDRAW  0x0100
#define EVT_QUIT    0x0101
#define EVT_SUSPEND 0x0102
#define EVT_RESUME  0x0103

void   addEventHandler  (void (*evtHandle));
void   pack             (void);
void   eventLoop        (void);

void   sendEvt(struct plugin * plug,int evt);

#endif