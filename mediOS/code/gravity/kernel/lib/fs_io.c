/* 
*   kernel/lib/fs_io.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <fs_io.h>
#include <api.h>

int fopen(const char* pathname, int flags)
{
    int res=flags;
    API_FILE(0x000,(void*) pathname,(void*) &res);
    return res;
}

int fclose(int fd)
{
    int res;
    API_FILE(0x001,(void*) fd,(void*) &res);
    return res;
}

int fsync(int fd)
{
    int res;
    API_FILE(0x002,(void*) fd,(void*) &res);
    return res;
}

ssize_t fread(int fd, void *buf, size_t count)
{
    ssize_t res=count;
    COUPLE_DATA data;
    data.a=(void*)fd;
    data.b=buf;
    API_FILE(0x003,(void*) &data,(void*) &res);
    return res;
}

off_t fseek(int fildes, off_t offset, int whence)
{
    off_t res=offset;
    COUPLE_DATA data;
    data.a=(void*)fildes;
    data.b=(void*)whence;
    API_FILE(0x004,(void*) &data,(void*) &res);
    return res;
}

off_t ftell(int fd)
{
    off_t res;
    API_FILE(0x005,(void*) fd,(void*) &res);
    return res;
}

int fcreat(const char *pathname, mode_t mode)
{
    int res;
    COUPLE_DATA data;
    data.a=(void*)pathname;
    data.b=(void*)mode;
    API_FILE(0x006,(void*) &data,(void*) &res);
    return res;
}
ssize_t fwrite(int fd, const void *buf, size_t count)
{
    ssize_t res=count;
    COUPLE_DATA data;
    data.a=(void*)fd;
    data.b=(void*)buf;
    API_FILE(0x007,(void*) &data,(void*) &res);
    return res;
}

int fremove(const char* pathname)
{
    int res;
    API_FILE(0x008,(void*) pathname,(void*) &res);
    return res;
}
    
int frename(const char* path, const char* newname)
{
    int res;
    COUPLE_DATA data;
    data.a=(void*)path;
    data.b=(void*)newname;
    API_FILE(0x009,(void*) &data,(void*) &res);
    return res;
}

int ftruncate(int fd, off_t length)
{
    int res;
    COUPLE_DATA data;
    data.a=(void*)fd;
    data.b=(void*)length;
    API_FILE(0x00A,(void*) &data,(void*) &res);
    return res;    
}

int filesize(int fd)
{
    int res;
    API_FILE(0x00B,(void*) fd,(void*) &res);
    return res;
}

DIR* opendir(const char* name)
{
    DIR * res;
    API_FILE(0x100,(void*) name,(void*) &res);
    return res;
}

int closedir(DIR* dir)
{
    int res;
    API_FILE(0x101,(void*) dir,(void*) &res);
    return res;
}

int mkdir(const char* name, int mode)
{
    int res=mode;
    API_FILE(0x102,(void*) name,(void*) &res);
    return res;
}

int rmdir(const char* name)
{
    int res;
    API_FILE(0x103,(void*) name,(void*) &res);
    return res;
}

struct dirent* readdir(DIR* dir)
{
    struct dirent* res;
    API_FILE(0x104,(void*) dir,(void*) &res);
    return res;    
}
