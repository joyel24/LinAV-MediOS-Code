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

#include <kernel/kernel.h>

#include <kernel/stdfs.h>
#include <kernel/vfs.h>
#include <kernel/fat.h>

#include <sys_def/string.h>

#warning these functions update VFS => any running readdir in the parent will be corrupted
#warning might be a problem when doing recursive rm in a folder

MED_RET_T mkdir(char *name, int mode)
{
#warning MODE not used in mkdir
    DIR *dir;
    struct vfs_pathname path;
    struct vfs_pathname dirName;

    char namecopy[MAX_PATH];

    struct dirent *entry;
    MED_RET_T ret_val;

    if ( name[0] != '/' ) {
        printk("mkdir: Only absolute paths supported right now\n");
        return -1;
    }

    path.length = strlen(name);
    if(path.length>0)
        path.str=name;
    else
    {
        printk("Error empty path\n");
        return -MED_EINVAL;
    }

    /* check for an ending '/' */

   if(path.str[path.length-1] == '/')
    {
        printk("We have an ending / discarding it\n");
        path.str[path.length-1]='\0';
        path.length--;
    }

    /* Split the base name and the path */
    vfs_pathnameSplitPath(&path,&path,&dirName);

    /* create a copy of dirName */
    vfs_pathNameDup(&dirName,&dirName,namecopy);

    /*put an end to path*/
    path.str[path.length]='\0';

    if(dirName.length == 0)
    {
        printk("[mkdir] Empty dir name\n");
        return -MED_EINVAL;
    }

    dir = opendir(path.str);

    if(!dir) {
        printk("mkdir: can't open parent dir\n");
        return -MED_ERROR;
    }

    /* Now check if the name already exists */
    while ((entry = readdir(dir))) {
        if ( !strcasecmp(dirName.str, entry->d_name) ) {
            printk("[mkdir] error: folder already exists\n");
            closedir(dir);
            return -MED_EEXIST;
        }
    }

    VFS_PRINT("About to call fat_creatDir\n");

    ret_val = fat_createDir(&dirName, dir);

    closedir(dir);

    return ret_val;
}

MED_RET_T rmdir(char* name)
{
    MED_RET_T ret_val;
    DIR* dir;
    struct dirent* entry;
    
    dir = opendir(name);
    if (!dir)
    {
     /* open error */
        return -1;
    }

    /* check if the directory is empty */
    while ((entry = readdir(dir)))
    {
        if (strcmp(entry->d_name, ".") &&
            strcmp(entry->d_name, ".."))
        {
            printk("[rmdir] error, folder not empty\n");
            closedir(dir);
            return -MED_ENOTEMPTY;
        }
    }

    ret_val = fat_fileRemove(dir);
    if ( ret_val != MED_OK )
    {
        printk("[rmdir] Failed removing dir |%s| (err=%d)\n", name,-ret_val);
    }

    closedir(dir);
    
    ret_val = vfs_rmNodeFromTree(dir);
    if ( ret_val != MED_OK )
    {
        printk("[rmdir] Failed removing node (%s) (err=%d)\n", name,-ret_val);
    }
    
    return ret_val;
}


MED_RET_T rmfile(char* name)
{
    MED_RET_T ret_val;
    FILE * file;
    int fd = open(name, O_WRONLY);
    
    if ( fd < 0 )
    {
        printk("[rmfile] file (%s) can't be opened (err=%d)\n", name,-fd);
        return fd;
    }

    file = (FILE*)fd;
    
    ret_val = fat_fileRemove(file);
    if ( ret_val != MED_OK )
    {
        printk("[rmfile] Failed removing dir |%s| (err=%d)\n", name,-ret_val);
        return ret_val;
    }
   
    fat_setFileSize(file,0);
    
    ret_val = close(fd);
    if ( ret_val != MED_OK )
    {
        printk("[rmfile] error closing |%s| (err=%d)\n", name,-ret_val);
        return ret_val;
    }
    
    ret_val = vfs_rmNodeFromTree(file);
    if ( ret_val != MED_OK )
    {
        printk("[rmfile] Failed removing node (%s) (err=%d)\n", name,-ret_val);
        return ret_val;
    }

    return MED_OK;
}

