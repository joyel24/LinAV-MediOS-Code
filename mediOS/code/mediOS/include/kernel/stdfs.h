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

/* file related */
int       open(char * name,int flags);
MED_RET_T close(int fdesc);
MED_RET_T truncate(int fdesc, unsigned int size);
int       write(int fdesc, const void* buf, unsigned int count);
int       read(int fdesc, const void* buf, unsigned int count);
MED_RET_T lseek(int fdesc, unsigned int offset, int whence);
int       ftell(int fdesc);
int       filesize(int fdesc);
MED_RET_T fsync(int fdesc);

/*
extern int      rename    (const char* path, const char* newname);
*/

/* folder related */
DIR * opendir(char * pathname);
MED_RET_T closedir(DIR * fd);
struct dirent * readdir(DIR * fd);

/* file/dir ops */
MED_RET_T mkdir(char *name, int mode);
MED_RET_T rmdir(char* name);
MED_RET_T mvdir(char* path, char* newpath);

MED_RET_T rmfile(char* name);
MED_RET_T mvfile(char* path, char* newpath);
#endif
