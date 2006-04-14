/*
*   kernel/fs/fat.c
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

#include <sys_def/string.h>
#include <sys_def/ctype.h>

#include <kernel/kernel.h>
#include <kernel/errors.h>
#include <kernel/malloc.h>

#include <kernel/ata.h>

#include <kernel/fat.h>
#include <kernel/vfs.h>
#include <kernel/vfs_node.h>
#include <kernel/vfs_pathname.h>

#include "fat_defines.h"

struct file_cache cache_list[MAX_OPEN];
unsigned char readdir_buf [3][SECTOR_SIZE];

extern char fat_cache_sectors[FAT_CACHE_SIZE][SECTOR_SIZE];
extern struct fat_cache_entry fat_cache[FAT_CACHE_SIZE];

int fat_mount(int drive,unsigned int startsector,struct vfs_node ** mounted_root)
{
    MED_RET_T ret_val;
    struct bpb * fat_bpb;
    struct vfs_node * root_node;
    struct fat_entry * dir_entry;

    fat_bpb = (struct bpb*)malloc(sizeof(struct bpb));

    if(!fat_bpb)
    {
        printk("[FAT] mount error: can't allocate mem for bpb drive=%d\n",drive);
        ret_val= -MED_ENOMEM;
        goto alloc_bpb_err;
    }

    ret_val=fat_initDevice(fat_bpb,drive,startsector);
    if(ret_val!=MED_OK)
        goto root_node_err;

    root_node = (struct vfs_node *)malloc(sizeof(struct vfs_node));
    if(!root_node)
    {
        ret_val = -MED_ENOMEM;
        goto root_node_err;
    }

    memset(root_node,0,sizeof(struct vfs_node));

    root_node->storage_location = fat_bpb->bpb_rootclus;
    root_node->type=VFS_TYPE_DIR;
    root_node->name.str=malloc(2);
    root_node->name.str[0]='/';
    root_node->name.str[1]='\0';
    root_node->name.length=1;

    dir_entry = (struct fat_entry *)malloc(sizeof(struct fat_entry ));
    if(!dir_entry)
    {
        ret_val = -MED_ENOMEM;
        goto gen_err;
    }

    memset(dir_entry,0,sizeof(struct fat_entry));
    dir_entry->firstcluster = dir_entry->lastcluster = root_node->storage_location;
    dir_entry->fat_bpb=fat_bpb;

    root_node->custom_data=(void*)dir_entry;

    *mounted_root=root_node;

    return MED_OK;

gen_err:
    free(root_node);
root_node_err:
    free(fat_bpb);
alloc_bpb_err:
    return ret_val;
}

MED_RET_T fat_unmount(struct vfs_node * root, int flush)
{

    MED_RET_T ret_val;
    struct fat_entry * root_entry = (struct fat_entry *)root->custom_data;
    struct bpb* fat_bpb = root_entry->fat_bpb;


    if(flush)
    {
        ret_val = fat_flushFat(fat_bpb); /* the clean way, while still alive */
    }
    else
    {   /* volume is not accessible any more, e.g. MMC removed */
        int i;
        for(i = 0;i < FAT_CACHE_SIZE;i++)
        {
            struct fat_cache_entry *fce = &fat_cache[i];
            if(fce->inuse && fce->fat_vol == fat_bpb)
            {
                fce->inuse = false; /* discard all from that volume */
                fce->dirty = false;
            }
        }

        ret_val = MED_OK;
    }
    return ret_val;
}

int fat_writeLongName(struct fat_entry * file,unsigned int firstentry,unsigned int numentries,
               const unsigned char* name,const unsigned char* shortname,int is_directory)
{
    unsigned char buf[SECTOR_SIZE];
    unsigned char* entry;
    unsigned int idx = firstentry % DIR_ENTRIES_PER_SECTOR;
    unsigned int sector = firstentry / DIR_ENTRIES_PER_SECTOR;
    unsigned int i, j=0;
    unsigned char chksum = 0;
    int nameidx=0, namelen = strlen(name);
    int rc;

    /*printk("write_long_name(file:%x, first:%d, num:%d, name:%s)\n",
            file->firstcluster, firstentry, numentries, name);*/

    rc = fat_seek(file, sector);
    if (rc!=MED_OK)
        return rc;

    rc = fat_readWrite(file, 1, buf, false);
    if (rc<1)
        return rc * 10 - 2;

    /* calculate shortname checksum */
    for (i=11; i>0; i--)
        chksum = ((chksum & 1) ? 0x80 : 0) + (chksum >> 1) + shortname[j++];

    /* calc position of last name segment */
    if ( namelen > NAME_BYTES_PER_ENTRY )
        for (nameidx=0;
             nameidx < (namelen - NAME_BYTES_PER_ENTRY);
             nameidx += NAME_BYTES_PER_ENTRY);

    for (i=0; i < numentries; i++) {
        /* new sector? */
        if ( idx >= DIR_ENTRIES_PER_SECTOR ) {
            /* update current sector */
            rc = fat_seek(file, sector);
            if (rc<0)
                return rc * 10 - 3;

            rc = fat_readWrite(file, 1, buf, true);
            if (rc<1)
                return rc * 10 - 4;

            /* read next sector */
            rc = fat_readWrite(file, 1, buf, false);
            if (rc<0) {
                //printk("Failed writing new sector: %d\n",rc);
                return rc * 10 - 5;
            }
            if (rc==0)
                /* end of dir */
                memset(buf, 0, sizeof buf);

            sector++;
            idx = 0;
        }

        entry = buf + idx * DIR_ENTRY_SIZE;

        /* verify this entry is free */
        if (entry[0] && entry[0] != 0xe5 )
            printk("Dir entry %d in sector %x is not free! "
                   "%02x %02x %02x %02x",
                   idx, sector,
                   entry[0], entry[1], entry[2], entry[3]);

        memset(entry, 0, DIR_ENTRY_SIZE);
        if ( i+1 < numentries ) {
            /* longname entry */
            int k, l = nameidx;

            entry[FATLONG_ORDER] = numentries-i-1;
            if (i==0) {
                /* mark this as last long entry */
                entry[FATLONG_ORDER] |= 0x40;

                /* pad name with 0xffff  */
                for (k=1; k<12; k++) entry[k] = 0xff;
                for (k=14; k<26; k++) entry[k] = 0xff;
                for (k=28; k<32; k++) entry[k] = 0xff;
            };
            /* set name */
            for (k=0; k<5 && l <= namelen; k++) {
                entry[k*2 + 1] = name[l++];
                entry[k*2 + 2] = 0;
            }
            for (k=0; k<6 && l <= namelen; k++) {
                entry[k*2 + 14] = name[l++];
                entry[k*2 + 15] = 0;
            }
            for (k=0; k<2 && l <= namelen; k++) {
                entry[k*2 + 28] = name[l++];
                entry[k*2 + 29] = 0;
            }

            entry[FATDIR_ATTR] = FAT_ATTR_LONG_NAME;
            entry[FATDIR_FSTCLUSLO] = 0;
            entry[FATLONG_TYPE] = 0;
            entry[FATLONG_CHKSUM] = chksum;
            //printk("Longname entry %d: %.13s\n", idx, name+nameidx);
        }
        else {
            /* shortname entry */
            unsigned short date=0, time=0, tenth=0;
            //printk("Shortname entry: %.13s\n", shortname);
            strncpy(entry + FATDIR_NAME, shortname, 11);
            entry[FATDIR_ATTR] = is_directory?FAT_ATTR_DIRECTORY:0;
            entry[FATDIR_NTRES] = 0;

            fat_time(&date, &time, &tenth);
            entry[FATDIR_CRTTIMETENTH] = tenth;
            *(unsigned short*)(entry + FATDIR_CRTTIME) = SWAB16(time);
            *(unsigned short*)(entry + FATDIR_WRTTIME) = SWAB16(time);
            *(unsigned short*)(entry + FATDIR_CRTDATE) = SWAB16(date);
            *(unsigned short*)(entry + FATDIR_WRTDATE) = SWAB16(date);
            *(unsigned short*)(entry + FATDIR_LSTACCDATE) = SWAB16(date);
        }
        idx++;
        nameidx -= NAME_BYTES_PER_ENTRY;
    }

    /* update last sector */
    rc = fat_seek(file, sector);
    if (rc<0)
        return rc * 10 - 6;

    rc = fat_readWrite(file, 1, buf, true);
    if (rc<1)
        return rc * 10 - 7;

    return MED_OK;
}

