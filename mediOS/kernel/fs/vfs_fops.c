/*
*   kernel/fs/vfs_fops.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
* Part of this code is from Rockbox project
* Copyright (C) 2002 by Björn Stenberg
*
*/

#include <sys_def/string.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/stdfs.h>
#include <kernel/list.h>

#include <kernel/fat.h>
#include <kernel/vfs.h>
#include <kernel/vfs_node.h>

extern struct vfs_node * root_node;
extern struct vfs_node * opened_node;

int open(const char * name,int flags)
{
    MED_RET_T ret_val;
    struct vfs_pathname path;

    FILE * fd;

    if(!root_node)
        return -MED_ENOENT;
    
    path.length = strlen(name);
    if(path.length>0)
        path.str=name;
    else
        return -MED_EINVAL;

    if(path.str[0] != '/')
    {
        printk("'%s' not an absolute path\n",name);
        printk("only root path (strating with '/') are supported\n");
        return -MED_EINVAL;
    }

    ret_val=vfs_nodeLookup(&path,root_node,&fd,&path);

    if(ret_val!=MED_OK && ret_val!=-MED_ENOENT)
    {
        printk("Err(%d): during 'vfs_nodeLookup' call\n",-ret_val);
        return -MED_EINVAL;
    }

    if(path.length<=0)
    {
        /* ok found it !! */
        if(fd->opened)
        {
            /* already opened */
            printk("File %s alreday opened\n",name);
            return -MED_EBUSY;
        }

        if(fd->type == VFS_TYPE_DIR)
        {
            /* it is a dir => can't open dir with fopen !! */
            printk("Trying to open a folder with fopen !!\n");
            printk("Folder name: '%s'\n",name);
            return -MED_EINVAL;
        }
    }
    else
    {
        /* not found, should we create it? */
        if(FILE_CREATE(flags) && FILE_WRITE(flags))
        {
            struct  vfs_pathname path2;
            struct  vfs_node * new_node;
            /* are we in the needed folder ?*/
            VFS_PRINT("Need to create file\n");
            vfs_pathnameSplit(&path,&path,&path2);
            if(path2.length != 0)
            {
                printk("File not existing, and can't create beacuse folder also absent (%s)\n",name);
                return -MED_ENOMEM;
            }
            new_node = (struct  vfs_node *)malloc(sizeof(struct  vfs_node));
            if(!new_node)
            {
                printk("Error can't create new node, not enough mem\n");
                return -MED_ENOMEM;
            }

            memset(new_node,0x0,sizeof(struct  vfs_node));
            new_node->type=VFS_TYPE_FILE;
            ret_val = vfs_nodeInitChild(fd,&path,new_node);
            if(ret_val!=MED_OK)
            {
                printk("Error in initChild (err=%d)\n",-ret_val);
                free(new_node);
                return ret_val;
            }

            ret_val=fat_createFile(path.str,new_node,fd);
            if(ret_val!=MED_OK)
            {
                printk("Error in fat_create_file (err=%d)\n",-ret_val);
                ret_val=vfs_rmNodeFromTree(new_node);
                if(ret_val!=MED_OK)
                    printk("Error in rm node from tree (err=%d)\n",-ret_val);
                return ret_val;
            }
            fd=new_node;
        }
        else
        {
            printk("File '%s' not found\n",name);
            return -MED_ENOENT;
        }
    }

    ret_val=fat_fileOpen(fd);

    if(ret_val != MED_OK)
    {
        printk("Error in fat_open: %d\n",-ret_val);
        return ret_val;
    }

    /* ok file is opened */
    fd->opened=1;
    fd->position = 0;
    fd->flags = flags;

    /* setup struct */
    if(FILE_TRUNC(flags))
    {
        ret_val=fat_fileTruncate(fd,0);
        if(ret_val != MED_OK)
        {
            printk("error truncating file\n");
            fat_fileClose(fd);
            fd->opened=0;
            return ret_val;
        }
    }

    if (FILE_APPEND(flags) && FILE_WRITE(flags))
    {
        ret_val = lseek((int)fd,0,SEEK_END);
        if(ret_val!=MED_OK)
        {
            printk("error seeking at end for file (flags APPEND)\n");
            fat_fileClose(fd);
            fd->opened=0;
            return ret_val;
        }
    }

    vfs_nodeRef(fd);
    
    LIST_ADD_TAIL_NAMED(opened_node,fd,prev_open,next_open);

    return (int)fd;
}

