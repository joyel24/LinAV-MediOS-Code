/*
*   kernel/fs/fat_lowLevel.c
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

#include <kernel/fat.h>
#include <kernel/ata.h>

#include <kernel/kernel.h>
#include <kernel/errors.h>

#include <kernel/vfs.h>

#include "fat_defines.h"

char fat_cache_sectors[FAT_CACHE_SIZE][SECTOR_SIZE];
struct fat_cache_entry fat_cache[FAT_CACHE_SIZE];
extern struct file_cache cache_list[MAX_OPEN];

unsigned int fat_cluster2Sec(struct bpb* fat_bpb, int cluster)
{
    /* negative clusters (FAT16 root dir) don't get the 2 offset */
    int zerocluster = cluster < 0 ? 0 : 2;

    int max_cluster = fat_bpb->totalsectors -
        fat_bpb->firstdatasector / fat_bpb->bpb_secperclus + 1;

    if (cluster > max_cluster)
    {
        printk( "cluster2sec() - Bad cluster number (%d)\n", cluster);
        return -1;
    }

    return (cluster - zerocluster) * fat_bpb->bpb_secperclus
           + fat_bpb->firstdatasector;
}

MED_RET_T fat_initDevice(struct bpb* fat_bpb,int drive,unsigned int startsector)
{
    unsigned char buf[SECTOR_SIZE];
    MED_RET_T ret_val;
    int datasec;
    int rootdirsectors;

    /* Read the sector */
    ret_val = ata_rwData(drive,startsector,buf,1,ATA_DO_READ,ATA_WITH_DMA);
    if(ret_val)
    {
        printk( "fat_mount() - Couldn't read BPB (error code %d)\n", ret_val);
        return -MED_EIO;
    }

    memset(fat_bpb, 0, sizeof(struct bpb));
    fat_bpb->startsector    = startsector;
    fat_bpb->drive          = drive;

    fat_bpb->bpb_bytspersec = BYTES2INT16(buf,BPB_BYTSPERSEC);
    fat_bpb->bpb_secperclus = buf[BPB_SECPERCLUS];
    fat_bpb->bpb_rsvdseccnt = BYTES2INT16(buf,BPB_RSVDSECCNT);
    fat_bpb->bpb_numfats    = buf[BPB_NUMFATS];
    fat_bpb->bpb_totsec16   = BYTES2INT16(buf,BPB_TOTSEC16);
    fat_bpb->bpb_media      = buf[BPB_MEDIA];
    fat_bpb->bpb_fatsz16    = BYTES2INT16(buf,BPB_FATSZ16);
    fat_bpb->bpb_fatsz32    = BYTES2INT32(buf,BPB_FATSZ32);
    fat_bpb->bpb_totsec32   = BYTES2INT32(buf,BPB_TOTSEC32);
    fat_bpb->last_word      = BYTES2INT16(buf,BPB_LAST_WORD);

    /* calculate a few commonly used values */
    if (fat_bpb->bpb_fatsz16 != 0)
        fat_bpb->fatsize = fat_bpb->bpb_fatsz16;
    else
        fat_bpb->fatsize = fat_bpb->bpb_fatsz32;

    if (fat_bpb->bpb_totsec16 != 0)
        fat_bpb->totalsectors = fat_bpb->bpb_totsec16;
    else
        fat_bpb->totalsectors = fat_bpb->bpb_totsec32;

    fat_bpb->bpb_rootentcnt = BYTES2INT16(buf,BPB_ROOTENTCNT);
    rootdirsectors = ((fat_bpb->bpb_rootentcnt * 32)
        + (fat_bpb->bpb_bytspersec - 1)) / fat_bpb->bpb_bytspersec;

    fat_bpb->firstdatasector = fat_bpb->bpb_rsvdseccnt
        + rootdirsectors
        + fat_bpb->bpb_numfats * fat_bpb->fatsize;

    /* Determine FAT type */
    datasec = fat_bpb->totalsectors - fat_bpb->firstdatasector;
    fat_bpb->dataclusters = datasec / fat_bpb->bpb_secperclus;

    if ( fat_bpb->dataclusters < 65525 )
    { /* FAT16 */
        fat_bpb->is_fat16 = true;
        if (fat_bpb->dataclusters < 4085)
        { /* FAT12 */
            printk("This is FAT12. Go away!\n");
            return -MED_EINVAL;
        }
    }

    if (fat_bpb->is_fat16)
    { /* FAT16 specific part of BPB */
        int dirclusters;
        fat_bpb->rootdirsector = fat_bpb->bpb_rsvdseccnt
            + fat_bpb->bpb_numfats * fat_bpb->bpb_fatsz16;
        dirclusters = ((rootdirsectors + fat_bpb->bpb_secperclus - 1)
            / fat_bpb->bpb_secperclus); /* rounded up, to full clusters */
        /* I assign negative pseudo cluster numbers for the root directory,
           their range is counted upward until -1. */
        fat_bpb->bpb_rootclus = 0 - dirclusters; /* backwards, before the data */
        fat_bpb->rootdiroffset = dirclusters * fat_bpb->bpb_secperclus
            - rootdirsectors;
    }
    else
    { /* FAT32 specific part of BPB */
        fat_bpb->bpb_rootclus  = BYTES2INT32(buf,BPB_ROOTCLUS);
        fat_bpb->bpb_fsinfo    = BYTES2INT16(buf,BPB_FSINFO);
        fat_bpb->rootdirsector = fat_cluster2Sec(fat_bpb, fat_bpb->bpb_rootclus);
    }

    ret_val = fat_bpbIsSane(fat_bpb);
    if (ret_val < 0)
    {
        printk( "fat_mount() - BPB is not sane\n");
        return -MED_EBADDATA;
    }

    if (fat_bpb->is_fat16)
    {
        fat_bpb->fsinfo.freecount = 0xffffffff; /* force recalc below */
        fat_bpb->fsinfo.nextfree = 0xffffffff;
    }
    else
    {
        /* Read the fsinfo sector */
        ret_val = ata_rwData(drive,startsector + fat_bpb->bpb_fsinfo,buf,1,ATA_DO_READ,ATA_WITH_DMA);
        if (ret_val < 0)
        {
            printk( "fat_mount() - Couldn't read FSInfo (error code %d)\n", ret_val);
            return -MED_EIO;
        }
        fat_bpb->fsinfo.freecount = BYTES2INT32(buf, FSINFO_FREECOUNT);
        fat_bpb->fsinfo.nextfree = BYTES2INT32(buf, FSINFO_NEXTFREE);
    }

    /* calculate freecount if unset */
    if ( fat_bpb->fsinfo.freecount == 0xffffffff )
    {
        fat_recalcFree(fat_bpb);
    }

    printk("Freecount: %d\n",fat_bpb->fsinfo.freecount);
    printk("Nextfree: 0x%x\n",fat_bpb->fsinfo.nextfree);
    printk("Cluster count: 0x%x\n",fat_bpb->dataclusters);
    printk("Sectors per cluster: %d\n",fat_bpb->bpb_secperclus);
    printk("FAT sectors: 0x%x\n",fat_bpb->fatsize);

    return MED_OK;
}

