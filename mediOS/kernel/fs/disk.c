/*
*   kernel/fs/disk.c
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

#include <sys_def/stddef.h>
#include <sys_def/string.h>

#include <kernel/ata.h>
#include <kernel/malloc.h>
#include <kernel/irq.h>
#include <kernel/kernel.h>
#include <kernel/disk.h>
#include <kernel/timer.h>
#include <kernel/fat.h>
#include <kernel/kernel.h>
#include <kernel/bat_power.h>
#include <kernel/vfs.h>


/* Partition table entry layout:
   -----------------------
   0: 0x80 - active
   1: starting head
   2: starting sector
   3: starting cylinder
   4: partition type
   5: end head
   6: end sector
   7: end cylinder
   8-11: starting sector (LBA)
   12-15: nr of sectors in partition
*/

#define BYTES2INT32(array,pos) \
          (array[pos] | (array[pos+1] << 8 ) | \
          (array[pos+2] << 16 ) | (array[pos+3] << 24 ))

static struct partition_info part[8]; /* space for 4 partitions on 2 drives */

static struct hd_info_s disk_info[2];

char * fatStr[]={"zero", "FAT12", "FAT16<32MB", "ExtMSDOS", "FAT16>32MB", "FAT32<2048GB", "FAT32-LBA",
                    "FAT16>32MB-LBA", "ExtMSDOS-LBA"};
int fatId[]={0x00, 0x01, 0x04, 0x05, 0x06, 0x0B, 0x0C, 0x0E, 0x0F};

extern int hd_sleep_state;

void disk_init(void)
{
    ata_init();
    vfs_init();
    fat_init(); /* reset all mounted partitions */

    if(disk_mount(CF_DRIVE)!=MED_OK)
        printk("Error doing disk init\n");

    printk("[init disk] done\n");
}

MED_RET_T disk_mount(int drive)
{
    struct partition_info* pinfo;

    pinfo = disk_setup(drive);
    if (pinfo == NULL)
    {
        printk("Error in disk setup\n");
        return -MED_EIO;
    }

    vfs_mount(drive,pinfo[0].start);
    return MED_OK;
}

MED_RET_T disk_umount(int drive,bool flush)
{
    if(vfs_hasOpenNode())
    {
        printk("Can't umount => there is opened files\n");
        return -MED_ENBUSY;
    }
    vfs_Destructor();
    return MED_OK;
}


struct partition_info * disk_setup(int drive)
{
    int i,j;
    unsigned char * sector=(unsigned char *)malloc(sizeof(unsigned char)*SECTOR_SIZE);
    /* For each drive, start at a different position, in order not to destroy
       the first entry of drive 0.
       That one is needed to calculate config sector position. */
    struct partition_info* pinfo = &part[drive*4];
    if ((int)drive >= sizeof(part)/sizeof(*part)/4)
        return NULL; /* out of space in table */

    if(!sector)
        return NULL;
    /* identify disk */
    disk_identify(drive,&disk_info[drive]);
    printk("[init IDE-CF] reading drive %d\n     %s\n     %s|%s\n     %d sectors per ata request\n",
                drive,disk_info[drive].model,
                disk_info[drive].firmware,disk_info[drive].serial,disk_info[drive].multi_sector);

    /* Read MBR */
    if(ata_rwData(drive,0,sector,1,ATA_DO_READ,ATA_WITH_DMA)<0) /* read 1 sector at LBA 0 */
        return NULL;

    /* check that the boot sector is initialized */
    if ( (sector[510] != 0x55) ||
         (sector[511] != 0xaa)) {
        printk("Bad boot sector signature\n");
        return NULL;
    }

    /* parse partitions */
    for ( i=0; i<4; i++ ) {
        unsigned char* ptr = sector + 0x1be + 16*i;
        pinfo[i].type  = ptr[4];
        pinfo[i].start = BYTES2INT32(ptr, 8);
        pinfo[i].size  = BYTES2INT32(ptr, 12);

        j=0;
        while(j<9 && fatId[j]!=pinfo[i].type) j++;

        if(j<9)
                strcpy(pinfo[i].strType,fatStr[j]);
        else
                printk("Error: partition type not supported: %x\n",pinfo[i].type);

        printk("Part%d: start=%08x, size=%08x, type:%s (%02x)\n",i,
                pinfo[i].start,pinfo[i].size,
                pinfo[i].strType,pinfo[i].type);

        /* extended? */
        if ( pinfo[i].type == 5 ) {
            /* not handled yet */
        }
    }
    free(sector);
    return pinfo;
}

void disk_identify(int drive, struct hd_info_s * hd_info)
{
    unsigned char * buffer=(unsigned char *)malloc(sizeof(unsigned char)*SECTOR_SIZE);

    if(buffer && !ata_rwData(drive,0,buffer,1,ATA_DO_IDENT,ATA_WITH_DMA))
    {
        strncpy(hd_info->serial, &buffer[20], 20);
        dd_swapChar(hd_info->serial,20);
        dd_findEnd(hd_info->serial,20);
        strncpy(hd_info->firmware, &buffer[46], 8);
        dd_swapChar(hd_info->firmware,8);
        dd_findEnd(hd_info->firmware,8);
        strncpy(hd_info->model, &buffer[54], 40);
        dd_swapChar(hd_info->model,40);
        dd_findEnd(hd_info->model,40);
        hd_info->multi_sector = buffer[47] & 0xff ;
    }
    else
    {
        hd_info->serial[0]='\0';
        hd_info->firmware[0]='\0';
        hd_info->model[0]='\0';
        hd_info->size=0;
    }
    free(buffer);
}

/* !!!!! ata_stopHD should use the correct mode => ATA_FORCE_STOP or ATA_DELAY_STOP
void disk_haltHD(void)
{
    disk_umount(HD_DRIVE,FLUSH);
    hd_sleep_state=1;
    ata_stopHD();
}
*/

void disk_printPartInfo(struct partition_info * partition_list)
{
    int i;
    for(i=0;i<4;i++)
    printk("Partition %d: start=%x, size=%x, type:%s (%x)\n",i,
                partition_list[i].start,partition_list[i].size,
                partition_list[i].strType,partition_list[i].type);
}

void dd_swapChar(char * txt,int size)
{
    int i;
    for(i=0;i<size/2;i++)
    {
            char c=txt[2*i];
            txt[2*i]=txt[2*i+1];
            txt[2*i+1]=c;
    }
}

void dd_findEnd(char * txt,int size)
{
    int i,j;
    /* find first real char */
    for(i=0;i<size;i++)
        if(txt[i]!=' ')
            break;
    /* remove head space */
    for(j=i;j<size;j++)
    {
        if(txt[j]==' ')
        {
            txt[j-i]=0;
            break;
         }
         txt[j-i]=txt[j];
    }
    if(txt[j-i]!=0)
        txt[j-i]=0;
}
