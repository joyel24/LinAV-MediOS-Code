/*
* include/kernel/shell.h
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

#ifndef __SHELL_H
#define __SHELL_H

#include <sys_def/types.h>

#define SHELL_DIR "/mediOS"

#define SHELL_HANDLERS_FILE SHELL_DIR"/handlers.cfg"

#define SHELL_STATUS_HEIGHT 16

bool shell_execute(char * command,char * param);
void shell_restore();
void shell_main();

//debug
void shell_printHandlers();

#endif
