/*
* include/kernel/internal_commands.h
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

#ifndef __INTERNAL_COMMANDS_H
#define __INTERNAL_COMMANDS_H

#include <sys_def/types.h>

bool intCmd_execute(char * command,char * param);

#endif