MED_RET_T fat_addDirEntry(struct vfs_node * dir_node,struct vfs_node * file_node,
    const char* name,int is_directory,int dotdir)
{
    struct fat_entry * dir_entry = (struct fat_entry *)dir_node->custom_data;
    struct fat_entry * file_entry = (struct fat_entry *)file_node->custom_data;
    struct bpb* fat_bpb = dir_entry->fat_bpb;
    unsigned char buf[SECTOR_SIZE];
    unsigned char shortname[16];
    MED_RET_T ret_val;
    unsigned int sector;
    int done = 0;
    int eof = 0;
    int last = 0;
    int entries_needed, entries_found = 0;
    int namelen = strlen(name);
    int firstentry;

    VFS_PRINT("[fat_addDirEntry] start\n");

    /* The "." and ".." directory entries must not be long names */
    if(dotdir) {
        int i;
        strncpy(shortname, name, 16);
        for(i = strlen(shortname);i < 12;i++)
            shortname[i] = ' ';

        entries_needed = 1;
    } else {
        /* create dos name */
        fat_createDosName(name, shortname);

        /* one dir entry needed for every 13 bytes of filename,
           plus one entry for the short name */
        entries_needed = namelen / NAME_BYTES_PER_ENTRY + 1;
        if (namelen % NAME_BYTES_PER_ENTRY)
            entries_needed++;
    }

 restart:
    firstentry = 0;

    ret_val = fat_seek(dir_entry, 0);
    if (ret_val!=MED_OK)
        return ret_val;

    for (sector=0; !done; sector++)
    {
        unsigned int i;

        ret_val = 0;
        if (!eof) {
            ret_val = fat_readWrite(dir_entry, 1, buf, 0);
        }
        if (ret_val == 0) {
            /* eof: add new sector */
            eof = 1;

            memset(buf, 0, sizeof buf);
            VFS_PRINT("Adding new sector to dir\n");
            ret_val = fat_seek(dir_entry, sector);
            if (ret_val!=MED_OK)
        return ret_val;

            /* add sectors (we must clear the whole cluster) */
            do {
                ret_val = fat_readWrite(dir_entry, 1, buf, 1);
                if (ret_val < 1)
                    return -MED_EIO;
            } while (dir_entry->sectornum < (int)fat_bpb->bpb_secperclus);
        }
        if (ret_val <0) {
            return ret_val;
        }

        /* look for free slots */
        for (i=0; i < DIR_ENTRIES_PER_SECTOR && !done; i++)
        {
            unsigned char firstbyte = buf[i * DIR_ENTRY_SIZE];
            switch (firstbyte) {
            case 0: /* end of dir */
                entries_found = entries_needed;
                VFS_PRINT("Found last entry %d\n",
                        sector * DIR_ENTRIES_PER_SECTOR + i);
                done = 1;
                break;

            case 0xe5: /* free entry */
                entries_found++;
                VFS_PRINT("Found free entry %d (%d/%d)\n",
                        sector * DIR_ENTRIES_PER_SECTOR + i,
                        entries_found, entries_needed);
                break;

            default:
                entries_found = 0;

                /* check that our intended shortname doesn't already exist */
                if (!strncmp(shortname, buf + i * DIR_ENTRY_SIZE, 12)) {
                    /* filename exists already. make a new one */
                    //snprintf(shortname+8, 4, "%03X", (unsigned)rand() & 0xfff);
#warning rand needed when duplicated short name
                    printk("Duplicate shortname, changing to %s\n",
                            shortname);

                    /* name has changed, we need to restart search */
                    goto restart;
                }
                break;
            }

            if (!firstentry && (entries_found == entries_needed)) {
                firstentry = sector * DIR_ENTRIES_PER_SECTOR + i;

                /* if we're not extending the dir,
                   we must go back to first free entry */
                if (done)
                    last = 1;
                else
                    firstentry -= (entries_needed - 1);
            }
        }
    }

    sector = firstentry / DIR_ENTRIES_PER_SECTOR;
    VFS_PRINT("Adding longname to entry %d in sector %d\n",
            firstentry, sector);

    ret_val = fat_writeLongName(dir_entry, firstentry,
                         entries_needed, name, shortname, is_directory);
    if (ret_val < 0)
        return ret_val;

    /* remember where the shortname dir entry is located */
    file_entry->dirEntryNum = firstentry + entries_needed - 1;
    file_entry->nbDirEntries = entries_needed;
    VFS_PRINT("Added new dir entry %d, using %d slots.\n",
            file_entry->dirEntryNum, file_entry->nbDirEntries);

    /* advance the end-of-dir marker? */
    if (last)
    {
        unsigned int lastentry = firstentry + entries_needed;

        VFS_PRINT("Updating end-of-dir entry %d\n",lastentry);

        if (lastentry % DIR_ENTRIES_PER_SECTOR)
        {
            int idx = (lastentry % DIR_ENTRIES_PER_SECTOR) * DIR_ENTRY_SIZE;

            ret_val = fat_seek(dir_entry, lastentry / DIR_ENTRIES_PER_SECTOR);
            if (ret_val < 0)
                return ret_val;

            ret_val = fat_readWrite(dir_entry, 1, buf, 0);
            if (ret_val < 1)
                return ret_val;

            /* clear last entry */
            buf[idx] = 0;

            ret_val= fat_seek(dir_entry, lastentry / DIR_ENTRIES_PER_SECTOR);
            if (ret_val < 0)
                return ret_val;

            /* we must clear entire last cluster */
            do {
                ret_val = fat_readWrite(dir_entry, 1, buf, 1);
                if (ret_val < 1)
                    return ret_val;
                memset(buf, 0, sizeof buf);
            } while (dir_entry->sectornum < (int)fat_bpb->bpb_secperclus);
        }
        else
        {
            /* add a new sector/cluster for last entry */
            memset(buf, 0, sizeof buf);
            do {
                ret_val = fat_readWrite(dir_entry, 1, buf, 1);
                if (ret_val < 1)
                    return ret_val; /* Same error code as above, can't be
                                           more than -9 */
            } while (dir_entry->sectornum < (int)fat_bpb->bpb_secperclus);
        }
    }
    return MED_OK;
}

