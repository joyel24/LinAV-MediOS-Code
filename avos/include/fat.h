/* libavos.h
   By oxygen
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

//#define FAT_CHAIN_END   0x0ffffff8

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
	int sectorNumber;
	int startCluster;
	int curCluster;
	int prevCluster;
	bool eof_disk;
	int dirCluster;
	bool isRootDir;
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

struct fatCache {
	char fatBuffer[1024]; // 2* sector size  (for FAT12)
	int fatBufferLba;
	bool write_done;
	int lastFree;
};

#include "dirent.h"
#include "ata.h"

extern int fatInit(struct partInfo * partition);

extern int fatReadCluster(int cluster, char* buffer);
extern int fatTrace(int cluster);
extern int countFreeCluster();

extern int fatNxtEntry(struct fatent * fat_ent,struct dirEntry * entry);
extern int fatCreateEntry(struct fatent * fat_ent,struct dirent* ent,const char * name);
extern int fatRemoveEntry(struct fatent * fat_ent);
extern int fatValidateEntry(struct dirEntry * entry);
extern int fatUpdateEntry(struct dirent * ent);

extern void fatGetData(struct dirent *theent,struct dirEntry * entry);

extern void fatOpendir(struct fatent * fat_ent,int startCluster);

extern int getClusterSize();

extern int fatTruncate(struct fatent * fat_ent,bool total_remove);

//****************************************
//* internal function

extern int flushFatCache();
extern int updateFatCache(int cluster);
extern int writeFatCache(int cluster,int val);
extern int readFatCache(int cluster);

extern int fatRWSector(struct fatent * fat_ent,bool write);
extern int fatNxtSector(struct fatent * fat_ent,bool write);
extern int fatPrevSector(struct fatent * fat_ent);

extern int getNxtFreeCluster(int cluster);
extern int addNewCluster(int cluster);

extern int fatRWEntry(struct fatent * fat_ent,struct dirEntry * entry,bool write);
extern int fatPrevEntry(struct fatent * fat_ent);
extern int fatFindEntry(struct fatent * fat_ent,struct dirent * ent,struct dirEntry * entry);

extern void createDosName(char *name, char *fatName);
extern char chkChar(char c);

extern int fatCleanCluster(int cluster);

//extern int chkFAT();

extern bool isEOChain(int cluster);
extern int fatEndValue();

///////////////////////////////////////////////////////////////////////////////////
// from initial fat.c
extern int bootRead(int addr, int n);
extern int fatDirFilter(struct dirEntry dirIn[], struct dirEntry dirOut[], int n);
extern int fatReadFile(int cluster, char* buffer);
extern int getRootClu();

#endif
