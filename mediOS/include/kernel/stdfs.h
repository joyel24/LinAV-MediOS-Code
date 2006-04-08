/*
*   include/kernel/stdfs.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __STDFS_H
#define __STDFS_H

#include <sys_def/stdfs.h>

int       open(const char * name,int flags);
MED_RET_T close(int fdesc);
MED_RET_T truncate(int fdesc, unsigned int size);
int       write(int fdesc, const void* buf, unsigned int count);
int       read(int fdesc, const void* buf, unsigned int count);
MED_RET_T lseek(int fdesc, unsigned int offset, int whence);
int       ftell(int fdesc);
int       filesize(int fdesc);
MED_RET_T fsync(int fdesc);

/*
extern int      remove    (const char* pathname);
extern int      rename    (const char* path, const char* newname);
*/

DIR * opendir(char * pathname);
MED_RET_T closedir(DIR * fd);
struct dirent * readdir(DIR * fd);
int mkdir(const char *name, int mode);

/*
extern int mkdir(const char* name, int mode);
extern int rmdir(const char* name);
*/

#endif
