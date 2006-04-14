/*
*   include/kernel/vfs_node.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __VFS_NODE_H
#define __VFS_NODE_H

#include <kernel/errors.h>
#include <kernel/vfs_pathname.h>

typedef struct vfs_node FILE;
typedef struct vfs_node DIR;

#include <sys_def/stdfs.h>


#define FILE_WRITE(FLAG)  ((FLAG) & (O_RDWR | O_WRONLY))
#define FILE_TRUNC(FLAG)  ((FLAG) & (O_TRUNC))
#define FILE_CREATE(FLAG) ((FLAG) & (O_CREAT))
#define FILE_APPEND(FLAG) ((FLAG) & (O_APPEND))

#define CHK_FD(FD) { \
    if(!FD->opened)   \
        return -MED_EINVAL;      \
}

/*forward definition*/

struct vfs_node {
    /* fs instance of the node */

    /* node location on the disk */
    unsigned int storage_location;
    /* name of the file in the vfs structure */
    struct vfs_pathname name;

    /* type of the node: file / dir */
    vfs_node_type type;

    /* number of ref on the node */
    int ref_cnt;
    int dir_loaded;

    /* file/dir opened*/
    int dirty;
    int opened;

    /* file specific */
    unsigned int position;
    int flags;

    /* dir specific */
    struct vfs_node * curNode;
    struct dirent theent;

    /*can be used by the fs */
    void * custom_data;

    /* node linkage in VFS tree */
    struct vfs_node * parent,*children,*siblings_prev,*siblings_next;

    /* dirty node of the fs linkage */
    struct vfs_node *prev_dirty,*next_dirty;
    /* open node of the fs linkage */
    struct vfs_node *prev_open,*next_open;
};

void vfs_nodePrintTree(struct vfs_node *node,int level);
MED_RET_T vfs_nodeInitChild(struct vfs_node * parent,
                struct  vfs_pathname * name,
                struct vfs_node * node);
MED_RET_T vfs_nodeRef(struct vfs_node * node);
MED_RET_T vfs_nodeLookup(struct  vfs_pathname * path,
                    struct vfs_node * start_node,
                    struct vfs_node ** result_node,
                    struct  vfs_pathname * result_remaining_path);

MED_RET_T vfs_nodeUnRef(struct vfs_node * node);
MED_RET_T vfs_nodeRef(struct vfs_node * node);
MED_RET_T vfs_rmNodeFromTree(struct vfs_node * node);
MED_RET_T vfs_nodeSetDirty(struct vfs_node * node);
MED_RET_T vfs_nodeClearDirty(struct vfs_node * node);


#endif
