/* 
*   include/fat.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
* Part of this code is from Rockbox project
* Copyright (C) 2002 by Björn Stenberg
*
*/

#ifndef __K_DIR_H_
#define __K_DIR_H_

#include <sys_def/types.h>
#include <sys_def/dir.h>

void init_dir(void);

extern DIR* kopendir(const char* name);
extern int kclosedir(DIR* dir);
extern int kmkdir(const char* name, int mode);
extern int krmdir(const char* name);

extern struct dirent* kreaddir(DIR* dir);

#endif
