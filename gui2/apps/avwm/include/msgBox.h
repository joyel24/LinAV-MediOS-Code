/*
* msgBox.h
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

#ifndef __MSGBOX_H
#define __MSGBOX_H

    // Return values
    #define MSGBOX_CANCEL 0
    #define MSGBOX_OK     1
    #define MSGBOX_NO     2
    #define MSGBOX_YES    3

    // Messagebox types
    #define MSGBOX_TYPE_OK          0
    #define MSGBOX_TYPE_OKCANCEL    1
    #define MSGBOX_TYPE_YESNO       2
    #define MSGBOX_TYPE_YESNOCANCEL 3

    // Messagebox icons
    #define MSGBOX_ICON_EXCLAMATION 0
    #define MSGBOX_ICON_WARNING     1
    #define MSGBOX_ICON_INFORMATION 2
    #define MSGBOX_ICON_QUESTION    3
    #define MSGBOX_ICON_ERROR       4

    void iniMsgBox(void);
    int msgBox(unsigned char* caption, unsigned char* msg, int type, int icon);

#endif
