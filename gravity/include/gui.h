/* 
*   include/gui.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GRAVITY_GUI_H
#define __GRAVITY_GUI_H

typedef enum _E_GUI_CONTROL_TYPE
{
	eRECT = 0,
	eTEXT,
	eBUTTON
} E_GUI_CONTROL_TYPE;

typedef struct _S_GUI_CONTROL
{
	long x, y, w, h;
	const char* pszText;
	unsigned long type;
	unsigned long flags;
	unsigned long param;
} S_GUI_CONTROL;

/*

1) Task context operations: show, hide, z-move - goes to GFX manager

2) Set active task for user input

3) Set task context gui template, get/set template items params (text, states)

4) Get/set keyboard state

5) Get/set pointer device state

6) Character input

7) Popup-menus

8) Message-boxes

9) Cursor

10) app bar info

*/

#endif //__GRAVITY_GUI_H