void fat_recalcFree(struct bpb* fat_bpb)
{
    int free = 0;
    unsigned i;
    if (fat_bpb->is_fat16)
    {
        for (i = 0; i<fat_bpb->fatsize; i++) {
            unsigned int j;
            unsigned short* fat = fat_cacheFatSector(fat_bpb, i, 0);
            for (j = 0; j < CLUSTERS_PER_FAT16_SECTOR; j++) {
                unsigned int c = i * CLUSTERS_PER_FAT16_SECTOR + j;
                if ( c > fat_bpb->dataclusters+1 ) /* nr 0 is unused */
                    break;

                if (SWAB16(fat[j]) == 0x0000) {
                    free++;
                    if ( fat_bpb->fsinfo.nextfree == 0xffffffff )
                        fat_bpb->fsinfo.nextfree = c;
                }
            }
        }
    }
    else
    {
        for (i = 0; i<fat_bpb->fatsize; i++) {
            unsigned int j;
            unsigned int* fat = fat_cacheFatSector(fat_bpb, i, 0);
            for (j = 0; j < CLUSTERS_PER_FAT_SECTOR; j++) {
                unsigned int c = i * CLUSTERS_PER_FAT_SECTOR + j;
                if ( c > fat_bpb->dataclusters+1 ) /* nr 0 is unused */
                    break;

                if (!(SWAB32(fat[j]) & 0x0fffffff)) {
                    free++;
                    if ( fat_bpb->fsinfo.nextfree == 0xffffffff )
                        fat_bpb->fsinfo.nextfree = c;
                }
            }
        }
    }
    fat_bpb->fsinfo.freecount = free;
    fat_updateFsInfo(fat_bpb);
}

