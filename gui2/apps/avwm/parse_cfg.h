/*
* parse_cfg.h
*
* avos - http://avos.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __PARSE_CFG_H
#define __PARSE_CFG_H

#define MAX_TOKEN 250

struct cfg_item {
	char name[MAX_TOKEN];
	char parent[MAX_TOKEN];
	char link[MAX_TOKEN];
	char param[MAX_TOKEN];
	struct cfg_item * nxt;
};

int  next_char(void);
void again(int ch);
int  nxt_token(char * buff);
int  nxt_cfg(char *item,char *value);
int  do_parse(struct cfg_item ** cfg,char * filename);
void cfgCleanMenu(struct cfg_item * cfg);


#endif