MED_RET_T mvEntry(char* path,char* newpath,struct vfs_node * openedfile, vfs_node_type type)
{
    MED_RET_T ret_val;
    struct vfs_pathname VFS_newPath;
    struct vfs_pathname VFS_newName;
    char newpath1[MAX_PATH];
    char newpath2[MAX_PATH];
    DIR * parent_dir;
    
    VFS_newPath.str = newpath;
    VFS_newPath.length = strlen(newpath);
    
    /* making sure not to change original string */
    vfs_pathNameDup(&VFS_newPath,&VFS_newPath,newpath1);
    
    /* Split the base name and the path */
    vfs_pathnameSplitPath(&VFS_newPath,&VFS_newPath,&VFS_newName);

    /* create a copy of dirName */
    vfs_pathNameDup(&VFS_newName,&VFS_newName,newpath2);

    /*put an end to path*/
    VFS_newPath.str[VFS_newPath.length]='\0';
        
    parent_dir = opendir(VFS_newPath.str);
    if(!parent_dir)
    {
        printk("[mv%s] Error opening parent folder |%s|\n",type==VFS_TYPE_FILE?"file":"dir",
            VFS_newPath.str);
        return -MED_ERROR;
    }
    
    
    ret_val = fat_mvFileDir(openedfile, parent_dir, &VFS_newName);
    if ( ret_val != MED_OK )
    {
        printk("[mv%s] Failed renaming file: %d\n",type==VFS_TYPE_FILE?"file":"dir", -ret_val);
        return ret_val;
    }

    ret_val = closedir(parent_dir);
    if (ret_val != MED_OK)
    {
        printk("[mv%s] Failed closing parent folder: %d\n",type==VFS_TYPE_FILE?"file":"dir", -ret_val);
        return ret_val;
    }

    return ret_val;
}

MED_RET_T mvfile(char* path, char* newpath)
{
    int fd;
    MED_RET_T ret_val,ret_val2;
    
    /* verify new path does not already exist or isn't a folder */
    fd = open(newpath, O_RDONLY);
    if ( fd >= 0 || fd == -MED_EISDIR)
    {
        close(fd);
        printk("[mvFile] error in params : newpath %s %s\n",newpath,fd == -MED_EISDIR?"is a dir":"already exists");
        return -MED_EINVAL;
    }
    
    fd = open(path, O_RDONLY);
    if ( fd < 0 )
    {
        printk("[mvFile] error in params : curpath %s does not exists or is not a file\n",path);
        return -fd;
    }
    
    ret_val = mvEntry(path, newpath, (FILE*)fd,VFS_TYPE_FILE);
        
    ret_val2 = close(fd);
    if ( ret_val2 != MED_OK )
    {
        printk("[mvfile] error closing file |%s| (err=%d)\n", path,-ret_val2);
        return ret_val2;
    }
    
    if(ret_val == MED_OK)
    {
        ((FILE*)fd)->ref_cnt = 0 ;
        ret_val2 = vfs_rmNodeFromTree((FILE*)fd);
        if ( ret_val2 != MED_OK )
        {
            printk("[mvfile] error removing node |%s| (err=%d)\n", path,-ret_val2);
            return ret_val2;
        }
    }
    
    return ret_val;    
}

MED_RET_T mvdir(char* path,char* newpath)
{
    DIR * dir;
    int fd;
    MED_RET_T ret_val,ret_val2;
    
    /* verify new path does not already exist */
    dir = opendir(newpath);
    if ( dir != NULL )
    {
        closedir(dir);
        printk("[mvDir] error in params : newpath %s already exists\n",newpath);
        return -MED_EINVAL;
    }
    
    fd = open(newpath, O_RDONLY);
    if(fd >= 0)
    {
        close(fd);
        printk("[mvDir] error in params : newpath %s is a file\n",newpath);
        return -MED_EINVAL;
    }
    
    dir = opendir(path);
    if ( dir == NULL )
    {
        printk("[mvDir] error in params : curpath %s does not exists or is a file\n",path);
        return -MED_EINVAL;
    }
    
    ret_val = mvEntry(path, newpath, dir,VFS_TYPE_DIR);
    
    ret_val2 = closedir(dir);
    if ( ret_val2 != MED_OK )
    {
        printk("[mvDir] error closing dir |%s| (err=%d)\n", path,-ret_val2);
        return ret_val2;
    }
    
    if(ret_val == MED_OK)
    {
        dir->ref_cnt = 0 ;
        ret_val2 = vfs_rmNodeFromTree(dir);
        if ( ret_val2 != MED_OK )
        {
            printk("[mvdir] error removing node |%s| (err=%d)\n", path,-ret_val2);
            return ret_val2;
        }
    }
    
    return ret_val; 
    
}