MED_RET_T fat_bpbIsSane(struct bpb* fat_bpb)
{
    if(fat_bpb->bpb_bytspersec != 512)
    {
        printk( "bpb_is_sane() - Error: sector size is not 512 (%d)\n",
                fat_bpb->bpb_bytspersec);
        return -MED_EBADDATA;
    }
    if(fat_bpb->bpb_secperclus * fat_bpb->bpb_bytspersec > 128*1024)
    {
        printk( "bpb_is_sane() - Error: cluster size is larger than 128K "
                "(%d * %d = %d)\n",
                fat_bpb->bpb_bytspersec, fat_bpb->bpb_secperclus,
                fat_bpb->bpb_bytspersec * fat_bpb->bpb_secperclus);
        return -MED_EBADDATA;
    }
    if(fat_bpb->bpb_numfats != 2)
    {
        printk( "bpb_is_sane() - Warning: NumFATS is not 2 (%d)\n",
                fat_bpb->bpb_numfats);
    }
    if(fat_bpb->bpb_media != 0xf0 && fat_bpb->bpb_media < 0xf8)
    {
        printk( "bpb_is_sane() - Warning: Non-standard "
                "media type (0x%02x)\n",
                fat_bpb->bpb_media);
    }
    if(fat_bpb->last_word != 0xaa55)
    {
        printk( "bpb_is_sane() - Error: Last word is not "
                "0xaa55 (0x%04x)\n", fat_bpb->last_word);
        return -MED_EBADDATA;
    }

    if (fat_bpb->fsinfo.freecount >
        (fat_bpb->totalsectors - fat_bpb->firstdatasector)/
        fat_bpb->bpb_secperclus)
    {
        printk( "bpb_is_sane() - Error: FSInfo.Freecount > disk size "
                 "(0x%04x)\n", fat_bpb->fsinfo.freecount);
        return -MED_EBADDATA;
    }

    return MED_OK;
}

void fat_flushFatSector(struct fat_cache_entry *fce,unsigned char *sectorbuf)
{
    int rc;
    int secnum;

    /* With multivolume, use only the FAT info from the cached sector! */
    secnum = fce->secnum + fce->fat_vol->startsector;

    /* Write to the first FAT */
    rc = ata_rwData(fce->fat_vol->drive,
            secnum,sectorbuf,1,ATA_DO_WRITE,ATA_WITH_DMA);

    if(rc < 0)
    {
        printk("flush_fat_sector() - Could not write sector %d"
               " (error %d)\n",
               secnum, rc);
    }
    if(fce->fat_vol->bpb_numfats > 1)
    {
        /* Write to the second FAT */
        secnum += fce->fat_vol->fatsize;
        rc = ata_rwData(fce->fat_vol->drive,
            secnum,sectorbuf,1,ATA_DO_WRITE,ATA_WITH_DMA);
        if(rc < 0)
        {
            printk("flush_fat_sector() - Could not write sector %d"
                   " (error %d)\n",
                   secnum, rc);
        }
    }
    fce->dirty = false;
}