MED_RET_T fat_updateShortEntry(struct vfs_node * file, int size, int attr)
{
    struct fat_entry * file_entry = (struct fat_entry *)file->custom_data;

    unsigned char buf[SECTOR_SIZE];
    int sector = file_entry->dirEntryNum / DIR_ENTRIES_PER_SECTOR;
    unsigned char* entry = buf + DIR_ENTRY_SIZE * (file_entry->dirEntryNum % DIR_ENTRIES_PER_SECTOR);
    unsigned int* sizeptr;
    unsigned short* clusptr;
    struct fat_entry * dir_entry = (struct fat_entry *)(file->parent->custom_data);
    MED_RET_T ret_val;



    VFS_PRINT("update_file_size(cluster:%x entry:%d size:%d)\n",
            file_entry->firstcluster, file_entry->dirEntryNum, size);

    /*find dir entry in parent dir*/
    ret_val = fat_seek( dir_entry, sector);
    if (ret_val!=MED_OK)
        return ret_val;

    ret_val = fat_readWrite(dir_entry, 1, buf, false);
    if (ret_val < 1)
        return ret_val;

    if (!entry[0] || entry[0] == 0xe5)
        printk("Updating size on empty dir entry %d\n", file_entry->dirEntryNum);

    entry[FATDIR_ATTR] = attr & 0xFF;

    clusptr = (short*)(entry + FATDIR_FSTCLUSHI);
    *clusptr = SWAB16(file_entry->firstcluster >> 16);

    clusptr = (short*)(entry + FATDIR_FSTCLUSLO);
    *clusptr = SWAB16(file_entry->firstcluster & 0xffff);

    sizeptr = (int*)(entry + FATDIR_FILESIZE);
    *sizeptr = SWAB32(size);

    {
#ifdef HAVE_RTC
        unsigned short time = 0;
        unsigned short date = 0;
#else
        /* get old time to increment from */
        unsigned short time = SWAB16(*(unsigned short*)(entry + FATDIR_WRTTIME));
        unsigned short date = SWAB16(*(unsigned short*)(entry + FATDIR_WRTDATE));
#endif
        fat_time(&date, &time, NULL);
        *(unsigned short*)(entry + FATDIR_WRTTIME) = SWAB16(time);
        *(unsigned short*)(entry + FATDIR_WRTDATE) = SWAB16(date);
        *(unsigned short*)(entry + FATDIR_LSTACCDATE) = SWAB16(date);
    }

    ret_val = fat_seek( dir_entry, sector );
    if (ret_val!=MED_OK)
        return ret_val;

    ret_val = fat_readWrite(dir_entry, 1, buf, true);
    if (ret_val < 1)
        return ret_val;

    return MED_OK;
}


void fat_truncate(struct fat_entry * entry)
{
    /* truncate trailing clusters */
    int next;
    int last =  entry->lastcluster;
    struct bpb* fat_bpb = entry->fat_bpb;

    for ( last = fat_getNextCluster(fat_bpb, last); last; last = next ) {
        next = fat_getNextCluster(fat_bpb, last);
        fat_updateFatEntry(fat_bpb, last,0);
    }
    if (entry->lastcluster)
        fat_updateFatEntry(fat_bpb, entry->lastcluster,FAT_EOF_MARK);
}

