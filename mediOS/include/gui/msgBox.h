/*
* kernel/gui/msgBox.h
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

#ifndef __MSGBOX_H
#define __MSGBOX_H

    #define MSGBOX_1BUTTON_MIN_WIDTH 160
    #define MSGBOX_2BUTTON_MIN_WIDTH 180
    #define MSGBOX_3BUTTON_MIN_WIDTH 260

    #define MSGBOX_HEIGHT     80


    // Return values
    #define MSGBOX_NOTHING 0
    #define MSGBOX_CANCEL  1
    #define MSGBOX_OK      2
    #define MSGBOX_NO      3
    #define MSGBOX_YES     4

    // Messagebox types
    #define MSGBOX_TYPE_OK          0
    #define MSGBOX_TYPE_OKCANCEL    1
    #define MSGBOX_TYPE_YESNO       2
    #define MSGBOX_TYPE_YESNOCANCEL 3

    // Messagebox icons
    #define MSGBOX_ICON_NO_ICON     0
    #define MSGBOX_ICON_EXCLAMATION 1
    #define MSGBOX_ICON_WARNING     2
    #define MSGBOX_ICON_INFORMATION 3
    #define MSGBOX_ICON_QUESTION    4
    #define MSGBOX_ICON_ERROR       5

    void iniMsgBox(void);
    int msgBox(unsigned char* caption, unsigned char* msg, int type, int icon,int evt_hanlder);

#endif
