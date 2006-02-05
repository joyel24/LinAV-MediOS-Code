/* 
*   include/kernel/file.h
*
*   MediOS project
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

#ifndef __FILE_H_
#define __FILE_H_

#include <sys_def/types.h>
#include <sys_def/file.h>

                         
extern void     init_file   (void);                           

extern int      fopen      (const char* pathname, int flags);
extern int      fclose     (int fd);
extern int      fsync      (int fd);
extern ssize_t  fread      (int fd, void *buf, size_t count);
extern off_t    lseek      (int fildes, off_t offset, int whence);
extern off_t    ftell      (int fd);
extern int      fcreat      (const char *pathname, mode_t mode);
extern ssize_t  fwrite      (int fd, const void *buf, size_t count);
extern int      fremove     (const char* pathname);
extern int      frename     (const char* path, const char* newname);
extern int      ftruncate  (int fd, off_t length);
extern int      filesize   (int fd);

#endif