MED_RET_T fat_closeWrite(struct vfs_node * opened_file,int size,int attr)
{
    MED_RET_T ret_val;
    struct fat_entry * entry=(struct fat_entry*)(opened_file->custom_data);
    struct bpb* fat_bpb = entry->fat_bpb;

    VFS_PRINT("fat_closeWrite: size=%d, attr=%d\n",size,attr);

    if (!size)
    {
        /* empty file */
        if ( entry->firstcluster )
        {
            fat_updateFatEntry(fat_bpb, entry->firstcluster, 0);
            entry->firstcluster = 0;
        }
    }

    if (opened_file->parent)
    {
        ret_val = fat_updateShortEntry(opened_file,size,attr);
        if (ret_val != MED_OK)
            return ret_val;
    }

    fat_flushFat(fat_bpb);

    return MED_OK;
}

int fat_readWrite( struct fat_entry * entry, int sectorcount,void* buf, int write )
{
#warning need to update retval system => everywhere we have rc
    struct bpb* fat_bpb = entry->fat_bpb;
    int cluster = entry->lastcluster;
    int sector = entry->lastsector;
    int clusternum = entry->clusternum;
    int numsec = entry->sectornum;
    int eof = entry->eof;
    int first=0, last=0;
    int i;
    int rc;

    VFS_PRINT("[FAT READ/WRITE] cluster=%x sector=%x\nclusternum=%x numsec=%x\neof=%x mode=%s\n",
        cluster,sector,clusternum,numsec,eof,write?"W":"R");

    if ( eof && !write)
    {
        VFS_PRINT("[FAT READ/WRITE] EOF & !write => ret 0\n");
        return 0;
    }

    /* find sequential sectors and write them all at once */
    for (i=0; (i < sectorcount) && (sector > -1); i++ )
    {
        VFS_PRINT("[FAT READ/WRITE] read sect %d/%d\n",i,sectorcount);
        numsec++;
        if ( numsec > (int)fat_bpb->bpb_secperclus || !cluster )
        {
            int oldcluster = cluster;
            VFS_PRINT("need to find cluster num\n");
            if (write)
                cluster = fat_nextWriteCluster(entry, cluster, &sector);
            else
            {
                cluster = fat_getNextCluster(fat_bpb, cluster);
                sector = fat_cluster2Sec(fat_bpb, cluster);
            }

            clusternum++;
            numsec=1;

            VFS_PRINT("get: cluster=%x sector=%x\n",cluster,sector);

            if (!cluster)
            {
                eof = 1;
                VFS_PRINT("[FAT READ/WRITE] set EOF\n");
                if ( write )
                {
                    /* remember last cluster, in case
                       we want to append to the file */
                    cluster = oldcluster;
                    clusternum--;
                    i = -1; /* Error code */
                    break;
                }
            }
            else
                eof = 0;
        }
        else
        {
            if (sector)
                sector++;
            else {
                /* look up first sector of file */
                sector = fat_cluster2Sec(fat_bpb, entry->firstcluster);
                VFS_PRINT("cluster2sec: in=%x,out=%x\n",entry->firstcluster,sector);
                numsec=1;
                if (entry->firstcluster < 0)
                {   /* FAT16 root dir */
                    sector += fat_bpb->rootdiroffset;
                    numsec += fat_bpb->rootdiroffset;
                }
            }
        }

        if (!first)
            first = sector;

        if ( ((sector != first) && (sector != last+1)) || /* not sequential */
             (last-first+1 == 256) )     /* max 256 sectors per ata request */
        {
            int count = last - first + 1;
            VFS_PRINT("About to xfer %x, sect=%x (using first)\n",count,first);
            rc = fat_transfer(fat_bpb, first, count, buf, write );
            VFS_PRINT("ret=%d from fat_transfer\n",rc);
            if (rc < 0)
            {
                VFS_PRINT("=>fat_transfer error:%d\n",-rc);
                return rc * 10 - 1;
            }
            buf = (char *)buf + count * SECTOR_SIZE;
            first = sector;
        }

        if ((i == sectorcount-1) && /* last sector requested */
            (!eof))
        {
            int count = sector - first + 1;
            VFS_PRINT("About to xfer %x, sect=%x (last sector to read)\n",count,first);
            rc = fat_transfer(fat_bpb, first, count, buf, write );
            VFS_PRINT("ret=%d from fat_transfer\n",rc);
            if (rc < 0)
            {
                VFS_PRINT("=>fat_transfer error:%d\n",-rc);
                return rc * 10 - 2;
            }
        }

        last = sector;
    }

    VFS_PRINT("Out read/write loop: i=%d,eof=%d\n",i,eof);

    entry->lastcluster = cluster;
    entry->lastsector = sector;
    entry->clusternum = clusternum;
    entry->sectornum = numsec;
    entry->eof = eof;

    /* if eof, don't report last block as read/written */
    if (eof)
        i--;

    return i;
}

MED_RET_T fat_seek(struct fat_entry * entry, unsigned int seeksector )
{
    struct bpb* fat_bpb = entry->fat_bpb;
    int clusternum=0, numclusters=0, sectornum=0, sector=0;
    int cluster = entry->firstcluster;
    int i;

    VFS_PRINT("[FAT SEEK] %x\n",seeksector);

    if (cluster < 0) /* FAT16 root dir */
        seeksector += fat_bpb->rootdiroffset;

    entry->eof = false;
    if (seeksector) {
        /* we need to find the sector BEFORE the requested, since
           the file struct stores the last accessed sector */
        seeksector--;
        numclusters = clusternum = seeksector / fat_bpb->bpb_secperclus;
        sectornum = seeksector % fat_bpb->bpb_secperclus;

        if (entry->clusternum && clusternum >= entry->clusternum)
        {
            cluster = entry->lastcluster;
            numclusters -= entry->clusternum;
        }

        for (i=0; i<numclusters; i++) {
            cluster = fat_getNextCluster(fat_bpb, cluster);
            if (!cluster) {
                printk("Seeking beyond the end of the file! "
                       "(sector %d, cluster %d)\n", seeksector, i);
                return -MED_EINVAL;
            }
        }

        sector = fat_cluster2Sec(fat_bpb, cluster) + sectornum;
    }
    else {
        sectornum = -1;
    }

    entry->lastcluster = cluster;
    entry->lastsector = sector;
    entry->clusternum = clusternum;
    entry->sectornum = sectornum + 1;
    return MED_OK;
}

