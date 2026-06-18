/* 
*   kernel/fs/file.c
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

#include <kernel/kfile.h>
#include <kernel/fat.h>
#include <kernel/kdir.h>
#include <kernel/kernel.h>

/*
  These functions provide a roughly POSIX-compatible file IO API.

  Since the fat32 driver only manages sectors, we maintain a one-sector
  cache for each open file. This way we can provide byte access without
  having to re-read the sector each time. 
  The penalty is the RAM used for the cache and slightly more complex code.
*/

#define MAX_OPEN_FILES 8

struct filedesc {
    unsigned char cache[SECTOR_SIZE];
    int cacheoffset;
    int fileoffset;
    int size;
    int attr;
    struct fat_file fatfile;
    bool busy;
    bool write;
    bool dirty;
    bool trunc;
};

static struct filedesc openfiles[MAX_OPEN_FILES];

static int flush_cache(int fd);

void init_file(void)
{
    int fd;
    for ( fd=0; fd<MAX_OPEN_FILES; fd++ )
        openfiles[fd].busy=false;
}

int kfcreat(const char *pathname, mode_t mode)
{
    (void)mode;
    return kfopen(pathname, O_WRONLY|O_CREAT|O_TRUNC);
}

int kfopen(const char* pathname, int flags)
{
    DIR* dir;
    struct dirent* entry;
    int fd;
    char pathnamecopy[MAX_PATH];
    char* name;
    struct filedesc* file = NULL;
    int rc;

    //printk("open(\"%s\",%d)\n",pathname,flags);

    if ( pathname[0] != '/' ) {
        printk("'%s' is not an absolute path.\n",pathname);
        printk("Only absolute pathnames supported at the moment\n");
        return -1;
    }

    /* find a free file descriptor */
    for ( fd=0; fd<MAX_OPEN_FILES; fd++ )
        if ( !openfiles[fd].busy )
            break;

    if ( fd == MAX_OPEN_FILES ) {
        printk("Too many files open\n");
        return -2;
    }

    file = &openfiles[fd];
    memset(file, 0, sizeof(struct filedesc));

    if (flags & (O_RDWR | O_WRONLY)) {
        file->write = true;

        if (flags & O_TRUNC)
            file->trunc = true;
    }
    file->busy = true;

    strncpy(pathnamecopy,pathname,sizeof(pathnamecopy));
    pathnamecopy[sizeof(pathnamecopy)-1] = 0;

    /* locate filename */
    name=strrchr(pathnamecopy+1,'/');
    if ( name ) {
        *name = 0; 
        dir = kopendir(pathnamecopy);
        *name = '/';
        name++;
    }
    else {
        dir = kopendir("/");
        name = pathnamecopy+1;
    }
    if (!dir) {
        printk("Failed opening dir\n");
        file->busy = false;
        return -4;
    }

    if(name[0] == 0) {
        printk("Empty file name\n");
        file->busy = false;
        kclosedir(dir);
        return -5;
    }
    
    /* scan dir for name */
    while ((entry = kreaddir(dir))) {
        if ( !strcasecmp(name, entry->d_name) ) {
            fat_open(dir->fatdir.file.volume,
                     entry->startcluster,
                     &(file->fatfile),
                     &(dir->fatdir));
            file->size = file->trunc ? 0 : entry->size;
            file->attr = entry->attribute;
            break;
        }
    }

    if ( !entry ) {
        //printk("Didn't find file %s\n",name);
        if ( file->write && (flags & O_CREAT) ) {
            rc = fat_create_file(name,
                                 &(file->fatfile),
                                 &(dir->fatdir));
            if (rc < 0) {
                printk("Couldn't create %s in %s\n",name,pathnamecopy);
                file->busy = false;
                kclosedir(dir);
                return rc * 10 - 6;
            }
            file->size = 0;
            file->attr = 0;
        }
        else {
            printk("Couldn't find %s in %s\n",name,pathnamecopy);
            file->busy = false;
            kclosedir(dir);
            return -7;
        }
    } else {
        if(file->write && (file->attr & FAT_ATTR_DIRECTORY)) {
            file->busy = false;
            kclosedir(dir);
            return -8;
        }
    }
    kclosedir(dir);

    file->cacheoffset = -1;
    file->fileoffset = 0;

    if (file->write && (flags & O_APPEND)) {
        rc = klseek(fd,0,SEEK_END);
        if (rc < 0 )
            return rc * 10 - 9;
    }

    return fd;
}