void * fat_cacheFatSector(struct bpb* fat_bpb,int fatsector, int dirty)
{
    int secnum = fatsector + fat_bpb->bpb_rsvdseccnt;
    int cache_index = secnum & FAT_CACHE_MASK;
    struct fat_cache_entry *fce = &fat_cache[cache_index];
    unsigned char *sectorbuf = &fat_cache_sectors[cache_index][0];
    int rc;

    /* Delete the cache entry if it isn't the sector we want */
    if(fce->inuse && (fce->secnum != secnum || fce->fat_vol != fat_bpb ))
    {
        /* Write back if it is dirty */
        if(fce->dirty)
        {
            fat_flushFatSector(fce, sectorbuf);
        }
        fce->inuse = 0;
    }

    /* Load the sector if it is not cached */
    if(!fce->inuse)
    {
        rc = ata_rwData(fat_bpb->drive,
            secnum + fat_bpb->startsector,sectorbuf,1,ATA_DO_READ,ATA_WITH_DMA);

        if(rc < 0)
        {
            printk( "cache_fat_sector() - Could not read sector %d"
                    " (error %d)\n", secnum, rc);
            return NULL;
        }
        fce->inuse = 1;
        fce->secnum = secnum;
        fce->fat_vol = fat_bpb;
    }
    if (dirty)
        fce->dirty = 1; /* dirt remains, sticky until flushed */
    return sectorbuf;
}

unsigned int fat_findFreeCluster(struct bpb* fat_bpb, unsigned int startcluster)
{
    unsigned int sector;
    unsigned int offset;
    unsigned int i;

    if (fat_bpb->is_fat16)
    {
        sector = startcluster / CLUSTERS_PER_FAT16_SECTOR;
        offset = startcluster % CLUSTERS_PER_FAT16_SECTOR;

        for (i = 0; i<fat_bpb->fatsize; i++) {
            unsigned int j;
            unsigned int nr = (i + sector) % fat_bpb->fatsize;
            unsigned short* fat = fat_cacheFatSector(fat_bpb, nr, false);
            if ( !fat )
                break;
            for (j = 0; j < CLUSTERS_PER_FAT16_SECTOR; j++) {
                int k = (j + offset) % CLUSTERS_PER_FAT16_SECTOR;
                if (SWAB16(fat[k]) == 0x0000) {
                    unsigned int c = nr * CLUSTERS_PER_FAT16_SECTOR + k;
                     /* Ignore the reserved clusters 0 & 1, and also
                        cluster numbers out of bounds */
                    if ( c < 2 || c > fat_bpb->dataclusters+1 )
                        continue;
                    //printk("find_free_cluster(%x) == %x\n",startcluster,c);
                    fat_bpb->fsinfo.nextfree = c;
                    return c;
                }
            }
            offset = 0;
        }
    }
    else
    {
        sector = startcluster / CLUSTERS_PER_FAT_SECTOR;
        offset = startcluster % CLUSTERS_PER_FAT_SECTOR;

        for (i = 0; i<fat_bpb->fatsize; i++) {
            unsigned int j;
            unsigned int nr = (i + sector) % fat_bpb->fatsize;
            unsigned int* fat = fat_cacheFatSector(fat_bpb, nr, false);
            if ( !fat )
                break;
            for (j = 0; j < CLUSTERS_PER_FAT_SECTOR; j++) {
                int k = (j + offset) % CLUSTERS_PER_FAT_SECTOR;
                if (!(SWAB32(fat[k]) & 0x0fffffff)) {
                    unsigned int c = nr * CLUSTERS_PER_FAT_SECTOR + k;
                     /* Ignore the reserved clusters 0 & 1, and also
                        cluster numbers out of bounds */
                    if ( c < 2 || c > fat_bpb->dataclusters+1 )
                        continue;
                    //printk("find_free_cluster(%x) == %x\n",startcluster,c);
                    fat_bpb->fsinfo.nextfree = c;
                    return c;
                }
            }
            offset = 0;
        }
    }

    return 0; /* 0 is an illegal cluster number */
}

