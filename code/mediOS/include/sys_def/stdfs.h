/*
*   include/sys_def/stdfs.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __SYS_DEF_STDFS_H
#define __SYS_DEF_STDFS_H

#define MAX_PATH 260

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2
#define O_CREAT 4
#define O_APPEND 8
#define O_TRUNC  0x10

#define ATTR_READ_ONLY   0x01
#define ATTR_HIDDEN      0x02
#define ATTR_SYSTEM      0x04
#define ATTR_VOLUME_ID   0x08
#define ATTR_DIRECTORY   0x10
#define ATTR_ARCHIVE     0x20
#define ATTR_VOLUME      0x40 /* this is a volume, not a real directory */

typedef enum {
    VFS_TYPE_FILE  = 0x1,
    VFS_TYPE_DIR   = 0x2,
} vfs_node_type;

struct dirent {
    unsigned char  * d_name;
    int type;
    int attribute;
    int size;
//    int storage_location;
};

#include <kernel/vfs_node.h>

#endif
