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

int  next_char(void);
void again(int ch);
int  nxt_token(char * buff);
int  nxt_cfg(char *item,char *value);

int  openFile(char * filename);
void closeFile(void);


#endif
