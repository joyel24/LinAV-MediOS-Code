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
#include <kernel/stdfs.h>
#include <kernel/vfs_pathname.h>
#include <kernel/vfs_node.h>
#include <kernel/list.h>

#include <kernel/fat.h>
#include <kernel/disk.h>

struct vfs_mountPoint * root_mountPoint;

struct vfs_mountPoint * mountPoint_list;

void vfs_init(void)
{
    root_mountPoint=NULL;
    mountPoint_list=NULL;
    
    /* init drivers */
    fat_init();
    printk("[VFS init] done\n");
}

MED_RET_T vfs_mount(char * mount_path,int drive, int partition_num)
{
    int ret_val;
    struct vfs_node * root_node;
    struct vfs_mountPoint * mountPoint;
    
    struct vfs_pathname VFS_Path;
    struct vfs_pathname VFS_Name;
    char path_str[MAX_PATH];
    char name_str[MAX_PATH];
  
    char * mount_str;
    int is_root=0;
    struct vfs_node * res=NULL;
    
    strcpy(path_str,mount_path);
    VFS_Path.str=path_str;
    VFS_Path.length=strlen(mount_path);
        
    if(mount_path[0] == '/' && mount_path[1] == '\0')
    {
        is_root=1;        
        if(root_mountPoint!=NULL)
        {
            printk("[vfs_mount] error: trying to mount root-dir but it already exists\n");
            return -MED_EEXIST;
        }
    }
    else
    {
        if(root_mountPoint==NULL)
        {
            printk("[vfs_mount] error: can't mount %s, no root-dir\n",mount_path);
            return -MED_EEXIST;
        }
        
        /* check if mountpoint already exists */
        ret_val=vfs_nodeExist(&VFS_Path,&res);
        if(ret_val!=MED_OK)
            return ret_val;
        if(res)
        {
            printk("[vfs_mount] error: %s already exists\n",mount_path);
            return -MED_EINVAL;
        }            
        
        /* need to find where to mount the drive */    
  
        /* Split the base name and the path */
        vfs_pathnameSplitPath(&VFS_Path,&VFS_Path,&VFS_Name);
        /* create a copy of name */
        vfs_pathNameDup(&VFS_Name,&VFS_Name,name_str);
        /*put an end to path*/
        VFS_Path.str[VFS_Path.length]='\0';
        
        ret_val=vfs_nodeExist(&VFS_Path,&res);
        if(ret_val!=MED_OK)
            return ret_val;
        
        if(!res)
        {
            printk("[vfs_mount] Error geting parent folder |%s|\n",VFS_Path.str);
            return -MED_ERROR;
        }
    }
    
    ret_val = fat_mount(drive,
        drive_info[drive]->partition_list[partition_num].start,
        &root_node);
    
    if(ret_val!=MED_OK)
    {
        printk("[vfs_mount] fat error mounting %s: %d\n",mount_path,-ret_val);
        return ret_val;
    }
    
    mountPoint = (struct vfs_mountPoint *)kmalloc(sizeof(struct vfs_mountPoint));
    if(!mountPoint)
    {
        #warning we should also clean root_node !!!
        printk("[vfs_mount] malloc error creating mountPoint\n");
        return -MED_ENOMEM;
    }
    
    mountPoint->root_node=root_node;
    mountPoint->root_node->mount_point=mountPoint;
    mountPoint->drive=drive;
    mountPoint->partition_num=partition_num;
    mountPoint->dirty_list=NULL;
    mountPoint->opened_node=NULL;
    LIST_ADD_TAIL(mountPoint_list,mountPoint);
    
    if(is_root)
    {
        
        root_node->name.length=strlen(mount_path);
        root_node->name.str=kmalloc(root_node->name.length+1);
        strcpy(root_node->name.str,mount_path);
        root_mountPoint=mountPoint;
    }
    else
    {
        mount_str=kmalloc(VFS_Name.length+1);
        vfs_pathNameDup(&VFS_Name,&root_node->name,mount_str);        
        vfs_nodeAddChild(res,root_node);
        #warning we should also add . and .. folder in non root-dir mount point
    }
    
    ret_val=fat_loadDir(root_node);
    if(ret_val!=MED_OK)
    {
        printk("[vfs_mount] error loading %s content: %d\n",mount_path,-ret_val);
        return ret_val;
    }
    
    printk("[vfs_mount] %s mounted\n",root_node->name.str);
    
    return MED_OK;
}

struct vfs_mountPoint * vfs_isMounted(int drive,int partition_num)
{
    struct vfs_mountPoint * mPoint;
    int nb_mPoint;
    LIST_FOREACH(mountPoint_list,mPoint,nb_mPoint)
    {
        if(mPoint->drive==drive && mPoint->partition_num==partition_num)
            return mPoint;
    }
    return NULL;
}

MED_RET_T vfs_umount(int drive,int partition_num)
{
    struct vfs_mountPoint * mPoint=vfs_isMounted(drive,partition_num);
    int ret_val;
    
    if(mPoint)
    {
        ret_val=vfs_Destructor(mPoint);
        if(ret_val!=MED_OK)
            return ret_val;
        LIST_DELETE(mountPoint_list,mPoint);
        free(mPoint);
        printk("[vfs_umount] %s-$%d done\n",disk_getName(drive),partition_num);
    }
    else
    {
        printk("[vfs_umount] %s-$%d not mounted\n",disk_getName(drive),partition_num);
    }
    return MED_OK;
}

MED_RET_T vfs_DestroyAll(void)
{
    return vfs_Destructor(root_mountPoint);
}

MED_RET_T vfs_Destructor(struct vfs_mountPoint * mount_root)
{
    MED_RET_T ret_val;
    struct vfs_node * root_node=mount_root->root_node;
    
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

    
    vfs_rmNodeFromTree(root_node);

    mount_root->root_node=NULL;
    mount_root->opened_node=NULL;
    mount_root->dirty_list=NULL;
        
    return MED_OK;
}

MED_RET_T vfs_clearNodeTree(struct vfs_node * root)
{
    struct vfs_node * ptr;
    int ret_val;
    
    while(!LIST_IS_EMPTY(root->children))
    {
        ptr = root->children;
        if(ptr->opened && ptr->dirty)
        {
            printk("[vfs_clearNodeTree] %s opened => NEED a SYNC fction\n",ptr->name.str);
#warning need to sync file                
        }
        
        if(ptr->type == VFS_TYPE_DIR)
        {
            ret_val=vfs_clearNodeTree(ptr);
            if(ret_val!=MED_OK)
                return ret_val;
        }
        
        vfs_rmNodeFromTree(ptr);
    }

    return MED_OK;

}

void vfs_PrintOpenList(void)
{
    struct vfs_node * node;
    struct vfs_mountPoint * mPoint;
    int nb_node;
    int nb_mPoint;
    
    LIST_FOREACH(mountPoint_list,mPoint,nb_mPoint)
    {
        LIST_FOREACH_NAMED(mPoint->opened_node,node,nb_node,prev_open,next_open)
        {
            printk("%d-%d:%s\n",nb_mPoint,nb_node,node->name.str);
        }
    }
}

int vfs_hasOpenNode(void)
{
    struct vfs_mountPoint * mPoint;
    int nb_mPoint;
    LIST_FOREACH(mountPoint_list,mPoint,nb_mPoint)
    {
        if(!LIST_IS_EMPTY_NAMED(mPoint->opened_node,prev_open,next_open))
            return 1;
    }
    return 0;
}

void vfs_rootPrint(void)
{
    vfs_nodePrintTree(root_mountPoint->root_node,0);
}