MED_RET_T fat_readdir(struct fat_entry * dir,struct fat_direntry * entry)
{
    int done = 0;
    int i;
    MED_RET_T ret_val=MED_OK;
    unsigned char firstbyte;
    unsigned char* buf = readdir_buf[0];
    int longarray[20];
    int longs=0;
    int sectoridx=0;

    dir->entryCount=0;

    while(!done)
    {
        if ( !(dir->entryN % DIR_ENTRIES_PER_SECTOR) )
        {
            ret_val = fat_readWrite(dir, 1, buf, 0);
            if (ret_val == 0)
            {
                /* eof */
                ret_val=-MED_ENOENT;
                VFS_PRINT("[READ DIR] EOF\n");
                break;
            }
            if (ret_val < 0)
            {
                printk( "fat_getnext() - Couldn't read dir"
                        " (error code %d)\n", ret_val);
                break;
            }
            //printk("fat_readWrite ok: %x\n",ret_val);
            //print_data(buf,0x200);
        }


        for (i = dir->entryN % DIR_ENTRIES_PER_SECTOR;
             i < DIR_ENTRIES_PER_SECTOR; i++)
        {
            unsigned int entrypos = i * DIR_ENTRY_SIZE;
            firstbyte = buf[entrypos];
            dir->entryN++;

            if (firstbyte == 0xe5) {
                /* free entry */
                sectoridx = 0;
                dir->entryCount=0;
                VFS_PRINT("[READ DIR] free entry\n");
                continue;
            }

            if (firstbyte == 0) {
                /* last entry */
                VFS_PRINT("[READ DIR] last entry\n");
                entry->name[0]='\0';
                ret_val = MED_OK;
                dir->entryCount=0;
                done = 1;
                break;
            }

            dir->entryCount++;

            /* longname entry? */
            if ( ( buf[entrypos + FATDIR_ATTR] &
                   FAT_ATTR_LONG_NAME_MASK ) == FAT_ATTR_LONG_NAME )
            {
                longarray[longs++] = entrypos + sectoridx;
                continue;
            }
            else
            {
                fat_parseDirEntry(entry,&buf[entrypos]);
                /* don't return volume id entry */
                VFS_PRINT("[READ DIR] found: %s\n",entry->name);
                if ( entry->attr == FAT_ATTR_VOLUME_ID )
                {
                    //printk("VOL ID => loop\n");
                    continue;
                }
                if((entry->name[0]=='.' && entry->name[1]=='\0') ||
                    (entry->name[0]=='.' && entry->name[1]=='.' && entry->name[2]=='\0'))
                {
                    //printk("empty name or dot => loop\n");
                    continue;
                }
                if ( longs )
                {
                    int j,l=0;
                    /* iterate backwards through the dir entries */
                    for (j=longs-1; j>=0; j--) {
                        unsigned char* ptr = readdir_buf[0];
                        int index = longarray[j];
                        /* current or cached sector? */
                        if ( sectoridx >= SECTOR_SIZE ) {
                            if ( sectoridx >= SECTOR_SIZE*2 ) {
                                if ( ( index >= SECTOR_SIZE ) &&
                                        ( index < SECTOR_SIZE*2 ))
                                    ptr = readdir_buf[1];
                                else
                                    ptr = readdir_buf[2];
                            }
                            else {
                                if ( index < SECTOR_SIZE )
                                    ptr = readdir_buf[1];
                            }

                            index &= SECTOR_SIZE-1;
                        }

                        /* names are stored in unicode, but we
                            only grab the low byte (iso8859-1). */
                        fat_unicode2Iso(ptr + index + 1, entry->name + l, 5);
                        l+= 5;
                        fat_unicode2Iso(ptr + index + 14, entry->name + l, 6);
                        l+= 6;
                        fat_unicode2Iso(ptr + index + 28, entry->name + l, 2);
                        l+= 2;
                    }
                    entry->name[l]=0;
                }
                done = 1;
                sectoridx = 0;
                ret_val = MED_OK;
                break;
            }
        }

         if ( sectoridx )
            memcpy( readdir_buf[2], readdir_buf[0], SECTOR_SIZE );
        else
            memcpy( readdir_buf[1], readdir_buf[0], SECTOR_SIZE );
        sectoridx += SECTOR_SIZE;
    }
    return ret_val;
}

MED_RET_T fat_loadDir(struct vfs_node * parent_node)
{
    struct fat_entry * current_dir = (struct fat_entry *)parent_node->custom_data;
    struct vfs_node * new_node;
    struct fat_entry * new_entry;
    struct fat_direntry * entry;
    int ret_val;
    current_dir->entryN=0;

    while(1)
    {
        new_node = (struct vfs_node *)malloc(sizeof(struct vfs_node));
        if(!new_node)
        {
            printk("Error malloc for new_node\n");
            return -MED_ENOMEM;
        }

        memset(new_node,0,sizeof(struct vfs_node));

        new_entry = (struct fat_entry *)malloc(sizeof(struct fat_entry ));
        if(!new_entry)
        {
            printk("Error malloc for new_entry\n");
            free(new_node);
            return -MED_ENOMEM;
        }

        memset(new_entry,0,sizeof(struct fat_entry));

        new_node->custom_data = (void*)new_entry;
        entry = &new_entry->dir_entry;


        ret_val= fat_readdir(current_dir,entry);

        if(ret_val != MED_OK)
        {
            free(new_entry);
            free(new_node);
            return ret_val;
        }

        if(entry->name[0]=='\0')
        {
            free(new_entry);
            free(new_node);
            break;
        }

        VFS_PRINT("[LOAD Dir] ok we have: %s (%d)\n",entry->name,current_dir->entryN-1);

        if ( entry->attr == FAT_ATTR_DIRECTORY )
            new_node->type=VFS_TYPE_DIR;
        else
            new_node->type=VFS_TYPE_FILE;


        new_entry->size = entry->filesize;
        new_entry->attr = entry->attr;
        new_entry->firstcluster=new_entry->lastcluster=new_node->storage_location = entry->firstcluster;
        new_entry->lastsector = new_entry->clusternum = new_entry->sectornum = new_entry->eof = 0;
        new_entry->fat_bpb=((struct fat_entry *)(parent_node->custom_data))->fat_bpb;
        new_entry->dirEntryNum = current_dir->entryN-1;
        new_entry->nbDirEntries= current_dir->entryCount;

        vfs_nodeInitChild(parent_node,NULL,new_node);

        new_node->name.str = entry->name;
        new_node->name.length = strlen(new_node->name.str);

        new_entry->cache = NULL;

    }
    parent_node->dir_loaded=1;
    return MED_OK;
}

