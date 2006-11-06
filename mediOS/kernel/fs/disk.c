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

#ifdef HAVE_EXT_MODULE
#include <kernel/cf_module.h>
#endif

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

struct hd_info_s * drive_info[NB_DRIVE];

char * fatStr[]={"zero", "FAT12", "FAT16<32MB", "ExtMSDOS", "FAT16>32MB", "FAT32<2048GB", "FAT32-LBA",
                    "FAT16>32MB-LBA", "ExtMSDOS-LBA"};
int fatId[]={0x00, 0x01, 0x04, 0x05, 0x06, 0x0B, 0x0C, 0x0E, 0x0F};

char * drive_name[] = { "Main HD", "CF card"};

extern int hd_sleep_state;

void dd_swapChar(char * txt,int size);
void dd_findEnd(char * txt,int size);

/* drive 0 will be mounted first */
struct disk_mountInfo drive_Info[] = {
{
    mount_path    : "/",
    drive         : HD_DRIVE,
    partition_num : DISK_PART_0
},
#ifdef HAVE_EXT_MODULE 
{
    mount_path    : "/CF",
    drive         : CF_DRIVE,
    partition_num : DISK_PART_0
}
#endif
};

MED_RET_T disk_rmAll(void)
{
#ifdef HAVE_EXT_MODULE    
        MED_RET_T ret_val;
    if(CF_IS_CONNECTED)
    {
        ret_val=disk_rm(CF_DRIVE);
        if(ret_val != MED_OK)
            return ret_val;
    }
#endif
    return disk_rm(HD_DRIVE);
}



MED_RET_T disk_rm(int drive)
{
    int i;
    int ret_val;
    if(drive_info[drive])
    {
        for(i=0;i<4;i++)
        {
            ret_val=vfs_umount(drive,i);
            if(ret_val!=MED_OK)
                return ret_val;
        }
        /* now we can remove disk info */
        free(&drive_info[drive]->partition_list);
        free(drive_info[drive]);
        drive_info[drive]=NULL;
    }
    else
    {
        printk("No info on %s\n",drive_name[drive]);
        return -MED_EINVAL;
    }
    return MED_OK;
}

MED_RET_T disk_add(int drive)
{
    drive_info[drive]=disk_setup(drive);
    if(!drive_info[drive])
        return -MED_ERROR; 
    return MED_OK;   
}

MED_RET_T disk_addAll(void)
{
    MED_RET_T ret_val;
    
    ret_val=disk_add(HD_DRIVE);
    if(ret_val != MED_OK)
    {
        printk("Error init main disk\n");
        return ret_val;
    }
    vfs_mount(MOUNT_DISK_PARAM(HD_DRIVE));
    
#ifdef HAVE_EXT_MODULE    
    if(CF_IS_CONNECTED)
    {
        disk_addCF();
    }
#endif
    
    return MED_OK;
}

void disk_init(void)
{
    int i;
    for(i=0;i<NB_DRIVE;i++) drive_info[i]=NULL;
    
    /* we should always have a HDD, let's add it */
    if(disk_add(HD_DRIVE)!=MED_OK)
    {
        printk("Error init main disk\n");
        return;
    }
    
#warning quick hack for partition less HDD
    if(!drive_info[HD_DRIVE]->partition_list[0].active)
    {
        printk("No active partition => load 0\n");
        drive_info[HD_DRIVE]->partition_list[0].active=1;
        drive_info[HD_DRIVE]->partition_list[0].start=0;
    } 
    
    /* mount root partition */
    vfs_mount(MOUNT_DISK_PARAM(HD_DRIVE));

    /* let's see if we have CF too */
#ifdef HAVE_EXT_MODULE    
    if(CF_IS_CONNECTED)
    {
        disk_addCF();
    }
#endif
        
    printk("[DISK] init done\n");
}


void disk_addCF(void)
{
    if(disk_add(CF_DRIVE)!=MED_OK)
        printk("Error init CF disk\n");   
    else
        vfs_mount(MOUNT_DISK_PARAM(CF_DRIVE));
}

struct hd_info_s * disk_setup(int drive)
{
    int i,j;
    unsigned char * sector=(unsigned char *)kmalloc(sizeof(unsigned char)*SECTOR_SIZE);
    
    if(!sector)
        return NULL;
    
    struct hd_info_s * disk_info = (struct hd_info_s *)kmalloc(sizeof(struct hd_info_s));
    if(!disk_info)
        goto exit_error1;
    /* let's assume we have only 4 partitions */
    struct partition_info * part_info = (struct partition_info *)kmalloc(4*sizeof(struct partition_info));
    if(!part_info)
        goto exit_error2;
    disk_info->partition_list=part_info;
    
    /* identify disk */
    if(ata_rwData(drive,0,sector,1,ATA_DO_IDENT,ATA_WITH_DMA)<0)
        goto main_exit;        
   
    strncpy(disk_info->serial, &sector[20], 20);
    dd_swapChar(disk_info->serial,20);
    dd_findEnd(disk_info->serial,20);
    strncpy(disk_info->firmware, &sector[46], 8);
    dd_swapChar(disk_info->firmware,8);
    dd_findEnd(disk_info->firmware,8);
    strncpy(disk_info->model, &sector[54], 40);
    dd_swapChar(disk_info->model,40);
    dd_findEnd(disk_info->model,40);
    disk_info->multi_sector = sector[47] & 0xff ;
    disk_info->partition_list=NULL;
    
    printk("[DISK] reading %s info\n     %s\n     %s|%s\n     %d sectors per ata request\n",
                drive_name[drive],
                disk_info->model,
                disk_info->firmware,disk_info->serial,disk_info->multi_sector);
                
    
    /* Read MBR */
    if(ata_rwData(drive,0,sector,1,ATA_DO_READ,ATA_WITH_DMA)<0) /* read 1 sector at LBA 0 */
        goto main_exit;

        
    /* check that the boot sector is initialized */
    if ( (sector[510] != 0x55) ||
         (sector[511] != 0xaa)) {
        printk("Bad boot sector signature\n");
        goto main_exit;
    }

    /* parse partitions */
    for ( i=0; i<4; i++ ) {
        unsigned char* ptr = sector + 0x1be + 16*i;
        part_info[i].type  = ptr[4];
        part_info[i].start = BYTES2INT32(ptr, 8);
        part_info[i].size  = BYTES2INT32(ptr, 12);
        part_info[i].active = 0;
        j=0;
        while(j<9 && fatId[j]!=part_info[i].type) j++;

        if(j<9)
        {
            part_info[i].active = 1;
            strcpy(part_info[i].strType,fatStr[j]);
            printk("\tPart%d: start=%08x, size=%08x, type:%s (%02x)\n",i,
                part_info[i].start,part_info[i].size,
                part_info[i].strType,part_info[i].type);
        }
        else        
                printk("Error: partition type not supported: %x\n",part_info[i].type);

        /* extended? */
        if ( part_info[i].type == 5 ) {
            /* not handled yet */
        }
    }
    
    /* adding part_info to disk structure */
    disk_info->partition_list=part_info;
    
    free(sector);
    return disk_info;
/* if something goes wrong exit here freeing what should be free*/    
main_exit:
    free(part_info);
exit_error2:    
    free(disk_info);
exit_error1:
    free(sector);
    return NULL;
}

char * disk_getName(int id)
{
    return drive_name[id];
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