MED_RET_T fat_updateFsInfo(struct bpb* fat_bpb)
{
    unsigned char fsinfo[SECTOR_SIZE];
    unsigned int* intptr;
    MED_RET_T ret_val;

    if (fat_bpb->is_fat16)
        return MED_OK; /* FAT16 has no FsInfo */

    /* update fsinfo */
    ret_val = ata_rwData(fat_bpb->drive,fat_bpb->startsector + fat_bpb->bpb_fsinfo,
                fsinfo,1,ATA_DO_READ,ATA_WITH_DMA);

    if (ret_val < 0)
    {
        printk( "flush_fat() - Couldn't read FSInfo (error code %d)\n", ret_val);
        return ret_val;
    }
    intptr = (int*)&(fsinfo[FSINFO_FREECOUNT]);
    *intptr = SWAB32(fat_bpb->fsinfo.freecount);

    intptr = (int*)&(fsinfo[FSINFO_NEXTFREE]);
    *intptr = SWAB32(fat_bpb->fsinfo.nextfree);

    ret_val = ata_rwData(fat_bpb->drive,fat_bpb->startsector + fat_bpb->bpb_fsinfo,
                fsinfo,1,ATA_DO_WRITE,ATA_WITH_DMA);
    if (ret_val < 0)
    {
        printk( "flush_fat() - Couldn't write FSInfo (error code %d)\n", ret_val);
        return ret_val;
    }

    return MED_OK;
}

int fat_updateFatEntry(struct bpb* fat_bpb, unsigned int entry, unsigned int val)
{
    if (fat_bpb->is_fat16)
    {
        int sector = entry / CLUSTERS_PER_FAT16_SECTOR;
        int offset = entry % CLUSTERS_PER_FAT16_SECTOR;
        unsigned short* sec;

        val &= 0xFFFF;

        //printk("update_fat_entry(%x,%x)\n",entry,val);

        if (entry==val)
            printk("Creating FAT loop: %x,%x\n",entry,val);

        if ( entry < 2 )
            printk("Updating reserved FAT entry %d.\n",entry);

        sec = fat_cacheFatSector(fat_bpb, sector, true);
        if (!sec)
        {
            printk( "update_fat_entry() - Could not cache sector %d\n", sector);
            return -1;
        }

        if ( val ) {
            if (SWAB16(sec[offset]) == 0x0000 && fat_bpb->fsinfo.freecount > 0)
                fat_bpb->fsinfo.freecount--;
        }
        else {
            if (SWAB16(sec[offset]))
                fat_bpb->fsinfo.freecount++;
        }

        //printk("update_fat_entry: %d free clusters\n", fat_bpb->fsinfo.freecount);

        sec[offset] = SWAB16(val);
    }
    else
    {
        int sector = entry / CLUSTERS_PER_FAT_SECTOR;
        int offset = entry % CLUSTERS_PER_FAT_SECTOR;
        unsigned int* sec;

        //printk("update_fat_entry(%x,%x)\n",entry,val);

        if (entry==val)
            printk("Creating FAT loop: %x,%x\n",entry,val);

        if ( entry < 2 )
            printk("Updating reserved FAT entry %d.\n",entry);

        sec = fat_cacheFatSector(fat_bpb, sector, true);
        if (!sec)
        {
            printk( "update_fat_entry() - Could not cache sector %d\n", sector);
            return -1;
        }

        if ( val ) {
            if (!(SWAB32(sec[offset]) & 0x0fffffff) &&
                fat_bpb->fsinfo.freecount > 0)
                fat_bpb->fsinfo.freecount--;
        }
        else {
            if (SWAB32(sec[offset]) & 0x0fffffff)
                fat_bpb->fsinfo.freecount++;
        }

        //printk("update_fat_entry: %d free clusters\n", fat_bpb->fsinfo.freecount);

        /* don't change top 4 bits */
        sec[offset] &= SWAB32(0xf0000000);
        sec[offset] |= SWAB32(val & 0x0fffffff);
    }

    return 0;
}

