/* 
*   cmd_line.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __CMD_LINE_H
#define __CMD_LINE_H

#include <readline/readline.h>
#include <readline/history.h>

#include "emu.h"

class Cpu;

typedef struct cmd_line_elem {
  char *name;
  int (*func)(char * arg);
  char *doc;
  cmd_line_elem * nxt;
} COMMAND;

void init_cmd_line(void);
void cmd_line(void);
void add_cmd_fct(char * name,int(*fct)(char*),char * help_str);
void print_cmd_list(void);

int my_atoi(char * string);

#endif

