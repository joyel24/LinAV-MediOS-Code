/*
*   include/kernel/fat.h
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
* Copyright (C) 2002 by Linus Nielsen Feltzing
*
*/

#ifndef __FAT_H
#define __FAT_H

#include <kernel/errors.h>
#include <kernel/vfs_node.h>

#define BLOCK_SIZE 512
#define MAX_OPEN 10

struct fat_direntry
{
    unsigned char name[256];        /* Name plus \0 */
    unsigned short attr;            /* Attributes */
    unsigned char crttimetenth;     /* Millisecond creation
                                       time stamp (0-199) */
    unsigned short crttime;         /* Creation time */
    unsigned short crtdate;         /* Creation date */
    unsigned short lstaccdate;      /* Last access date */
    unsigned short wrttime;         /* Last write time */
    unsigned short wrtdate;         /* Last write date */
    unsigned int filesize;          /* File size in bytes */
    int firstcluster;               /* fstclusterhi<<16 + fstcluslo */
};

#define FAT_ATTR_READ_ONLY   0x01
#define FAT_ATTR_HIDDEN      0x02
#define FAT_ATTR_SYSTEM      0x04
#define FAT_ATTR_VOLUME_ID   0x08
#define FAT_ATTR_DIRECTORY   0x10
#define FAT_ATTR_ARCHIVE     0x20
#define FAT_ATTR_VOLUME      0x40 /* this is a volume, not a real directory */

struct fat_entry {
/* entry counter for folders */
    int entryN;
    int entryCount;
/* entry num in parent folder */
    int dirEntryNum;
/*nb entries needed in parent folder */
    int nbDirEntries;
/* folder / file data */
    struct bpb* fat_bpb;
    unsigned int lastcluster;
    unsigned int lastsector;
    unsigned int clusternum;
    unsigned int sectornum;
    unsigned int firstcluster;

    unsigned int size;
    int attr;

    struct fat_direntry dir_entry;
    int eof;
    struct file_cache * cache;
    int cache_num;
};

struct file_cache {
    char data[BLOCK_SIZE];
    int cacheOffset;
    int used;
};

void fat_init(void);

/* device related */
MED_RET_T fat_mount(int drive,unsigned int startsector,struct vfs_node ** mounted_root);
MED_RET_T fat_unmount(struct vfs_node * root, int flush);
MED_RET_T fat_freeDirEntries(struct vfs_node * opened_file);
MED_RET_T fat_fileRemove(struct vfs_node * opened_file);

/*vfs related */
MED_RET_T fat_loadDir(struct vfs_node * parent_node);

/* file related*/
MED_RET_T    fat_fileTruncate(struct vfs_node * opened_file, unsigned int size);
MED_RET_T    fat_fileSeek(struct vfs_node * opened_file,unsigned int pos);
MED_RET_T    fat_fileFlushCache(struct vfs_node * opened_file);
MED_RET_T    fat_fileOpen(struct vfs_node * opened_file);
MED_RET_T    fat_fileClose(struct vfs_node * opened_file);
MED_RET_T    fat_fileSync(struct vfs_node * opened_file);
MED_RET_T    fat_createFile(const char* name,struct vfs_node* file,struct vfs_node* dir);
unsigned int fat_fileSize(struct vfs_node * opened_file);
int fat_fileWrite(struct vfs_node * opened_file, void* buf, unsigned int count);
int fat_fileRead(struct vfs_node * opened_file, void* buf, unsigned int count);
int fat_attribute(struct vfs_node * opened_file);
#endif
