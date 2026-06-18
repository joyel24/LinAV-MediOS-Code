/*
* include/kernel/shellmenu.h
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

#ifndef __SHELLMENU_H
#define __SHELLMENU_H

#include <kernel/shell.h>

#include <sys_def/types.h>

#define SHELL_MENU_FILE SHELL_DIR"/menu.cfg"

#define SHELL_FOLDER_ICON "sub_icon.ico"
#define SHELL_BACK_ICON "back_icon.ico"
#define SHELL_APP_ICON "plugin_icon.ico"

#define SHELL_MENU_ICONVIEW_WIDTH 64
#define SHELL_MENU_ICONVIEW_HEIGHT 64

#define SHELL_MENU_LISTVIEW_WIDTH LCD_WIDTH
#define SHELL_MENU_LISTVIEW_HEIGHT 16

void shellMenu_handleEvent(int event);
bool shellMenu_init();
void shellMenu_close();

//debug
void shellMenu_printItems();

#endif
