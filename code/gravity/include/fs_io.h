/* 
*   include/api.h
*
*   AMOS project
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __FS_IO_H
#define __FS_IO_H

#include <sys_def/file.h>
#include <sys_def/dir.h>

int      fopen(const char* pathname, int flags);
int      fclose     (int fd);
int      fsync      (int fd);
ssize_t  fread      (int fd, void *buf, size_t count);
off_t    fseek      (int fildes, off_t offset, int whence);
off_t    ftell      (int fd);
int      fcreat     (const char *pathname, mode_t mode);
ssize_t  fwrite     (int fd, const void *buf, size_t count);
int      fremove    (const char* pathname);
int      frename    (const char* path, const char* newname);
int      ftruncate  (int fd, off_t length);
int      filesize   (int fd);

DIR*     opendir(const char* name);
int      closedir(DIR* dir);
int      mkdir(const char* name, int mode);
int      rmdir(const char* name);
struct dirent* readdir(DIR* dir);

#endif
