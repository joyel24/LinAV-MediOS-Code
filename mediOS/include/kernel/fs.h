/* 
*   include/kernel/fs.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __FS_H
#define __FS_H

#include <kernel/errors.h>

/* string max sizes */
#define FILES_SYSTEM_NAME_MAXLEN 32

/* forward definition */
struct file_system;
struct fs_instance;
struct fs_stat_fs;

struct file_system {
    char name[FILES_SYSTEM_NAME_MAXLEN];
    
    int (*mount)(struct file_system * this,
                 int device,
                 char * mnt_point,
                 struct fs_instance ** mounted_fs,
                 void * mount_data                 
                 );
                 
    int (*umount)(struct file_system * this,
                  struct fs_instance * mounted_fs
                 );
                 
    /* custom data, free for fs */
    void * fs_data;
    
    /* mounted instances of this fs type */
    struct fs_instance * instances;
    
    /* used for the list of fs type */
    struct file_system * next;
    struct file_system * prev;
};

struct fs_instance {
    int (*statfs)(struct fs_instance * this,
                  struct fs_stat_fs * result);
                  
    /* custom data, free for fs instance*/
    void * fs_data;
    
    /* used for the list of fs type */
    struct fs_instance * next;
    struct fs_instance * prev;
};

struct fs_stat_fs {

};

/* file system related related functions */
void      vfs_init(void);
MED_RET_T vfs_registerFileSystem(struct file_system * fs_type);
MED_RET_T vfs_unRegisterFileSystem(struct file_system * fs_type);

#endif