int kfclose(int fd)
{
    struct filedesc* file = &openfiles[fd];
    int rc = 0;

    //printk("close(%d)\n", fd);

    if (fd < 0 || fd > MAX_OPEN_FILES-1) {
        return -1;
    }
    if (!file->busy) {
        return -2;
    }
    if (file->write) {
        rc = kfsync(fd);
        if (rc < 0)
            return rc * 10 - 3;
    }

    file->busy = false;
    return 0;
}

int kfsync(int fd)
{
    struct filedesc* file = &openfiles[fd];
    int rc = 0;

    //printk("fsync(%d)\n", fd);

    if (fd < 0 || fd > MAX_OPEN_FILES-1) {
        return -1;
    }
    if (!file->busy) {
        return -2;
    }
    if (file->write) {
        /* flush sector cache */
        if ( file->dirty ) {
            rc = flush_cache(fd);
            if (rc < 0)
                return rc * 10 - 3;
        }

        /* truncate? */
        if (file->trunc) {
            rc = kftruncate(fd, file->size);
            if (rc < 0)
                return rc * 10 - 4;
        }

        /* tie up all loose ends */
        rc = fat_closewrite(&(file->fatfile), file->size, file->attr);
        if (rc < 0)
            return rc * 10 - 5;
    }
    return 0;
}

int kfremove(const char* name)
{
    int rc;
    struct filedesc* file;
    int fd = kfopen(name, O_WRONLY);
    if ( fd < 0 )
        return fd * 10 - 1;

    file = &openfiles[fd];
    rc = fat_remove(&(file->fatfile));
    if ( rc < 0 ) {
        printk("Failed removing file: %d\n", rc);
        return rc * 10 - 3;
    }

    file->size = 0;

    rc = kfclose(fd);
    if (rc<0)
        return rc * 10 - 4;

    return 0;
}

int kfrename(const char* path, const char* newpath)
{
    int rc, fd;
    DIR* dir;
    char* nameptr;
    char* dirptr;
    struct filedesc* file;
    char newpath2[MAX_PATH];

    /* verify new path does not already exist */
    /* If it is a directory, errno == EISDIR if the name exists */
    fd = kfopen(newpath, O_RDONLY);
#warning original code was also checking || errno == EISDIR
    if ( fd >= 0 ) {
        kfclose(fd);
        return -1;
    }
    kfclose(fd);

    fd = kfopen(path, O_RDONLY);
    if ( fd < 0 ) {
        return fd * 10 - 2;
    }

    /* extract new file name */
    nameptr = strrchr(newpath,'/');
    if (nameptr)
        nameptr++;
    else
        return - 3;

    /* Extract new path */
    strcpy(newpath2, newpath);
    
    dirptr = strrchr(newpath2,'/');
    if(dirptr)
        *dirptr = 0;
    else
        return - 4;

    dirptr = newpath2;
    
    if(strlen(dirptr) == 0) {
        dirptr = "/";
    }
    
    dir = kopendir(dirptr);
    if(!dir)
        return - 5;
    
    file = &openfiles[fd];
    rc = fat_rename(&file->fatfile, &dir->fatdir, nameptr,
                    file->size, file->attr);
    if ( rc < 0 ) {
        printk("Failed renaming file: %d\n", rc);
        return rc * 10 - 6;
    }

    rc = kfclose(fd);
    if (rc<0) {
        return rc * 10 - 7;
    }

    rc = kclosedir(dir);
    if (rc<0) {
        return rc * 10 - 8;
    }

    return 0;
}