unsigned int fat_fileSize(struct vfs_node * opened_file)
{
    return ((struct fat_entry*)opened_file->custom_data)->size;
}

MED_RET_T fat_fileTruncate(struct vfs_node * opened_file, unsigned int size)
{
    MED_RET_T ret_val;
    int sector;
    struct fat_entry* entry = (struct fat_entry*)opened_file->custom_data;


    sector = size / SECTOR_SIZE;
    if (size % SECTOR_SIZE)
        sector++;

    ret_val = fat_seek(entry, sector);
    if (ret_val != MED_OK)
        return ret_val;

#warning check if we need to add some controls in fat_truncate
    fat_truncate(entry);

    entry->size = size;

    return MED_OK;
}

MED_RET_T fat_fileSeek(struct vfs_node * opened_file,unsigned int pos)
{
    int newsector;
    int oldsector;
    int sectoroffset;
    MED_RET_T ret_val=MED_OK;
    struct fat_entry* entry = (struct fat_entry*)opened_file->custom_data;
    /* new sector? */
    newsector = pos / SECTOR_SIZE;
    oldsector = opened_file->position / SECTOR_SIZE;
    sectoroffset = pos % SECTOR_SIZE;
    VFS_PRINT("[fat_fileSeek] dest=%x old_pos=%x - dest_sect=%x old_sect=%x - sect_off %x (sect_size=%x)\n",
        pos,opened_file->position,newsector,oldsector,sectoroffset,SECTOR_SIZE);

    if ((newsector != oldsector) || ((entry->cache->cacheOffset==-1) && sectoroffset))
    {
        if(newsector != oldsector)
        {
            if(opened_file->dirty)
            {
                ret_val = fat_fileFlushCache(opened_file);
                if (ret_val != MED_OK)
                    return ret_val;
            }
            ret_val = fat_seek(entry, newsector);
            if (ret_val != MED_OK)
                return ret_val;
        }
        if(sectoroffset)
        {
            ret_val = fat_readWrite(entry, 1,
                               entry->cache->data,false);
            if (ret_val < 0)
                return ret_val;
            entry->cache->cacheOffset = sectoroffset;
        }
        else
            entry->cache->cacheOffset = -1;
    }
    else
    {
        VFS_PRINT("[fat_fileSeek] good cache\n");
        if ( entry->cache->cacheOffset != -1 )
            entry->cache->cacheOffset = sectoroffset;
    }

    return ret_val;
}

MED_RET_T fat_fileFlushCache(struct vfs_node * opened_file)
{
    MED_RET_T ret_val;
    struct fat_entry * entry = (struct fat_entry*)opened_file->custom_data;
    int sector = opened_file->position / SECTOR_SIZE;

    VFS_PRINT("Flushing dirty sector cache\n");

    /* make sure we are on correct sector */
    ret_val = fat_seek(entry, sector);
    if(ret_val != MED_OK)
        return ret_val;

    ret_val = fat_readWrite(entry, 1,entry->cache->data, 1 );

    if ( ret_val < 1 )
    {
        printk("Error flushing cache : fat_readWrite returns: %d\n",-ret_val);
        if(entry->eof)
            return ret_val;
    }

    ret_val = vfs_nodeClearDirty(opened_file);
    if(ret_val != MED_OK)
        return ret_val;
    VFS_PRINT("Flush done\n");
    return MED_OK;
}

MED_RET_T fat_fileOpen(struct vfs_node * opened_file)
{
    /* searching for an available cache */
    struct fat_entry * entry=(struct fat_entry*)opened_file->custom_data;
    if(opened_file->type == VFS_TYPE_FILE)
    {
        int i=0;
        VFS_PRINT("Openning a file -> init cache system\n");
        for(i=0;i<MAX_OPEN;i++)
            if(!cache_list[i].used)
                break;
        if(i==MAX_OPEN)
            return -MED_ENOENT;
        entry->cache=&cache_list[i];
        entry->cache_num=i;
        cache_list[i].used=1;
        entry->cache->cacheOffset=-1;

        entry->firstcluster=entry->lastcluster=opened_file->storage_location ;
        entry->lastsector = entry->clusternum = entry->sectornum = entry->eof = 0;

    }
    else
        VFS_PRINT("Openning a dir -> nothing to do\n");
#warning should we reload entry for both dir and file?
    return MED_OK;
}

MED_RET_T fat_fileClose(struct vfs_node * opened_file)
{
    if(opened_file->type == VFS_TYPE_FILE)
    {
        int num=((struct fat_entry*)opened_file->custom_data)->cache_num;
        if(num>=0 && num<MAX_OPEN)
            cache_list[num].used=0;
    }
    return MED_OK;
}



MED_RET_T fat_createFile(const char* name,
                    struct vfs_node* file,
                    struct vfs_node* dir)
{
    MED_RET_T ret_val;

    /* need to create struct for file */
    struct fat_entry * new_file = (struct fat_entry *)malloc(sizeof(struct fat_entry));

    if(!new_file)
    {
        return -MED_ENOMEM;
    }

    memset(new_file,0x0,sizeof(struct fat_entry));
    file->custom_data=new_file;
    new_file->fat_bpb=((struct fat_entry*)dir->custom_data)->fat_bpb;
    ret_val = fat_addDirEntry(dir, file, name, false, false);

    return ret_val;
}

