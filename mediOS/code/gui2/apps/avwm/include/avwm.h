/*
* avwm.h
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

#ifndef __AVWM_H
#define __AVWM_H

#define MENU_SHADOW 2 // height of small shadow under the title

void   showSBar         (void);
void   hideSBar         (void);
int    sBarStatus       (void);

void   enableMenu       (void);
void   disableMenu      (void);
int    menuStatus       (void);

char*  getPath          (char * str);

#endif

