// ata.h
#ifndef _FAT_H_
#define _FAT_H_

#include <stdbool.h>


// assembly functions
#ifndef _u32_
#define _u32_
typedef unsigned long u32;
#endif

struct fatInfo{
	u32 fatLBAFAT;
	u32 fatLBADATA;
	u32 fatRootClu;
	u32 fatSecPerClu;
	u32 fatClusterSize;
};

struct fatent {
	unsigned char cache[512];
    int cacheoffset;
    int fatoffset;
	int sectorNumber;
	int startCluster;
	int curCluster;
	bool eof_disk;
};

#include "dir.h"
#include "file.h"

extern u32 fatInitT(u32 lba);
extern void fatReadCT(u32 lba,char * buffer);
extern u32 fatTraceCCT(u32 lba);
extern u32 getFatLBADATAT();
extern u32 getFatLBAFAT();
extern u32 getFatSecPerCluT();
extern u32 getFatRootCluT();

int fatInit(int lba);
int nxtSector(struct fatent * fat_ent);

#endif
