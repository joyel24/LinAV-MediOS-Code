/*
*   include/parse_cfg.h
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __PARSE_CFG_H
#define __PARSE_CFG_H

#define MAX_TOKEN 60
#define MAX_CFG 10

extern int line_num;

int file_open(char *name);
void file_close();
char next_char(void);
void again(int ch);
int nxt_token(char * buff);
int nxt_cfg(char *item,char *value);


#endif
