/* libavos.h
   By William Bland aka awksedgrep
   Copyright 2004, the Avos project.

   This file is free software; we give unlimited permission to copy
   and/or distribute it, with or without modifications, as long as this
   notice is preserved.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY, to the extent permitted by law; without
   even the implied warranty of MERCHANTABILITY or FITNESS FOR A
   PARTICULAR PURPOSE.
*/

#ifndef u32_defined
#define u32_defined yes
typedef unsigned long u32;
#endif

#define FAT_CHAIN_END   0x0ffffff8

#define FAT_ATTR_READ_ONLY  0x01
#define FAT_ATTR_HIDDEN     0x02
#define FAT_ATTR_SYSTEM     0x04
#define FAT_ATTR_VOLUME_ID  0x08
#define FAT_ATTR_DIR        0x10
#define FAT_ATTR_ARCHIVE    0x20
#define FAT_ATTR_LONG_NAME  FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN | FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID

struct dirEntry {
    char name[8];
    char ext[3];
    char attr;
    char ntres;
    char timeTenth;
    short crtTime;
    short crtDate;
    short lstAccDate;
    short fatCluHI;
    short wrtTime;
    short wrtDate;
    short fatCluLO;
    int size;
};

extern int fatInit(u32 lba);
extern int fatReadCluster(int cluster, char* buffer);
extern int fatTrace(int cluster);
extern int fatReadFile(int cluster, char* buffer);
