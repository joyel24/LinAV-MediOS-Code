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
#include <kernel/malloc.h>

#include <kernel/vfs.h>
#include <kernel/vfs_pathname.h>
#include <kernel/vfs_node.h>
#include <kernel/list.h>

#include <kernel/fat.h>

struct vfs_node * root_node;

struct vfs_node * dirty_list;
struct vfs_node * opened_node;

void vfs_init(void)
{
    root_node=NULL;
    opened_node=NULL;
    dirty_list=NULL;    
}

MED_RET_T vfs_mount(int device,unsigned int startsector)
{
    int ret_val;

    ret_val = fat_mount(device,startsector,&root_node);
    if(ret_val!=MED_OK)
        printk("error mounting root: %d\n",-ret_val);

    ret_val=fat_loadDir(root_node);
    if(ret_val!=MED_OK)
        printk("error loading root: %d\n",-ret_val);

    printk("root %s\n",root_node->name.str);

    opened_node=NULL;
    dirty_list=NULL;

    return MED_OK;
}

MED_RET_T vfs_Destructor(void)
{
    MED_RET_T ret_val;
    if(!root_node)
        return MED_OK;

    if(root_node->children)
    {
        ret_val=vfs_clearNodeTree(root_node);
        if(ret_val != MED_OK)
            return ret_val;
    }

    ret_val=fat_unmount(root_node,1);
    if(ret_val != MED_OK)
        return ret_val;

    free(root_node);

    root_node=NULL;
    opened_node=NULL;
    dirty_list=NULL;

    return MED_OK;
}

MED_RET_T vfs_clearNodeTree(struct vfs_node * root)
{
    struct vfs_node * ptr,*ptr2;
    int ret_val;
    if(root->children)
    {
        ptr=NULL;
        while(ptr!=root->children)
        {
            if(ptr==NULL)
                ptr=root->children;
            if(ptr->type == VFS_TYPE_DIR)
            {
                ret_val=vfs_clearNodeTree(ptr);
                if(ret_val!=MED_OK)
                    return ret_val;
            }
            ptr2=ptr->siblings_next;
            #warning need to check dirty flag / open flag too
            free(ptr);
            ptr=ptr2;
        }
    }
    return MED_OK;

}

void vfs_PrintOpenList(void)
{
    struct vfs_node * node;
    int nb_node;
    LIST_FOREACH_NAMED(opened_node,node,nb_node,prev_open,next_open)
    {
        printk("%d:%s\n",nb_node,node->name.str);
    }
}

int vfs_hasOpenNode(void)
{
    return !LIST_IS_EMPTY_NAMED(opened_node,prev_open,next_open);
}

void vfs_rootPrint(void)
{
    vfs_nodePrintTree(root_node,0);
}

