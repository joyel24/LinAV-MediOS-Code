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

#ifndef __K_ICONS_H
#define __K_ICONS_H

#include <gui/icons.h>

#include <kernel/shell.h>

#define ICON_DIR SHELL_DIR"/icons"

void icon_init(void);

struct icon_elem * icon_load(char * filename);
struct icon_elem * icon_add(char * name,unsigned char * data,int w,int h);
struct icon_elem * icon_get(char * name);

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
