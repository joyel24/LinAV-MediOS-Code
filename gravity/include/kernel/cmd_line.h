/* 
*   include/kernel/uart.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __CMD_LINE_H
#define __CMD_LINE_H

struct cmd_line_s {
    char * cmd;
    char * help_str;
    void   (*cmd_action)(char ** params);
    int    nb_args;
};

/* cmd list */

void do_help(char ** params);
void do_mem (char ** params);
void do_run (char ** params);
void do_tasks (char ** params);
void do_restart (char ** params);

#endif