int kftruncate(int fd, off_t size)
{
    int rc, sector;
    struct filedesc* file = &openfiles[fd];

    sector = size / SECTOR_SIZE;
    if (size % SECTOR_SIZE)
        sector++;

    rc = fat_seek(&(file->fatfile), sector);
    if (rc < 0) {
        return rc * 10 - 1;
    }

    rc = fat_truncate(&(file->fatfile));
    if (rc < 0) {
        return rc * 10 - 2;
    }

    file->size = size;

    return 0;
}

static int flush_cache(int fd)
{
    int rc;
    struct filedesc* file = &openfiles[fd];
    int sector = file->fileoffset / SECTOR_SIZE;
      
    //printk("Flushing dirty sector cache\n");
        
    /* make sure we are on correct sector */
    rc = fat_seek(&(file->fatfile), sector);
    if ( rc < 0 )
        return rc * 10 - 3;

    rc = fat_readwrite(&(file->fatfile), 1,
                       file->cache, true );

    if ( rc < 0 ) {
        if(file->fatfile.eof)
        return rc * 10 - 2;
    }

    file->dirty = false;

    return 0;
}

static int readwrite(int fd, void* buf, int count, bool write)
{
    int sectors;
    int nread=0;
    struct filedesc* file = &openfiles[fd];
    int rc;

    if ( !file->busy ) {
        return -1;
    }

    /*printk( "readwrite(%d,%x,%d,%s)\n",
             fd,buf,count,write?"write":"read");*/

    /* attempt to read past EOF? */
    if (!write && count > file->size - file->fileoffset)
        count = file->size - file->fileoffset;

    /* any head bytes? */
    if ( file->cacheoffset != -1 ) {
        int headbytes;
        int offs = file->cacheoffset;
        if ( count <= SECTOR_SIZE - file->cacheoffset ) {
            headbytes = count;
            file->cacheoffset += count;
            if ( file->cacheoffset >= SECTOR_SIZE )
                file->cacheoffset = -1;
        }
        else {
            headbytes = SECTOR_SIZE - file->cacheoffset;
            file->cacheoffset = -1;
        }

        if (write) {
            memcpy( file->cache + offs, buf, headbytes );
            if (offs+headbytes == SECTOR_SIZE) {
                int rc = flush_cache(fd);
                if ( rc < 0 ) {
                    return rc * 10 - 2;
                }
                file->cacheoffset = -1;
            }
            else
                file->dirty = true;
        }
        else {
            memcpy( buf, file->cache + offs, headbytes );
        }

        nread = headbytes;
        count -= headbytes;
    }
    
    /* If the buffer has been modified, either it has been flushed already
     * (if (offs+headbytes == SECTOR_SIZE)...) or does not need to be (no
     * more data to follow in this call). Do NOT flush here. */

    /* read/write whole sectors right into/from the supplied buffer */
    sectors = count / SECTOR_SIZE;
    if ( sectors ) {
        int rc = fat_readwrite(&(file->fatfile), sectors,
            (unsigned char*)buf+nread, write );
        if ( rc < 0 ) {
            printk("Failed read/writing %d sectors\n",sectors);
            if(write && file->fatfile.eof) {
                printk("No space left on device\n");
            } else {
                file->fileoffset += nread;
            }
            file->cacheoffset = -1;
            return nread ? nread : rc * 10 - 4;
        }
        else {
            if ( rc > 0 ) {
                nread += rc * SECTOR_SIZE;
                count -= sectors * SECTOR_SIZE;

                /* if eof, skip tail bytes */
                if ( rc < sectors )
                    count = 0;
            }
            else {
                /* eof */
                count=0;
            }

            file->cacheoffset = -1;
        }
    }

    /* any tail bytes? */
    if ( count ) {
        if (write) {
            if ( file->fileoffset + nread < file->size ) {
                /* sector is only partially filled. copy-back from disk */
                int rc;
                //printk("Copy-back tail cache\n");
                rc = fat_readwrite(&(file->fatfile), 1, file->cache, false );
                if ( rc < 0 ) {
                    printk("Failed writing\n");
                    file->fileoffset += nread;
                    file->cacheoffset = -1;
                    return nread ? nread : rc * 10 - 5;
                }
                /* seek back one sector to put file position right */
                rc = fat_seek(&(file->fatfile), 
                              (file->fileoffset + nread) / 
                              SECTOR_SIZE);
                if ( rc < 0 ) {
                    printk("fat_seek() failed\n");
                    file->fileoffset += nread;
                    file->cacheoffset = -1;
                    return nread ? nread : rc * 10 - 6;
                }
            }
            memcpy( file->cache, (unsigned char*)buf + nread, count );
            file->dirty = true;
        }
        else {
            rc = fat_readwrite(&(file->fatfile), 1, &(file->cache),false);
            if (rc < 1 ) {
                printk("Failed caching sector\n");
                file->fileoffset += nread;
                file->cacheoffset = -1;
                return nread ? nread : rc * 10 - 7;
            }
            memcpy( (unsigned char*)buf + nread, file->cache, count );
        }
            
        nread += count;
        file->cacheoffset = count;
    }

    file->fileoffset += nread;
    //printk("fileoffset: %d\n", file->fileoffset);

    /* adjust file size to length written */
    if ( write && file->fileoffset > file->size )
        file->size = file->fileoffset;

    return nread;
}