int fat_readFatEntry(struct bpb* fat_bpb, unsigned int entry)
{
    if (fat_bpb->is_fat16)
    {
        int sector = entry / CLUSTERS_PER_FAT16_SECTOR;
        int offset = entry % CLUSTERS_PER_FAT16_SECTOR;
        unsigned short* sec;

        sec = fat_cacheFatSector(fat_bpb, sector, 0);
        if (!sec)
        {
            printk( "read_fat_entry() - Could not cache sector %d\n", sector);
            return -1;
        }

        return SWAB16(sec[offset]);
    }
    else
    {
        int sector = entry / CLUSTERS_PER_FAT_SECTOR;
        int offset = entry % CLUSTERS_PER_FAT_SECTOR;
        unsigned int* sec;

        sec = fat_cacheFatSector(fat_bpb, sector, 0);
        if (!sec)
        {
            printk( "read_fat_entry() - Could not cache sector %d\n", sector);
            return -1;
        }

        return SWAB32(sec[offset]) & 0x0fffffff;
    }
}

int fat_getNextCluster(struct bpb* fat_bpb, int cluster)
{
    int next_cluster;
    int eof_mark = FAT_EOF_MARK;

    if (fat_bpb->is_fat16)
    {
        eof_mark &= 0xFFFF; /* only 16 bit */
        if (cluster < 0) /* FAT16 root dir */
            return cluster + 1; /* don't use the FAT */
    }

    next_cluster = fat_readFatEntry(fat_bpb, cluster);

    /* is this last cluster in chain? */
    if ( next_cluster >= eof_mark )
        return 0;
    else
        return next_cluster;
}


MED_RET_T fat_flushFat(struct bpb* fat_bpb)
{
    int i;
    MED_RET_T ret_val;
    unsigned char *sec;

    for(i = 0;i < FAT_CACHE_SIZE;i++)
    {
        struct fat_cache_entry *fce = &fat_cache[i];
        if(fce->inuse
            && fce->fat_vol == fat_bpb
            && fce->dirty)
        {
            sec = fat_cache_sectors[i];
            fat_flushFatSector(fce, sec);
        }
    }

    ret_val = fat_updateFsInfo(fat_bpb);
    if (ret_val!=MED_OK)
        return ret_val;

    return MED_OK;
}

#warning work on rtc code

void fat_time(unsigned short* date,unsigned short* time,unsigned short* tenth )
{
#ifdef HAVE_RTC
    struct tm* tm = get_time();

    if (date)
        *date = ((tm->tm_year - 80) << 9) |
            ((tm->tm_mon + 1) << 5) |
            tm->tm_mday;

    if (time)
        *time = (tm->tm_hour << 11) |
            (tm->tm_min << 5) |
            (tm->tm_sec >> 1);

    if (tenth)
        *tenth = (tm->tm_sec & 1) * 100;
#else
    /* non-RTC version returns an increment from the supplied time, or a
     * fixed standard time/date if no time given as input */
    int next_day = false;

    if (time)
    {
        if (0 == *time)
        {
            /* set to 00:15:00 */
            *time = (15 << 5);
        }
        else
        {
            unsigned short mins = (*time >> 5) & 0x003F;
            unsigned short hours = (*time >> 11) & 0x001F;
            if ((mins += 10) >= 60)
            {
                mins = 0;
                hours++;
            }
            if ((++hours) >= 24)
            {
                hours = hours - 24;
                next_day = true;
            }
            *time = (hours << 11) | (mins << 5);
        }
    }

    if (date)
    {
        if (0 == *date)
        {
            /* set to 1 August 2003 */
            *date = ((2003 - 1980) << 9) | (8 << 5) | 1;
        }
        else
        {
            unsigned short day = *date & 0x001F;
            unsigned short month = (*date >> 5) & 0x000F;
            unsigned short year = (*date >> 9) & 0x007F;
            if (next_day)
            {
                /* do a very simple day increment - never go above 28 days */
                if (++day > 28)
                {
                    day = 1;
                    if (++month > 12)
                    {
                        month = 1;
                        year++;
                    }
                }
                *date = (year << 9) | (month << 5) | day;
            }
        }
    }
    if (tenth)
        *tenth = 0;
#endif /* HAVE_RTC */
}

