/* 
*   include/kernel/disk.h
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
*/


#ifndef __DISK_H
#define __DISK_H

#include <sys_def/types.h>
#include <kernel/errors.h>

#define HD_DRIVE      0
#define CF_DRIVE      1

#define DISK_PART_0   0
#define DISK_PART_1   1
#define DISK_PART_2   2
#define DISK_PART_3   3

#define FLUSH         1
#define NOFLUSH       0

#ifdef HAVE_EXT_MODULE
#define NB_DRIVE      2
#else
#define NB_DRIVE      1
#endif

struct partition_info {
    int start;
    int size;
    int type;
    char strType[20];
    int active;
};

struct hd_info_s {
	char serial[21];
	char firmware[9];
	char model [41];
        unsigned long size;
        int multi_sector;
        struct partition_info * partition_list;
};

struct disk_mountInfo {
    char * mount_path;
    int drive;
    int partition_num;
};

#define MOUNT_DISK_PARAM(INFO) drive_Info[INFO].mount_path,\
    drive_Info[INFO].drive, \
    drive_Info[INFO].partition_num

extern struct hd_info_s * drive_info[NB_DRIVE];

void disk_init(void);

MED_RET_T disk_add(int drive);
MED_RET_T disk_rm(int drive);
MED_RET_T disk_rmAll(void);
MED_RET_T disk_addAll(void);
/* simple system for no */
void disk_addCF(void);

char * disk_getName(int id);

void disk_reInit(void);

struct hd_info_s * disk_setup(int drive);

#endif
