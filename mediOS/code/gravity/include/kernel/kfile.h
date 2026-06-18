/* 
*   include/kernel/kfile.h
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

#ifndef __K_FILE_H_
#define __K_FILE_H_

#include <sys_def/types.h>
#include <sys_def/file.h>

/*
typedef int (*open_func)(const char* pathname, int flags);
typedef ssize_t (*read_func)(int fd, void *buf, size_t count);
typedef int (*creat_func)(const char *pathname, mode_t mode);
typedef ssize_t (*write_func)(int fd, const void *buf, size_t count);
typedef void (*qsort_func)(void *base, size_t nmemb,  size_t size,
                           int(*_compar)(const void *, const void *));
*/
                           
extern void     init_file   (void);                           

extern int      kfopen      (const char* pathname, int flags);
extern int      kfclose     (int fd);
extern int      kfsync      (int fd);
extern ssize_t  kfread      (int fd, void *buf, size_t count);
extern off_t    klseek      (int fildes, off_t offset, int whence);
extern off_t    kftell      (int fd);
extern int      kfcreat      (const char *pathname, mode_t mode);
extern ssize_t  kfwrite      (int fd, const void *buf, size_t count);
extern int      kfremove     (const char* pathname);
extern int      kfrename     (const char* path, const char* newname);
extern int      kftruncate  (int fd, off_t length);
extern int      kfilesize   (int fd);

#endif
