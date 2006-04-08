/*
* include/gui/icons.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GUI_ICONS_H
#define __GUI_ICONS_H

#include <sys_def/graphics.h>

struct icon_elem {
    char * name;
    BITMAP bmap_data;
    unsigned char * data;
    struct icon_elem * nxt;
};

void iniIcon(void);

struct icon_elem * loadIcon(char * filename);
struct icon_elem * addIcon(char * name,unsigned char * data,int w,int h);
struct icon_elem * getIcon(char * name);

/* icon list
- linavLogo
- usbIcon
- powerIcon
- upBitmap
- dwBitmap
- dirBitmap
- mp3Bitmap
- textBitmap
- imageBitmap
- MsgBoxExclamationBitmap
- MsgBoxQuestionBitmap
- MsgBoxWarningBitmap
- MsgBoxInformationBitmap
- MsgBoxErrorBitmap
*/

#endif