MED_RET_T fat_fileSync(struct vfs_node * opened_file)
{
    struct fat_entry * entry = (struct fat_entry *)opened_file->custom_data;
    return fat_closeWrite(opened_file,entry->size,entry->attr);
}

int fat_fileReadWrite(struct vfs_node * opened_file, void* buf, int count, bool write)
{
    int sectors;
    int nread=0;
    int ret_val;
    struct fat_entry* entry = (struct fat_entry*)opened_file->custom_data;

    if (!write && count > entry->size - opened_file->position)
    {
        count = entry->size - opened_file->position;
        VFS_PRINT("[FAT FILE_RW] Need to correct count: new=%d\n",count);
    }
    /* any head bytes? */
    if(entry->cache->cacheOffset != -1)
    {
        int headbytes;
        int offs = entry->cache->cacheOffset;
        if(count <= SECTOR_SIZE - entry->cache->cacheOffset)
        {
            headbytes = count;
            entry->cache->cacheOffset += count;
            if(entry->cache->cacheOffset >= SECTOR_SIZE)
                entry->cache->cacheOffset = -1;

        }
        else
        {
            headbytes = SECTOR_SIZE - entry->cache->cacheOffset;
            entry->cache->cacheOffset = -1;
        }

        if(write)
        {
            memcpy( entry->cache->data + offs, buf, headbytes );
            if(offs+headbytes == SECTOR_SIZE)
            {
                VFS_PRINT("flushing cache\n");
                int ret_val = fat_fileFlushCache(opened_file);
                if(ret_val != MED_OK)
                    return ret_val;
                entry->cache->cacheOffset = -1;
            }
            else
                vfs_nodeSetDirty(opened_file);
        }
        else
        {
            memcpy( buf, entry->cache->data + offs, headbytes );
        }

        nread = headbytes;
        count -= headbytes;
    }
    else
        VFS_PRINT("no cache\n");

    /* If the buffer has been modified, either it has been flushed already
     * (if (offs+headbytes == SECTOR_SIZE)...) or does not need to be (no
     * more data to follow in this call). Do NOT flush here. */

    /* read/write whole sectors right into/from the supplied buffer */
    sectors = count / SECTOR_SIZE;
    if(sectors)
    {
        ret_val = fat_readWrite(entry,sectors,(unsigned char*)buf+nread, write );
        if (ret_val < 0)
        {
            printk("Failed read/writing %d sectors\n",sectors);
            if(write && entry->eof)
                printk("No space left on device\n");
            else
                opened_file->position += nread;
            entry->cache->cacheOffset = -1;
            return nread ? nread : ret_val;
        }
        else
        {
            if ( ret_val > 0 )
            {
                nread += ret_val * SECTOR_SIZE;
                count -= sectors * SECTOR_SIZE;

                /* if eof, skip tail bytes */
                if ( ret_val < sectors )
                    count = 0;
            }
            else
            {
                /* eof */
                printk("\n\neof\n");
                count=0;
            }
            entry->cache->cacheOffset = -1;
        }
    }

    /* any tail bytes? */
    if ( count )
    {
        if (write)
        {
            if ( opened_file->position + nread < entry->size )
            {
                /* sector is only partially filled. copy-back from disk */
                ret_val = fat_readWrite(entry, 1, entry->cache->data, false );
                if ( ret_val < 1 )
                {
                    printk("Failed reading\n");
                    opened_file->position += nread;
                    entry->cache->cacheOffset = -1;
                    return nread ? nread : ret_val;
                }
                /* seek back one sector to put file position right */
                ret_val = fat_seek(entry,
                              (opened_file->position + nread) /
                              SECTOR_SIZE);
                if ( ret_val != MED_OK )
                {
                    printk("fat_seek() failed\n");
                    opened_file->position += nread;
                    entry->cache->cacheOffset = -1;
                    return nread ? nread : ret_val;
                }
            }
            memcpy( entry->cache->data, (unsigned char*)buf + nread, count );
            vfs_nodeSetDirty(opened_file);
        }
        else
        {
            ret_val = fat_readWrite(entry, 1, &(entry->cache->data),false);
            if (ret_val < 1 )
            {
                printk("Failed caching sector\n");
                opened_file->position += nread;
                entry->cache->cacheOffset = -1;
                return nread ? nread : ret_val;
            }
            memcpy( (unsigned char*)buf + nread, entry->cache->data, count );
        }

        nread += count;
        entry->cache->cacheOffset = count;
    }

    opened_file->position += nread;

    /* adjust file size to length written */
    if ( write && opened_file->position > entry->size )
        entry->size = opened_file->position;

    return nread;
}

int fat_fileWrite(struct vfs_node * opened_file, void* buf, unsigned int count)
{
    return fat_fileReadWrite(opened_file,buf,count,1);
}

int fat_fileRead(struct vfs_node * opened_file, void* buf, unsigned int count)
{
    return fat_fileReadWrite(opened_file,buf,count,0);
}

