/*
*   include/dir.h
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __DIR_H_
#define __DIR_H_

#include <types.h>
#include <file.h>

#define ATTR_READ_ONLY   0x01
#define ATTR_HIDDEN      0x02
#define ATTR_SYSTEM      0x04
#define ATTR_VOLUME_ID   0x08
#define ATTR_DIRECTORY   0x10
#define ATTR_ARCHIVE     0x20
#define ATTR_VOLUME      0x40 /* this is a volume, not a real directory */

struct dirent {
    unsigned char d_name[MAX_PATH];
    int attribute;
    int size;
    int startcluster;
    unsigned short wrtdate; /*  Last write date */ 
    unsigned short wrttime; /*  Last write time */
};

#include <fat.h>

typedef struct {
    bool busy;
    int startcluster;
    struct fat_dir fatdir;
    struct fat_dir parent_dir;
    struct dirent theent;
    int volumecounter; /* running counter for faked volume entries */
} DIR;

void init_dir(void);

#endif