MED_RET_T fsync(int fdesc)
{
    FILE * fd = (FILE *)fdesc;
    MED_RET_T ret_val=MED_OK;
    CHK_FD(fd)
    if(FILE_WRITE(fd->flags))
    {
        /* flush sector cache */
#warning need a global management of dirty flag
        if ( fd->dirty )
        {

            ret_val = fat_fileFlushCache(fd);
            if (ret_val != MED_OK)
                return ret_val;
        }

        /* truncate? */
        if (FILE_TRUNC(fd->flags))
        {
            VFS_PRINT("FCLOSE: We need to truncate file\n");
            ret_val=fat_fileTruncate(fd,fat_fileSize(fd));
            if (ret_val != MED_OK)
                return ret_val;
        }

        /* tie up all loose ends */
        ret_val = fat_fileSync(fd);
        if (ret_val != MED_OK)
               return ret_val;
    }
    return ret_val;
}

MED_RET_T close(int fdesc)
{
    FILE * fd = (FILE *)fdesc;
    MED_RET_T ret_val=MED_OK;

    CHK_FD(fd)

    fsync(fdesc);

    ret_val=fat_fileClose(fd);

    if(ret_val!=MED_OK)
        return ret_val;

    fd->opened = 0;

    vfs_nodeUnRef(fd);
    LIST_DELETE_NAMED(opened_node,fd,prev_open,next_open);

    return MED_OK;
}

MED_RET_T truncate(int fdesc, unsigned int size)
{
    FILE * fd = (FILE *)fdesc;
    CHK_FD(fd);
    if(FILE_WRITE(fd->flags))
        return fat_fileTruncate(fd,size);
    return MED_OK;
}

int write(int fdesc, const void* buf, unsigned int count)
{
    FILE * fd = (FILE *)fdesc;
    CHK_FD(fd);
    if (!FILE_WRITE(fd->flags))
        return -MED_EINVAL;
    return fat_fileWrite(fd, (void *)buf, count);
}

int read(int fdesc, const void* buf, unsigned int count)
{
    FILE * fd = (FILE *)fdesc;
    CHK_FD(fd);
    return fat_fileRead(fd, (void *)buf, count);
}

int lseek(int fdesc, unsigned int offset, int whence)
{
    FILE * fd = (FILE *)fdesc;
    int pos;

    MED_RET_T ret_val=MED_OK;
    CHK_FD(fd);
#warning when we ll add real EOF
#warning we should manage it in lseek => eof=0 when seek to beg, eof=1 when seek to end
    switch ( whence )
    {
        case SEEK_SET:
            pos = offset;
            break;

        case SEEK_CUR:
            pos = fd->position + offset;
            break;

        case SEEK_END:
            pos = fat_fileSize(fd) + offset;
            break;

        default:
        {
            printk("[lseek] Bad whence:%d\n",whence);
            return -MED_EINVAL;
        }
    }

    if (pos == fd->position) // optimize the case where we just want current file position
        goto done;
    
    if ((pos < 0) || (pos > fat_fileSize(fd))) {
        printk("[lseek] Bad pos:%d\n",pos);
        return -MED_EINVAL;
    }

    ret_val=fat_fileSeek(fd,pos);
    if(ret_val<0)
    {
        VFS_PRINT("[lseek] error from fat_fileSeek: %d\n",-ret_val);
        return ret_val;
    }

    fd->position=pos;

done:
    return pos;
}

int ftell(int fdesc)
{
    FILE * fd = (FILE *)fdesc;
    CHK_FD(fd);
    return fd->position;
}


int filesize(int fdesc)
{
    FILE * fd = (FILE *)fdesc;
    CHK_FD(fd);
    return fat_fileSize(fd);
}

