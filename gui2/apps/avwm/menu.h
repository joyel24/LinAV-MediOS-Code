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

#ifndef __MENU_H
#define __MENU_H

struct cfg_menu {
	char name[MAX_TOKEN];
	char parent[MAX_TOKEN];
	char link[MAX_TOKEN];
	char param[MAX_TOKEN];
	struct cfg_menu * nxt;
};

void doDraw(void);
int  loadMenu(void);
void printMenu(void);
int  do_parse(struct cfg_menu ** cfg,char * filename);
void cfgCleanMenu(struct cfg_menu * cfg);
void addItem(struct cfg_menu ** cfg);

#endif
