/* 
*   kernel/fs/disk.c
*
*   AMOS project
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

#include <kernel/fat.h>
#include <kernel/dir.h>
#include <kernel/disk.h>
#include <kernel/kernel.h>


#define MAX_OPEN_DIRS 8

static DIR opendirs[MAX_OPEN_DIRS];

static const char* vol_names = "<HD%d>";
#define VOL_ENUM_POS 3

void init_dir(void)
{
    int dd;
    for ( dd=0; dd<MAX_OPEN_DIRS; dd++)
        opendirs[dd].busy=false;
}

/* returns on which volume this is, and copies the reduced name
   (sortof a preprocessor for volume-decorated pathnames) */
static int strip_volume(const char* name, char* namecopy)
{
    int volume = 0;

    if (name[1] == vol_names[0] ) /* a '<' quickly identifies our volumes */
    {
        const char* temp;
        temp = name + 1 + VOL_ENUM_POS; /* behind '/' and special name */
        volume = atoi(temp); /* number is following */
        temp = strchr(temp, '/'); /* search for slash behind */
        if (temp != NULL)
            name = temp; /* use the part behind the volume */
        else
            name = "/"; /* else this must be the root dir */
    }

    strncpy(namecopy, name, MAX_PATH);
    namecopy[MAX_PATH-1] = '\0';

    return volume;
}


DIR* opendir(const char* name)
{
    char namecopy[MAX_PATH];
    char* part;
    char* end;
    struct fat_direntry entry;
    int dd;
    DIR* pdir = opendirs;
    int volume;

    if ( name[0] != '/' ) {
        printk("Only absolute paths supported right now\n");
        return NULL;
    }

    /* find a free dir descriptor */
    for ( dd=0; dd<MAX_OPEN_DIRS; dd++, pdir++)
        if ( !pdir->busy )
            break;

    if ( dd == MAX_OPEN_DIRS ) {
        printk("Too many dirs open\n");
        return NULL;
    }

    pdir->busy = true;

    /* try to extract a heading volume name, if present */
    volume = strip_volume(name, namecopy);
    pdir->volumecounter = 0;

    if ( fat_opendir(volume, &pdir->fatdir, 0, NULL) < 0 ) {
        printk("Failed opening root dir\n");
        pdir->busy = false;
        return NULL;
    }

    for ( part = strtok_r(namecopy, "/", &end); part;
          part = strtok_r(NULL, "/", &end)) {
        /* scan dir for name */
        while (1) {
            if ((fat_getnext(&pdir->fatdir,&entry) < 0) ||
                (!entry.name[0])) {
                pdir->busy = false;
                return NULL;
            }
            if ( (entry.attr & FAT_ATTR_DIRECTORY) &&
                 (!strcasecmp(part, entry.name)) ) {
                pdir->parent_dir = pdir->fatdir;
                if ( fat_opendir(volume,
                                 &pdir->fatdir,
                                 entry.firstcluster,
                                 &pdir->parent_dir) < 0 ) {
                    printk("Failed opening dir '%s' (%d)\n",
                           part, entry.firstcluster);
                    pdir->busy = false;
                    return NULL;
                }
                pdir->volumecounter = -1; /* n.a. to subdirs */

                break;
            }
        }
    }

    return pdir;
}

int closedir(DIR* dir)
{
    dir->busy=false;
    return 0;
}

struct dirent* readdir(DIR* dir)
{
    struct fat_direntry entry;
    struct dirent* theent = &(dir->theent);
    /* Volumes (secondary file systems) get inserted into the root directory
        of the first volume, since we have no separate top level. */
    if (dir->volumecounter >= 0 /* on a root dir */
     && dir->volumecounter < NUM_VOLUMES /* in range */
     && dir->fatdir.file.volume == 0) /* at volume 0 */
    {   /* fake special directories, which don't really exist, but
           will get redirected upon opendir() */
        while (++dir->volumecounter < NUM_VOLUMES)
        {
            if (fat_ismounted(dir->volumecounter))
            {
                memset(theent, 0, sizeof(*theent));
                theent->attribute = FAT_ATTR_DIRECTORY | FAT_ATTR_VOLUME;
                snprintf(theent->d_name, sizeof(theent->d_name), 
                         vol_names, dir->volumecounter);
                return theent;
            }
        }
    }

    /* normal directory entry fetching follows here */
    if (fat_getnext(&(dir->fatdir),&entry) < 0)
        return NULL;

    if ( !entry.name[0] )
        return NULL;	

    strncpy(theent->d_name, entry.name, sizeof( theent->d_name ) );
    theent->attribute = entry.attr;
    theent->size = entry.filesize;
    theent->startcluster = entry.firstcluster;
    theent->wrtdate = entry.wrtdate;
    theent->wrttime = entry.wrttime;
   
    return theent;
}

int mkdir(const char *name, int mode)
{
    DIR *dir;
    char namecopy[MAX_PATH];
    char* end;
    char *basename;
    char *parent;
    struct dirent *entry;
    struct fat_dir newdir;
    int rc;

    (void)mode;

    if ( name[0] != '/' ) {
        printk("mkdir: Only absolute paths supported right now\n");
        return -1;
    }

    strncpy(namecopy,name,sizeof(namecopy));
    namecopy[sizeof(namecopy)-1] = 0;

    /* Split the base name and the path */
    end = strrchr(namecopy, '/');
    *end = 0;
    basename = end+1;

    if(namecopy == end) /* Root dir? */
        parent = "/";
    else
        parent = namecopy;
        
    printk("mkdir: parent: %s, name: %s\n", parent, basename);

    dir = opendir(parent);
    
    if(!dir) {
        printk("mkdir: can't open parent dir\n");
        return -2;
    }    

    if(basename[0] == 0) {
        printk("mkdir: Empty dir name\n");
        return -3;
    }
    
    /* Now check if the name already exists */
    while ((entry = readdir(dir))) {
        if ( !strcasecmp(basename, entry->d_name) ) {
            printk("mkdir error: file exists\n");
            closedir(dir);
            return - 4;
        }
    }

    memset(&newdir, sizeof(struct fat_dir), 0);
    
    rc = fat_create_dir(basename, &newdir, &(dir->fatdir));
    
    closedir(dir);
    
    return rc;
}

int rmdir(const char* name)
{
    int rc;
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
            printk("rmdir error: not empty\n");
            closedir(dir);
            return -2;
        }
    }

    rc = fat_remove(&(dir->fatdir.file));
    if ( rc < 0 ) {
        printk("Failed removing dir: %d\n", rc);
        rc = rc * 10 - 3;
    }

    closedir(dir);
    
    return rc;
}