ssize_t kfwrite(int fd, const void* buf, size_t count)
{
    if (!openfiles[fd].write) {
        return -1;
    }
    return readwrite(fd, (void *)buf, count, true);
}

ssize_t kfread(int fd, void* buf, size_t count)
{
    return readwrite(fd, buf, count, false);
}

off_t kftell(int fd)
{
    struct filedesc* file = &openfiles[fd];
    if ( !file->busy ) {
        return -1;
    }
    return file->fileoffset;
}

off_t klseek(int fd, off_t offset, int whence)
{
    int pos;
    int newsector;
    int oldsector;
    int sectoroffset;
    int rc;
    struct filedesc* file = &openfiles[fd];

    //printk("lseek(%d,%d,%d)\n",fd,offset,whence);

    if ( !file->busy ) {
        return -1;
    }

    switch ( whence ) {
        case SEEK_SET:
            pos = offset;
            break;

        case SEEK_CUR:
            pos = file->fileoffset + offset;
            break;

        case SEEK_END:
            pos = file->size + offset;
            break;

        default:
            return -2;
    }
    if ((pos < 0) || (pos > file->size)) {
        return -3;
    }

    /* new sector? */
    newsector = pos / SECTOR_SIZE;
    oldsector = file->fileoffset / SECTOR_SIZE;
    sectoroffset = pos % SECTOR_SIZE;

    if ( (newsector != oldsector) ||
         ((file->cacheoffset==-1) && sectoroffset) ) {

        if ( newsector != oldsector ) {
            if (file->dirty) {
                rc = flush_cache(fd);
                if (rc < 0)
                    return rc * 10 - 5;
            }
            
            rc = fat_seek(&(file->fatfile), newsector);
            if ( rc < 0 ) {
                return rc * 10 - 4;
            }
        }
        if ( sectoroffset ) {
            rc = fat_readwrite(&(file->fatfile), 1,
                               &(file->cache),false);
            if ( rc < 0 ) {
                return rc * 10 - 6;
            }
            file->cacheoffset = sectoroffset;
        }
        else
            file->cacheoffset = -1;
    }
    else
        if ( file->cacheoffset != -1 )
            file->cacheoffset = sectoroffset;

    file->fileoffset = pos;

    return pos;
}

int kfilesize(int fd)
{
    struct filedesc* file = &openfiles[fd];

    if ( !file->busy ) {
        return -1;
    }
    
    return file->size;
}
