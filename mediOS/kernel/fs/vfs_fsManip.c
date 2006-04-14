/*
*   kernel/fs/vfs_fsManip.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*
*/

#include <kernel/stdfs.h>

/*
int remove(const char* name)
{
    int rc;
    struct filedesc* file;
    int fd = open(name, O_WRONLY);
    if ( fd < 0 )
        return fd * 10 - 1;

    file = &openfiles[fd];
    rc = fat_remove(&(file->fatfile));
    if ( rc < 0 ) {
        printk("Failed removing file: %d\n", rc);
        return rc * 10 - 3;
    }

    file->size = 0;

    rc = close(fd);
    if (rc<0)
        return rc * 10 - 4;

    return 0;
}*/
