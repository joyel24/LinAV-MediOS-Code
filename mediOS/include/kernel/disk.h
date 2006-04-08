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

#define HD_DRIVE      0
#define CF_DRIVE      1

#define FLUSH         1
#define NOFLUSH       0

struct partition_info {
    int start;
    int size;
    int type;
    char strType[20];
};

struct hd_info_s {
	char serial[21];
	char firmware[9];
	char model [41];
        unsigned long size;
        int multi_sector;
};

#define NUM_VOLUMES  2 /* we can only mount 2 volumes */

void disk_init(void);

int disk_mount(int drive);
int disk_umount(int drive,bool flush);

struct partition_info * disk_setup(int drive);

void disk_identify(int drive, struct hd_info_s * hd_info);

void disk_haltHD(void);

void dd_swapChar(char * txt,int size);
void dd_findEnd(char * txt,int size);

#endif
