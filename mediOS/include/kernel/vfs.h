/*
*   include/kernel/vfs.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __VFS_H
#define __VFS_H

#include <kernel/disk.h>
#include <kernel/vfs_pathname.h>

//#define DEBUG_VFS

#ifdef DEBUG_VFS
#define VFS_PRINT(s...)   printk(s)
#else
#define VFS_PRINT(s...)
#endif

struct vfs_node;

struct vfs_mountPoint {
    int drive;
    int partition_num;
    struct vfs_node * root_node;
    struct vfs_node * dirty_list;
    struct vfs_node * opened_node;
    struct vfs_mountPoint * next,*prev;
};

MED_RET_T vfs_mount(char * mount_path,int drive, int partition_num);
MED_RET_T vfs_umount(int drive,int partition_num);
struct vfs_mountPoint * vfs_isMounted(int drive,int partition_num);

void vfs_init(void);

void vfs_rootPrint(void);

MED_RET_T vfs_DestroyAll(void);
MED_RET_T vfs_Destructor(struct vfs_mountPoint * mount_root);
MED_RET_T vfs_clearNodeTree(struct vfs_node * root);

void vfs_PrintOpenList(void);
int vfs_hasOpenNode(void);

#endif
