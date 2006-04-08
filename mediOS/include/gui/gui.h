/*
* include/gui/gui.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GUI_GUI_H
#define __GUI_GUI_H


#define TYPE_STD      0
#define TYPE_BACK     1
#define TYPE_DIR      2
#define TYPE_FILE     3

#define TYPE_INTERNAL 4

#define LEFT_SCROLL  0
#define RIGHT_SCROLL 1

#define PATHLEN    256

#define BRW_MODE    0
#define MENU_MODE   1
#define CP_MV_MODE  2

#define MODE_COPY   0
#define MODE_MOVE   1

#define MAX_TOKEN 250

#define MODE_SELECT   0
#define MODE_NOSELECT 1
#define MODE_STRING   2

void gui_start(void);
void gui_sendEvt(int evt_num);

/* status line */
void statusBar_ini(void);
void statusLine_EvtHandler(int evt);

#endif