unsigned char fat_char2Dos(unsigned char c)
{
    switch(c)
    {
        case 0xe5: /* Special kanji character */
            c = 0x05;
            break;
        case 0x20:
        case 0x22:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2e:
        case 0x3a:
        case 0x3b:
        case 0x3c:
        case 0x3d:
        case 0x3e:
        case 0x3f:
        case 0x5b:
        case 0x5c:
        case 0x5d:
        case 0x7c:
            /* Illegal name */
            c = 0;
            break;

        default:
            if(c < 0x20)
            {
                /* Illegal name */
                c = 0;
            }
            break;
    }
    return c;
}

void fat_createDosName(const unsigned char *name, unsigned char *newname)
{
    int i,j;

    /* Name part */
    for (i=0, j=0; name[i] && (j < 8); i++)
    {
        unsigned char c = fat_char2Dos(name[i]);
        if (c)
            newname[j++] = toupper(c);
    }
    while (j < 8)
        newname[j++] = ' ';

    /* Extension part */
#warning need rand here
    snprintf(newname+8, 4, "%03X", 0);//(unsigned)rand() & 0xfff);
}

void fat_parseDirEntry(struct fat_direntry *de, const unsigned char *buf)
{
    int i=0,j=0;
    memset(de, 0, sizeof(struct fat_direntry));
    de->attr = buf[FATDIR_ATTR];
    de->crttimetenth = buf[FATDIR_CRTTIMETENTH];
    de->crtdate = BYTES2INT16(buf,FATDIR_CRTDATE);
    de->crttime = BYTES2INT16(buf,FATDIR_CRTTIME);
    de->wrtdate = BYTES2INT16(buf,FATDIR_WRTDATE);
    de->wrttime = BYTES2INT16(buf,FATDIR_WRTTIME);
    de->filesize = BYTES2INT32(buf,FATDIR_FILESIZE);
    de->firstcluster = BYTES2INT16(buf,FATDIR_FSTCLUSLO) |
                      (BYTES2INT16(buf,FATDIR_FSTCLUSHI) << 16);

    /* fix the name */
    for (i=0; (i<8) && (buf[FATDIR_NAME+i] != ' '); i++)
        de->name[j++] = buf[FATDIR_NAME+i];
    if ( buf[FATDIR_NAME+8] != ' ' ) {
        de->name[j++] = '.';
        for (i=8; (i<11) && (buf[FATDIR_NAME+i] != ' '); i++)
            de->name[j++] = buf[FATDIR_NAME+i];
    }
}

int fat_nextWriteCluster(struct fat_entry * entry,int oldcluster,int* newsector)
{
    struct bpb* fat_bpb = entry->fat_bpb;
    int cluster = 0;
    int sector;

    if (oldcluster)
        cluster = fat_getNextCluster(fat_bpb, oldcluster);

    if (!cluster) {
        VFS_PRINT("No cluster found\n");
        if (oldcluster > 0)
        {
            VFS_PRINT("starting from oldcluster:%x\n",oldcluster);
            cluster = fat_findFreeCluster(fat_bpb, oldcluster+1);
            VFS_PRINT("get=%x\n",cluster);
        }
        else if (oldcluster == 0)
        {
            VFS_PRINT("oldcluster=0, starting from %x\n",fat_bpb->fsinfo.nextfree);
            cluster = fat_findFreeCluster(fat_bpb, fat_bpb->fsinfo.nextfree);
            VFS_PRINT("get=%x\n",cluster);
        }
        else /* negative, pseudo-cluster of the root dir */
        {
            printk("Error in searching new write cluster old<0\n");
            return 0; /* impossible to append something to the root */
        }

        if (cluster) {
            if (oldcluster)
            {
                VFS_PRINT("Adding new cluster to chain\n");
                fat_updateFatEntry(fat_bpb, oldcluster, cluster);
            }
            else
            {
                VFS_PRINT("We have a new file !!\n");
                entry->firstcluster = cluster;
            }

            fat_updateFatEntry(fat_bpb, cluster, FAT_EOF_MARK);
        }
        else {
            printk("next_write_cluster(): Disk full!\n");
            return 0;
        }
    }
    sector = fat_cluster2Sec(fat_bpb, cluster);
    if (sector<0)
        return 0;

    *newsector = sector;
    return cluster;
}