MED_RET_T fat_freeDirEntries(struct vfs_node * opened_file)
{
    unsigned char buf[SECTOR_SIZE];
    struct vfs_node * updir=opened_file->parent;
    struct fat_entry* fileEnt = (struct fat_entry*)opened_file->custom_data;
    struct fat_entry* dirEnt =  (struct fat_entry*)updir->custom_data;
    int numentries = fileEnt->nbDirEntries;
    unsigned int entry = fileEnt->dirEntryNum - numentries + 1;
    unsigned int sector = entry / DIR_ENTRIES_PER_SECTOR;
    int i;
    MED_RET_T ret_val;

    VFS_PRINT("[free_direntries] start\n");

    /* create a temporary file handle for the dir holding this file */
    ret_val = fat_seek( dirEnt, sector );
    if (ret_val != MED_OK)
        return ret_val;

    ret_val = fat_readWrite(dirEnt, 1, buf, 0);
    if (ret_val < 1)
        return ret_val * 10 - 3;

    for (i=0; i < numentries; i++) {
        VFS_PRINT("Clearing dir entry %d (%d/%d)\n",
                entry, i+1, numentries);
        buf[(entry % DIR_ENTRIES_PER_SECTOR) * DIR_ENTRY_SIZE] = 0xe5;
        entry++;

        if ( (entry % DIR_ENTRIES_PER_SECTOR) == 0 ) {
            /* flush this sector */
            ret_val = fat_seek( dirEnt, sector );
            if (ret_val != MED_OK)
                return ret_val;

            ret_val = fat_readWrite(dirEnt, 1, buf, 1);
            if (ret_val < 1)
                return ret_val * 10 - 5;

            if ( i+1 < numentries ) {
                /* read next sector */
                ret_val = fat_readWrite(dirEnt, 1, buf, 0);
                if (ret_val < 1)
                    return ret_val * 10 - 6;
            }
            sector++;
        }
    }

    if ( entry % DIR_ENTRIES_PER_SECTOR ) {
        /* flush this sector */
        ret_val = fat_seek(dirEnt, sector );
        if (ret_val != MED_OK)
            return ret_val;

        ret_val = fat_readWrite(dirEnt, 1, buf, true);
        if (ret_val < 1)
            return ret_val * 10 - 8;
    }

    return MED_OK;
}

MED_RET_T fat_fileRemove(struct vfs_node * opened_file)
{
    struct fat_entry* fileEnt = (struct fat_entry*)opened_file->custom_data;
    int next, last = fileEnt->firstcluster;
    MED_RET_T ret_val;
    struct bpb* fat_bpb = fileEnt->fat_bpb;

    VFS_PRINT("[fat_remove] (%x)\n",last);

    while ( last ) {
        next = fat_getNextCluster(fat_bpb, last);
        fat_updateFatEntry(fat_bpb, last,0);
        last = next;
    }
#warning if we add mount we need a different check of root
#warning should check all access to parent !!
    if ( opened_file->parent ) {
        ret_val = fat_freeDirEntries(opened_file);
        if (ret_val!=MED_OK)
            return ret_val;
    }

    fileEnt->firstcluster = 0;

    ret_val = fat_flushFat(fat_bpb);
    if (ret_val!=MED_OK)
        return ret_val;

    return MED_OK;
}

int fat_attribute(struct vfs_node * opened_file)
{
    struct fat_entry* fileEnt = (struct fat_entry*)opened_file->custom_data;
    return fileEnt->attr;
}


#if 0



int fat_create_dir(const char* name,
                   struct fat_dir* newdir,
                   struct fat_dir* dir)
{
    struct bpb* fat_bpb = &fat_bpbs[dir->file.volume];
    unsigned char buf[SECTOR_SIZE];
    int i;
    int sector;
    int rc;
    struct fat_file dummyfile;

    //printk("fat_create_dir(\"%s\",%x,%x)\n",name,newdir,dir);

    memset(newdir, 0, sizeof(struct fat_dir));
    memset(&dummyfile, 0, sizeof(struct fat_file));

    /* First, add the entry in the parent directory */
    rc = add_dir_entry(dir, &newdir->file, name, true, false);
    if (rc < 0)
        return rc * 10 - 1;

    /* Allocate a new cluster for the directory */
    newdir->file.firstcluster = find_free_cluster(fat_bpb, fat_bpb->fsinfo.nextfree);
    if(newdir->file.firstcluster == 0)
        return -1;

    update_fat_entry(fat_bpb, newdir->file.firstcluster, FAT_EOF_MARK);

    /* Clear the entire cluster */
    memset(buf, 0, sizeof buf);
    sector = cluster2sec(fat_bpb, newdir->file.firstcluster);
    for(i = 0;i < (int)fat_bpb->bpb_secperclus;i++) {
        rc = transfer(fat_bpb, sector + i, 1, buf, true );
        if (rc < 0)
            return rc * 10 - 2;
    }

    /* Then add the "." entry */
    rc = add_dir_entry(newdir, &dummyfile, ".", true, true);
    if (rc < 0)
        return rc * 10 - 3;
    dummyfile.firstcluster = newdir->file.firstcluster;
    update_short_entry(&dummyfile, 0, FAT_ATTR_DIRECTORY);

    /* and the ".." entry */
    rc = add_dir_entry(newdir, &dummyfile, "..", true, true);
    if (rc < 0)
        return rc * 10 - 4;

    /* The root cluster is cluster 0 in the ".." entry */
    if(dir->file.firstcluster == fat_bpb->bpb_rootclus)
        dummyfile.firstcluster = 0;
    else
        dummyfile.firstcluster = dir->file.firstcluster;
    update_short_entry(&dummyfile, 0, FAT_ATTR_DIRECTORY);

    /* Set the firstcluster field in the direntry */
    update_short_entry(&newdir->file, 0, FAT_ATTR_DIRECTORY);

    rc = flush_fat(fat_bpb);
    if (rc < 0)
        return rc * 10 - 5;

    return rc;
}


int fat_rename(struct fat_file* file,
                struct fat_dir* dir,
                const unsigned char* newname,
                int size,
                int attr)
{
    int rc;
    struct fat_dir olddir;
    struct fat_file newfile = *file;
    struct bpb* fat_bpb = &fat_bpbs[file->volume];

    if (file->volume != dir->file.volume) {
        printk("No rename across volumes!\n");
        return -1;
    }


    if ( !file->dircluster ) {
        printk("File has no dir cluster!\n");
        return -2;
    }

    /* create a temporary file handle */
    rc = fat_opendir(file->volume, &olddir, file->dircluster, NULL);
    if (rc < 0)
        return rc * 10 - 3;

    /* create new name */
    rc = add_dir_entry(dir, &newfile, newname, false, false);
    if (rc < 0)
        return rc * 10 - 4;

    /* write size and cluster link */
    rc = update_short_entry(&newfile, size, attr);
    if (rc < 0)
        return rc * 10 - 5;

    /* remove old name */
    rc = free_direntries(file);
    if (rc < 0)
        return rc * 10 - 6;

    rc = flush_fat(fat_bpb);
    if (rc < 0)
        return rc * 10 - 7;

    return 0;
}



#endif
