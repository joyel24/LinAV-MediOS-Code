/*
* include/gui/parse_cfg.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GUI_PARSE_CFG_H
#define __GUI_PARSE_CFG_H

#include <sys_def/stdfs.h>



/* reading */
int  next_char(void);
void again(int ch);
int  nxt_token(char * buff);
int  nxt_cfg(char *item,char *value);

/* writting */
#define CFG_READ  O_RDONLY
#define CFG_WRITE O_WRONLY

int write_cfg(char * item,char * value);
int write_comment(char * cmt);
int add_line(void);

/* file handling */

int  openFile(char * filename,int mode);
void closeFile(void);

int curLineNum(void);

#endif