int fat_transfer(struct bpb* fat_bpb, unsigned int start, int count, char* buf, int write )
{
    int rc;

    /*printk("transfer(s=%x, c=%x, %s)\n",
        start+ fat_bpb->startsector, count, write?"write":"read");*/
    if (write) {
        unsigned int firstallowed;
        if (fat_bpb->is_fat16)
            firstallowed = fat_bpb->rootdirsector;
        else
            firstallowed = fat_bpb->firstdatasector;

        if (start < firstallowed)
            printk("Write %d before data\n", firstallowed - start);
        if (start + count > fat_bpb->totalsectors)
            printk("Write %d after data\n",
                start + count - fat_bpb->totalsectors);
        rc = ata_rwData(fat_bpb->drive,start + fat_bpb->startsector,buf,count,ATA_DO_WRITE,ATA_WITH_DMA);
    }
    else
        rc = ata_rwData(fat_bpb->drive,start + fat_bpb->startsector,buf,count,ATA_DO_READ,ATA_WITH_DMA);
    if (rc < 0) {
        printk( "transfer() - Couldn't %s sector %x"
                " (error code %d)\n",
                write ? "write":"read", start, rc);
        return rc;
    }
    return MED_OK;
}

/* convert from unicode to a single-byte charset */
void fat_unicode2Iso(const unsigned char* unicode, unsigned char* iso,int count)
{
    int i;

    for (i=0; i<count; i++) {
        int x = i*2;
        switch (unicode[x+1]) {
            case 0x01: /* latin extended. convert to ISO 8859-2 */
            case 0x02:
                iso[i] = unicode2iso8859_2[unicode[x]];
                break;

            case 0x03: /* greek, convert to ISO 8859-7 */
                iso[i] = unicode[x] + 0x30;
                break;

                /* Sergei says most russians use Win1251, so we will too.
                   Win1251 differs from ISO 8859-5 by an offset of 0x10. */
            case 0x04: /* cyrillic, convert to Win1251 */
                switch (unicode[x]) {
                    case 1:
                        iso[i] = 168;
                        break;

                    case 81:
                        iso[i] = 184;
                        break;

                    default:
                        iso[i] = unicode[x] + 0xb0; /* 0xa0 for ISO 8859-5 */
                        break;
                }
                break;

            case 0x05: /* hebrew, convert to ISO 8859-8 */
                iso[i] = unicode[x] + 0x10;
                break;

            case 0x06: /* arabic, convert to ISO 8859-6 */
            case 0x0e: /* thai, convert to ISO 8859-11 */
                iso[i] = unicode[x] + 0xa0;
                break;

            default:
                iso[i] = unicode[x];
                break;
        }
    }
}

void fat_init(void)
{
    int i;

    /* mark the FAT cache as unused */
    for(i = 0;i < FAT_CACHE_SIZE;i++)
    {
        fat_cache[i].secnum = 8; /* We use a "safe" sector just in case */
        fat_cache[i].inuse = false;
        fat_cache[i].dirty = false;
        fat_cache[i].fat_vol = NULL;
    }

     memset(cache_list,0x0,sizeof(struct file_cache)*MAX_OPEN);
}


