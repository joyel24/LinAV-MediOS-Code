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

#ifndef _FAT_H
#define _FAT_H 1

#include <stdbool.h>

#define BUFFER_SIZE 512
#define SECTOR_SIZE 512

#ifndef u32_defined
#define u32_defined yes
typedef unsigned long u32;
#endif

#define FAT_CHAIN_END   0x0ffffff8

#define FAT_ENTRY_SIZE	32
#define NAME_SIZE 8
#define EXT_SIZE 3

#define FAT_ATTR_READ_ONLY  0x01
#define FAT_ATTR_HIDDEN     0x02
#define FAT_ATTR_SYSTEM     0x04
#define FAT_ATTR_VOLUME_ID  0x08
#define FAT_ATTR_DIR        0x10
#define FAT_ATTR_ARCHIVE    0x20
#define FAT_ATTR_LONG_NAME  FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN | FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID

struct fatent {
	unsigned char cache[BUFFER_SIZE];
    int cacheoffset;
    int fatoffset;
	int sectorNumber;
	int startCluster;
	int curCluster;
	int nxtCluster;
	bool eof_disk;
};

struct dirEntry {
    char name[NAME_SIZE];
    char ext[EXT_SIZE];
    char attr;
    char ntres;
    char timeTenth;
    short crtTime;
    short crtDate;
    short lstAccDate;
    unsigned short fatCluHI;
    short wrtTime;
    short wrtDate;
    unsigned short fatCluLO;
    int size;
};                              // SOFTWARE - 4B 30 90 2F [08 00] 93 1A 4B 30 [83 0E] <00 00 00 00>
                                // reading cluster 00080e81
#include "dirent.h"

extern int getRootClu();
extern int fatInit(u32 lba);
extern int fatReadCluster(int cluster, char* buffer);
extern int fatTrace(int cluster);
extern int fatReadFile(int cluster, char* buffer);
extern int fatDirFilter(struct dirEntry dirIn[], struct dirEntry dirOut[], int num);

extern int fatValidateEntry(struct dirEntry * entry);
extern void fatOpendir(struct fatent * fat_ent,int startCluster);
extern int fatNxtSector(struct fatent * fat_ent);

extern void fatGetName(char * name,struct dirEntry * entry);
extern void fatGetEntryName(char * entryName,struct dirEntry * entry);
extern void fatGetExt(char * ext,struct dirEntry * entry);
extern int fatGetAtr(struct dirEntry * entry);
extern int fatGetstrtClu(struct dirEntry * entry);
extern int fatGetSize(struct dirEntry * entry);

int fatloadFile(char * fileN);


#endif
