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

#define EVT_REDRAW      0x0100
#define EVT_QUIT        0x0101
#define EVT_SUSPEND     0x0102
#define EVT_RESUME      0x0103
#define EVT_CF_REMOVED  0x0104
#define EVT_CF_ADDED    0x0105
#define EVT_MENU_UP_LVL 0x0106

#define NO_APP      0
#define APP_MP3     1

#define MAX_PATH 256

#define MAX_OFF      10

struct wkUP_evt {
	int app;
        char arg[MAX_PATH];
};

void   addEventHandler  (void (*evtHandle));
void   pack             (void (*loopFct)(void));
void   myRelease_app    (void);
void   procNxtEvent     (int evt);
void   eventLoop        (void);

void   sendEvt(struct plugin * plug,int evt);

#endif
