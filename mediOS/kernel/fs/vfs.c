/*
*   kernel/fs/vfs.c
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

#include <sys_def/string.h>

#include <kernel/kernel.h>
#include <kernel/errors.h>

#include <kernel/vfs.h>
#include <kernel/vfs_pathname.h>
#include <kernel/vfs_node.h>

#include <kernel/fat.h>

struct vfs_node * root_node;

MED_RET_T vfs_init(int device,unsigned int startsector)
{
    int ret_val;
    
    ret_val = fat_mount(device,startsector,&root_node);
    if(ret_val!=MED_OK)
        printk("error mounting root: %d\n",-ret_val);

    ret_val=fat_loadDir(root_node);
    if(ret_val!=MED_OK)
        printk("error loading root: %d\n",-ret_val);

    printk("root %s\n",root_node->name.str);

    return MED_OK;
}

void vfs_rootPrint(void)
{
    vfs_nodePrintTree(root_node,0);
}

