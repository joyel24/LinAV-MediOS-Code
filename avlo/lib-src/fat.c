// avOS - http://avos.sourceforge.net
// Copyright (c) 2003 by Jimmy Moore
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.
//
// Date:     01/03/2004
// Author:   By Oxygen

#include "string.h"

#include <ata.h>
#include <fat.h>
#include <debug.h>

// mode in updateFatCache
#define WRITE_IF_NEEDED 1
#define ALWAYS_WRITE	2
#define NO_WRITE		3

static char boot[512];

static struct fat_info fatOpen[MAX_DEVICE];
static struct fat_info * curFat;
static int curFatId;


void inifatinfo()
{
	int fd;
	for ( fd=0; fd<MAX_DEVICE; fd++ )
        fatOpen[fd].busy=false;
}

//******************************************************
// fatInit
// do all fat initialization
//
// returns -1 on error
// returns fat fd otherwise
//******************************************************
int fatInit(struct partInfo * partition) {
    int c,lba,fd;
	lba=partition->start;
	struct fat_info * fat;

	for ( fd=0; fd<MAX_DEVICE; fd++ )
        if ( !fatOpen[fd].busy )
            break;

    if ( fd == MAX_DEVICE ) {
		debug("[fatInit] too many fat initialised\n");
        return -1;
    }

	fat=&fatOpen[fd];
	memset(fat, 0, sizeof(struct fat_info));

	fat->busy = true;

    c=ataReadSectorsA(lba, 1, boot);      // Read the bootsector

    if (c!=ATA_ERROR_NONE) {
        debug("Error reading BPB (ata returned:%d)\n",c);
        return -1;
    }

	// some sanity check
	if(bootRead(11,2)!=512)
	{
		debug("[FatIni] bad BPB: bytes per sector != 512 (%d)\n",bootRead(11,2));
		return -1;
	}

	if(bootRead(14,2)==0)
	{
		debug("[FatIni] bad BPB: nb of reserved sectors == 0 (%d)\n",bootRead(14,2));
		return -1;
	}

	if(bootRead(510,1) != 0x55 || bootRead(511,1) != 0xAA)
	{
		debug("[FatIni] bad BPB: wrong end signature (%x%x)\n",bootRead(510,1),bootRead(511,1));
		return -1;
	}

	fat->sectorSize=bootRead(11,2);

	// now we can start computing iteresting values
	//rootSize=((BPB_RootEntCnt*32)+(BPB_BytsPerSec-1))/BPB_BytsPerSec;
	fat->rootSize=(bootRead(17,2)*32+bootRead(11,2)-1)/bootRead(11,2);
	debug("[FatIni] rootSize=%x,rootEntryCnt=%x\n",fat->rootSize,bootRead(17,2));

	if(bootRead(22,2)!=0) //BPB_FatSz16
		fat->fatSize=bootRead(22,2); //BPB_FatSz16
	else
		fat->fatSize=bootRead(36,4); //BPB_FatSz32
	debug("[FatIni] fatSize=%x\n",fat->fatSize);

	if(bootRead(19,2)!=0) //BPB_TotSec16
		fat->totSec=bootRead(19,2); //BPB_TotSec16
	else
		fat->totSec=bootRead(32,4); //BPB_TotSec32
	debug("[FatIni] totSec=%x\n",fat->totSec);

	fat->secPerClu = bootRead(13,1); //BPB_SecPerClus
    debug("[FatIni] secPerClu = %x\n",fat->secPerClu);

	fat->numFats= bootRead(16,1); //BPB_NumFATs
	debug("[FatIni] NumFATs = %x\n",fat->numFats);

	fat->fatStart=bootRead(14,2); //BPB_RsvdSecCnt
	fat->LBAFat1 = lba + fat->fatStart;
	debug("[FatIni] fatStart1=%x (%x)\n",fat->fatStart,fat->LBAFat1);
	if(fat->numFats > 1) // we just use 2 FAT
	{
		fat->LBAFat2 = fat->LBAFat1 + fat->fatSize;
		debug("[FatIni] fatStart2 = %x  (%x)\n",fat->fatStart + fat->fatSize,fat->LBAFat2);
	}
	fat->rootLBA=lba+fat->fatStart+(fat->numFats*fat->fatSize);
	fat->firstDataSector=fat->fatStart+(fat->numFats*fat->fatSize)+fat->rootSize;
	fat->LBAData=fat->firstDataSector+lba;
	debug("[FatIni] firstDataSector= %x  (%x)\n",fat->firstDataSector,fat->LBAData);

	//countOfClusters=(totSec-firstDataSector)/BPB_SecPerClus;
	fat->countOfClusters=(fat->totSec-fat->firstDataSector)/bootRead(13,1);

	if(fat->countOfClusters < 4085)
	{
		fat->fatType=12;
		fat->rootClu=-1;
	}
	else if(fat->countOfClusters < 65525)
		{
			fat->fatType=16;
			fat->rootClu=-1;
		}
		else
		{
			fat->fatType=32;
			fat->rootClu=bootRead(44,4); //BPB_RootClus
		}
	debug("[FatIni] countOfClusters=%x => fatType=%d\n",fat->countOfClusters,fat->fatType);

	debug("[FatIni] rootClu=%x\n",fat->rootClu);

	fat->maxCluster=(fat->fatSize*bootRead(11,2)*8)/fat->fatType;

 	fat->fat_cache.fatBufferLba=-1;
	fat->fat_cache.write_done=false;
	fat->fat_cache.lastFree=-1;

	curFat=fat;
	curFatId=fd;

    return fd;
}

int bootRead(int addr, int n)
{
    int v=0;
    int c=0;
    for (c=0;c<n;c++) {
        v = (v << 8) | boot[addr + n - 1 - c];
    }
    return v;
}

//******************************************************
// closeFat
//
//******************************************************
int closeFat(int fd)
{
	if(fatOpen[fd].busy)
	{
		closeAllFile(fd);
		closeAllDir(fd);
		int oldSelect=curFatId;
		selectFat(fd);
		flushFatCache();
		if(oldSelect != fd)
			selectFat(oldSelect);
		fatOpen[fd].busy=false;
		return 1;
	}
	else
	{
		debug("[closeFat] fat %d not initialized yet\n",fd);
		return 0;
	}
}

//******************************************************
// selectFat
//
//******************************************************
int selectFat(int fd)
{
	if(fatOpen[fd].busy)
	{
		curFat=&fatOpen[fd];
		curFatId=fd;
		return 1;
	}
	else
	{
		debug("[selectFat] fat %d not initialized yet\n",fd);
		return 0;
	}
}

//******************************************************
// fatReadCluster
//
//******************************************************
int fatReadCluster(int cluster, char* buffer)
{
    int sec = curFat->LBAData + ((cluster - 2) * curFat->secPerClu);
    int c=ataReadSectorsA(sec, curFat->secPerClu, buffer);
	if(c!=ATA_ERROR_NONE) {
        debug("ataReadSectors returned error:%d\n",c);
        return c;
    }
	return c;
}

//******************************************************
// isEOChain
//
//******************************************************
bool isEOChain(int cluster)
{
	int cmp;
	switch(curFat->fatType)
	{
		case 32:
			cmp=0x0ffffff8;
			break;
		case 16:
			cmp=0xfff8;
			break;
		case 12:
			cmp=0x0ff8;
			break;
		default:
			debug("[isEOChain] wrong fatType: %d",curFat->fatType);
			return true;
	}
	return (cluster >= cmp);
}

//******************************************************
// fatEndValue
//
//******************************************************
int fatEndValue()
{
	switch(curFat->fatType)
	{
		case 32:
			return 0x0fffffff;
			break;
		case 16:
			return 0xffff;
			break;
		case 12:
			return 0x0fff;
			break;
		default:
			debug("[fatEndValue] wrong fatType: %d",curFat->fatType);
			return true;
	}
}

//******************************************************
// updateFatCache
// change the FAT cache if cluster is out of the current
// cache.
// if WRITE is enable, the previous cache is flushed
//
// mode: WRITE_IF_NEEDED or ALWAYS_WRITE or NO_WRITE
//
// returns 0 on error
//******************************************************
int updateFatCache(int cluster)
{
	int lba,count,c;
	switch(curFat->fatType)
	{
		case 32:
		case 16:
			lba=curFat->LBAFat1+cluster*curFat->fatType/(curFat->sectorSize*8);
			count=1;
			break;
		case 12:
			lba=curFat->LBAFat1+(cluster+cluster/2)/(curFat->sectorSize);
			if(lba+1>curFat->LBAData)
				count=1;
			else
				count=2;
			break;
		default:
			debug("[updateFatCache] wrong fatType: %d",curFat->fatType);
			return 0;
	}
	if (curFat->fat_cache.fatBufferLba != lba)
	{
		//debug("[updateFatCache] need to chg cache\n");
		if(!flushFatCache())
		{
			debug("[updateFatCache] error flushing cache\n");
			return 0;
		}
        c = ataReadSectorsA(lba, count, (char*) curFat->fat_cache.fatBuffer);
        if (c!=ATA_ERROR_NONE)
		{
			debug("[updateFatCache] error reading from disk (ata:%d)\n",c);
			return 0;
		}
        curFat->fat_cache.fatBufferLba = lba;
    }
	return 1;
}

//******************************************************
// flushFatCache
// flush cache to disk
//
// returns 0 on error
//******************************************************
int flushFatCache()
{
	int count,c;
	if(curFat->fat_cache.write_done)
	{
		switch(curFat->fatType)
		{
			case 32:
			case 16:
				count=1;
				break;
			case 12:
			if(curFat->fat_cache.fatBufferLba+1>curFat->LBAData)
				count=1;
			else
				count=2;
				break;
			default:
				debug("[flushFatCache] wrong fatType: %d",curFat->fatType);
				return 0;
		}

		c=ataWriteSectorsA(curFat->fat_cache.fatBufferLba,count,(char*) curFat->fat_cache.fatBuffer);
		if (c!=ATA_ERROR_NONE)
		{
			debug("[flushFatCache] error writing FAT cache in FAT1 (ataerror=%d)\n",c);
			return 0;
		}

		if(curFat->numFats>1)
		{
			c=ataWriteSectorsA(curFat->fat_cache.fatBufferLba+curFat->fatSize,count,(char*) curFat->fat_cache.fatBuffer);
			if (c!=ATA_ERROR_NONE)
			{
				debug("[flushFatCache] error writing FAT cache in FAT2 (ataerror=%d)\n",c);
				return 0;
			}
		}

		curFat->fat_cache.write_done=false;
	}

	return 1;
}

//******************************************************
// readFatCache
//
// returns -1 on error
//******************************************************
int readFatCache(int cluster)
{
	if(!updateFatCache(cluster))
	{
		debug("[readFatCache] error updating fatcache\n");
		return 0;
	}

	int result,offset;
	switch(curFat->fatType)
	{
		case 32:
			offset=((cluster*4) % (curFat->sectorSize));
			result=(curFat->fat_cache.fatBuffer[offset] & 0x000000ff)
				| ((curFat->fat_cache.fatBuffer[offset+1]<<8) & 0x0000ff00)
				| ((curFat->fat_cache.fatBuffer[offset+2]<<16) & 0x00ff0000)
				| ((curFat->fat_cache.fatBuffer[offset+3]<<24) & 0xff000000);
			result&=0x0fffffff;
			break;
		case 16:
			offset=((cluster*2) % (curFat->sectorSize));
			result=(curFat->fat_cache.fatBuffer[offset] & 0xff)
				| ((curFat->fat_cache.fatBuffer[offset+1]<<8) & 0xff00);
			result=result & 0x0000ffff;
			break;
		case 12:
			offset=((cluster+cluster/2) % (curFat->sectorSize));
			result=(curFat->fat_cache.fatBuffer[offset] & 0xff)
				| ((curFat->fat_cache.fatBuffer[offset+1]<<8) & 0xff00);
			if(cluster & 0x0001)
				result= result >> 4;
			else
				result= result & 0x0fff;
			result=result & 0x00000fff;
			break;
		default:
			debug("[readFatCache] wrong fatType: %d",curFat->fatType);
			return 0;
	}
	return result;
}

//******************************************************
// writeFatCache
//
// returns 0 on error
//******************************************************
int writeFatCache(int cluster,int val)
{
	if(!updateFatCache(cluster))
	{
		debug("[writeFatCache] error updating fatcache\n");
		return 0;
	}

	int offset;

	switch(curFat->fatType)
	{
		case 32:
			offset=((cluster*4) % (curFat->sectorSize));
			curFat->fat_cache.fatBuffer[offset]=val & 0x000000ff;
			curFat->fat_cache.fatBuffer[offset+1]=(val>>8) & 0x000000ff;
			curFat->fat_cache.fatBuffer[offset+2]=(val>>16) & 0x000000ff;
			curFat->fat_cache.fatBuffer[offset+3]=curFat->fat_cache.fatBuffer[offset+3] & 0xf0;
			curFat->fat_cache.fatBuffer[offset+3]=curFat->fat_cache.fatBuffer[offset+3] | ((val>>24) & 0x0000000f);
			break;
		case 16:
			offset=((cluster*2) % (curFat->sectorSize));
			curFat->fat_cache.fatBuffer[offset]=val & 0x00ff;
			curFat->fat_cache.fatBuffer[offset+1]=(val>>8) & 0x00ff;
			break;
		case 12:
			offset=((cluster+cluster/2) % (curFat->sectorSize));
			if(cluster & 0x0001)
			{
				val = val << 4;
				curFat->fat_cache.fatBuffer[offset] = curFat->fat_cache.fatBuffer[offset] & 0x0f;
				curFat->fat_cache.fatBuffer[offset+1]=0x00;
			}
			else
			{
				val = val & 0x0fff;
				curFat->fat_cache.fatBuffer[offset]=0x00;
				curFat->fat_cache.fatBuffer[offset+1] = curFat->fat_cache.fatBuffer[offset+1] & 0xf0;
			}
			curFat->fat_cache.fatBuffer[offset]=curFat->fat_cache.fatBuffer[offset] | (val & 0xff);
			curFat->fat_cache.fatBuffer[offset+1]=curFat->fat_cache.fatBuffer[offset+1] | ((val>>8) & 0xff);
			break;
		default:
			debug("[writeFatCache] wrong fatType: %d",curFat->fatType);
			return 0;
	}

	curFat->fat_cache.write_done=true;

	return 1;

}

//******************************************************
// countFreeCluster
// returns the number of free cluster//
//
// returns <0 on error
//******************************************************
int countFreeCluster()
{
	int cluster,count;
	count=0;

	for(cluster=2;cluster<curFat->maxCluster;cluster++)
		if(!(readFatCache(cluster) & 0x0fffffff))
				count++;
	return count;
}

//******************************************************
// fatTrace
// get next cluster in cluster chain
//
// returns -1 on error
//******************************************************
int fatTrace(int cluster)
{
	return readFatCache(cluster);
}

//******************************************************
// getNxtFreeCluster
// try to find next free cluster (starting at a given
// cluster number)
// if cluster == -1 then try starting at lastFree
//
// returns -1 on error
// returns new cluster number otherwise
//******************************************************
int getNxtFreeCluster(int cluster)
{
	if(cluster==-1 && curFat->fat_cache.lastFree!=-1)
	{
		if((cluster=getNxtFreeCluster(curFat->fat_cache.lastFree))>=0)
		{
			curFat->fat_cache.lastFree=cluster;
			writeFatCache(cluster,fatEndValue());
			return cluster;
		}
		else
		{
			cluster=2;
			curFat->fat_cache.lastFree=-1;
		}
	}

	if(cluster<2) cluster=2; // we can't use cluster 0 nor cluster 1

	while(cluster < curFat->maxCluster && (readFatCache(cluster) & 0x0fffffff))
		cluster++;

	if(cluster < curFat->maxCluster)
	{
		curFat->fat_cache.lastFree=cluster;
		writeFatCache(cluster,fatEndValue());
		return cluster;
	}
	else
	{
		debug("[getNxtFreeCluster] no more free cluster\n");
		return -1;
	}
}

//******************************************************
// addNewCluster
// try to add a cluster to a cluster chain:
// 1. looking from this buffer to the end;
// 2. starting from cluster 0;
//
// returns -1 on error
// returns new cluster number otherwise
//******************************************************
int addNewCluster(int cluster)
{
	if(cluster<2) cluster=2; // we can't use cluster 0 nor cluster 1

	int curCluster=cluster;
	int endCluster=curFat->maxCluster;

	while(curCluster<endCluster && (readFatCache(curCluster) & 0x0fffffff))
		curCluster++;

	if(curCluster<endCluster)
	{
		curFat->fat_cache.lastFree=curCluster;
		writeFatCache(curCluster,fatEndValue());
	}
	else
	{
		if((curCluster = getNxtFreeCluster(-1))<0) // try to get a free cluster starting search at cluster 0
		{
			debug("[addNewCluster] can't find a free cluster\n");
			return -1;
		}
	}

	writeFatCache(cluster,curCluster);

	flushFatCache();

	return curCluster;
}

//******************************************************
// fatRWSector
// read or write sector fat_ent.curCluster
// using the data in fat_ent.cache
//
// returns 0 on error
//******************************************************
int fatRWSector(struct fatent * fat_ent,bool write)
{
	int lba;

	if(fat_ent->isRootDir && curFat->fatType !=32)
	{
		if(fat_ent->sectorNumber > curFat->rootSize)
		{
			fat_ent->eof_disk=true;
			return 0;
		}
		lba=curFat->rootLBA+fat_ent->sectorNumber;
	}
	else
	{
		if(fat_ent->curCluster < curFat->rootClu || fat_ent->curCluster > curFat->countOfClusters+1)
		{
			debug("error accessing bad cluster number: %d\n",fat_ent->curCluster);
			return 0;
		}
		lba=(fat_ent->curCluster-2)*curFat->secPerClu+curFat->LBAData+fat_ent->sectorNumber;
	}

	int c;
	if(write)
		c=ataWriteSectorsA(lba, 1, fat_ent->cache);
	else
		c=ataReadSectorsA(lba, 1, fat_ent->cache);


	if (c!=ATA_ERROR_NONE)
	{
		if(write)
			debug("error writing sector %x,%d (ataerror=%d)\n",fat_ent->curCluster,fat_ent->sectorNumber,c);
		else
			debug("error reading sector %x,%d (ataerror=%d)\n",fat_ent->curCluster,fat_ent->sectorNumber,c);

		return 0;
	}

	return 1;
}

//******************************************************
// fatNxtSector
// move sector pointer to next sector
// change cluster if needed
// if write is enable, create new cluster if the end
// of the cluster chain is reached
//
// returns 0 on error
//******************************************************
int fatNxtSector(struct fatent * fat_ent,bool write)
{
	if(fat_ent->eof_disk)
	{
		debug("error end of file already reached\n");
		return 0;
	}

	if(fat_ent->startCluster==-1)
	{
		fat_ent->isRootDir=true;
	}

	if(fat_ent->curCluster==-1) // we start a new fat_ent
	{
		if(fat_ent->isRootDir && curFat->fatType !=32 )
		{
			//debug("beg of rootDir\n");
			fat_ent->sectorNumber=0;
			fat_ent->curCluster=0;
			fat_ent->cacheoffset=0;
			return 1;
		}
		else
		{
			//debug("beg of normal sector\n");
			fat_ent->curCluster=fat_ent->startCluster;
			fat_ent->sectorNumber=0;
			fat_ent->prevCluster=-1;
		}
	}
	else
	{
		fat_ent->sectorNumber++;
	}

	if(fat_ent->isRootDir && curFat->fatType !=32)
	{
		if( fat_ent->sectorNumber > curFat->rootSize)
		{
			fat_ent->eof_disk=true;
		}
		fat_ent->cacheoffset=0;
		return 1;
	}


	if(fat_ent->sectorNumber>=curFat->secPerClu) // need to change cluster
	{
		fat_ent->prevCluster=fat_ent->curCluster;
		fat_ent->curCluster=fatTrace(fat_ent->curCluster);

		//debug("[fatNxtSector] result of fatTrace:%x (base on:%x)\n",fat_ent->curCluster,fat_ent->prevCluster);

		if(!(fat_ent->curCluster && 0x0fffffff) )
		{
			debug("[fatNxtSector] error: nxt cluster == 0 (prev=%d)\n",fat_ent->prevCluster);
			return 0;
		}

		if(isEOChain(fat_ent->curCluster))
		{
			//debug("[fatNxtSector] end of chain\n");
			if(write)
			{
				if((fat_ent->curCluster=addNewCluster(fat_ent->prevCluster)) < 0)
				{
					debug("[fatNxtSector] error creating new cluster\n");
					return 0;
				}
				fatCleanCluster(fat_ent->curCluster);
			}
			else
				fat_ent->eof_disk=true;
		}
		fat_ent->sectorNumber=0;
	}

	fat_ent->cacheoffset=0;

	return 1;
}

//******************************************************
// fatPrevSector
// move sector pointer to previous sector
// change cluster if needed
//
// returns 0 on error
//******************************************************
int fatPrevSector(struct fatent * fat_ent)
{
	if(fat_ent->startCluster==-1)
	{
		fat_ent->isRootDir=true;
	}

	if(fat_ent->isRootDir && curFat->fatType !=32)
	{
		if(fat_ent->sectorNumber>0)
		{
			fat_ent->sectorNumber--;
		}
		else
		{
			debug("[fatPrevSector] can't go back\n");
			return 0;
		}

	}
	else
	{
		if(fat_ent->curCluster==-1) // we start a new fat_ent
		{
			debug("[fatPrevSector] beg of file\n");
			return 0;
		}

		if(fat_ent->sectorNumber>0)
		{
			fat_ent->sectorNumber--;
		}
		else
		{
			if(fat_ent->prevCluster==-1) // we need to trace from the beg of the file
			{
				int oldCluster=fat_ent->curCluster;
				fat_ent->prevCluster=fat_ent->startCluster;
				fat_ent->curCluster=fatTrace(fat_ent->startCluster);

				while(fat_ent->curCluster!=oldCluster && !isEOChain(fat_ent->curCluster))
				{
					fat_ent->prevCluster=fat_ent->curCluster;
					fat_ent->curCluster=fatTrace(fat_ent->curCluster);
				}

				if(isEOChain(fat_ent->curCluster))
				{
					debug("[fatPrevSector] can't find prev sector\n");
					return 0;
				}
			}

			fat_ent->curCluster=fat_ent->prevCluster;
			fat_ent->prevCluster=-1;
			fat_ent->sectorNumber=curFat->secPerClu-1;
		}
	}

	fat_ent->cacheoffset=0;
	fatRWSector(fat_ent,false);
	return 1;
}

//******************************************************
// fatCleanCluster
// clean all cluster data (put 0x00)
//
// returns 0 on error
//******************************************************
int fatCleanCluster(int cluster)
{
    char cache[BUFFER_SIZE];
	memset(cache,0,BUFFER_SIZE);
	int secNum,c;
	int lba=(cluster-2)*curFat->secPerClu+curFat->LBAData;

	for(secNum=0;secNum<curFat->secPerClu;secNum++)
		if((c=ataWriteSectorsA(lba+secNum,1, cache))!=ATA_ERROR_NONE)
		{
			debug("error writing sector (ataerror=%d)\n",c);
			break;
		}

	if(secNum<curFat->secPerClu)
		return 0;
	else
		return 1;

}

//******************************************************
// fatRWEntry
// read/write dirEntry from/to disk
// fat_ent points to next dir
// entry after operation, if the entry is not an end
// entry, a new cluster is added if needed
//
// returns 0 on error
//******************************************************
int fatRWEntry(struct fatent * fat_ent,struct dirEntry * entry,bool write)
{
	int count=FAT_ENTRY_SIZE;
	int pos=0;
	if(curFat->sectorSize<=fat_ent->cacheoffset) // let's move to next cluster
	{
		if(!fatNxtSector(fat_ent,write))
		{
			debug("[fatRWEntry] error getting nxt sector (1)\n");
			return 0;
		}

		if(write || (!write && !fat_ent->eof_disk))
		{
			if(!fatRWSector(fat_ent,false))
			{
				debug("[fatRWEntry] error reading nxt sector\n");
				return 0;
			}
		}

		if(fat_ent->eof_disk)
		{
			debug("[fatRWEntry] end of cluster chain, before end of direntry\n");
			return 0;
		}
	}

	if(curFat->sectorSize-fat_ent->cacheoffset<FAT_ENTRY_SIZE)
	{
		if(write)
		{
			memcpy(&fat_ent->cache[fat_ent->cacheoffset],entry,curFat->sectorSize-fat_ent->cacheoffset);
			if(!fatRWSector(fat_ent,true))
			{
				debug("[fatRWEntry] error writing cur sector\n");
				return 0;
			}
		}
		else
		{
			memcpy(entry,&fat_ent->cache[fat_ent->cacheoffset],curFat->sectorSize-fat_ent->cacheoffset);
		}

		pos=curFat->sectorSize-fat_ent->cacheoffset;
		count=FAT_ENTRY_SIZE-pos;

  		if(!fatNxtSector(fat_ent,write))
		{
			debug("[fatRWEntry] error getting nxt sector (1)\n");
			return 0;
		}

		if(fat_ent->eof_disk)
		{
			debug("[fatRWEntry] end of cluster chain, before end of direntry\n");
			return 0;
		}

		if(!fatRWSector(fat_ent,false))
		{
			debug("[fatRWEntry] error reading nxt sector\n");
			return 0;
		}
	}

	if(count>0 )
	{
		if(write)
		{
			memcpy(&(fat_ent->cache[fat_ent->cacheoffset]),&entry[pos],count);
			if(!fatRWSector(fat_ent,true)) // we have change a dir entry => flush this buffer
			{
				debug("[fatRWEntry] error reading/writing nxt sector (2)\n");
				return 0;
			}
		}
		else
		{
			memcpy(&entry[pos],&(fat_ent->cache[fat_ent->cacheoffset]),count);
		}
		fat_ent->cacheoffset+= count;
	}
	return 1;
}

//******************************************************
// fatNxtEntry
// read entry and move to nxt dir entry
//
// returns 0 on error
//******************************************************
int fatNxtEntry(struct fatent * fat_ent,struct dirEntry * entry)
{
	return fatRWEntry(fat_ent,entry,false);
}

//******************************************************
// fatPrevEntry
// move entry cursor to prev entry
//
// returns 0 on error
//******************************************************
int fatPrevEntry(struct fatent * fat_ent)
{
	int nxtOffset=curFat->sectorSize-(FAT_ENTRY_SIZE-fat_ent->cacheoffset);
	if(fat_ent->cacheoffset < FAT_ENTRY_SIZE)
	{
		if(fatPrevSector(fat_ent))
		{
			fat_ent->cacheoffset=nxtOffset;
		}
		else
		{
			debug("fatPrevEntry: can't get previous sector\n");
			return 0;
		}
	}
	else
	{
		fat_ent->cacheoffset -= FAT_ENTRY_SIZE;
	}
	return 1;
}

//******************************************************
// fatRemoveEntry
// remove an entry from a dir
//
// returns 0 on error
//******************************************************
int fatRemoveEntry(struct fatent * fat_ent)
{
	struct dirEntry entry;
	memset(&entry,0,sizeof(struct dirEntry));

	// we have to remove the previous entry

	// is this the last entry?

	if(!fatNxtEntry(fat_ent,&entry))
	{
		debug("fatRemoveEntry: error getting nxt entry\n");
		return 0;
	}

	if(!fatPrevEntry(fat_ent))
	{
		debug("fatRemoveEntry: error getting prev entry\n");
		return 0;
	}

	int type=fatValidateEntry(&entry);
	int entryId;

	int sectorNum=fat_ent->sectorNumber;
	int cacheOffset=fat_ent->cacheoffset;

	if(!fatPrevEntry(fat_ent))
	{
		debug("fatRemoveEntry: error getting prev entry\n");
		return 0;
	}

	if(type == END_ENTRY || fat_ent->eof_disk)
	{
		// test if we need to remove a cluster
		if(sectorNum==0 && cacheOffset==0)
		{
			fatTruncate(fat_ent,false);
		}
		entryId=0x0;
	}
	else
	{
		entryId=0xe5;
	}

	memset(&entry,0,sizeof(struct dirEntry));
	entry.name[0]=entryId;

	if(!fatRWEntry(fat_ent,&entry,true))
	{
		debug("fatRemoveEntry: can't write new entry\n");
		return 0;
	}

	return 1;
}

//******************************************************
// fatUpdateEntry
//
// returns 0 on error
//******************************************************
int fatUpdateEntry(struct dirent * ent)
{
	struct fatent fat_ent;
	struct dirEntry entry;
	if(!fatFindEntry(&fat_ent,ent,&entry))
	{
		debug("fatUpdateEntry: can't find entry\n");
		return 0;
	}

	/////////////////////////////////// data to update

	entry.size=ent->size;

	/////////////////////////////////////////////

	if(!fatRWEntry(&fat_ent,&entry,true))
	{
		debug("fatUpdateEntry: can't write new entry\n");
		return 0;
	}

	return 1;
}

//******************************************************
// fatFindEntry
// find an entry
//
// returns 0 on error
//******************************************************
int fatFindEntry(struct fatent * fat_ent,struct dirent * ent,struct dirEntry * entry)
{
	int i;

	memset(entry,0,sizeof(struct dirEntry));
	memset(fat_ent,0,sizeof(struct fatent));

	fat_ent->startCluster=ent->dirCluster;
	fat_ent->curCluster=-1;
	fat_ent->prevCluster=-1;
	fat_ent->eof_disk=false;

	if(!fatNxtSector(fat_ent,false))
	{
		debug("fatFindEntry: can't go to the first sector\n");
		return 0;
	}

	if(!fatRWSector(fat_ent,false))
	{
		debug("fatFindEntry: can't read the first sector\n");
		return 0;
	}

	bool done=false;

	while(!done)
	{
		if(!fatNxtEntry(fat_ent,entry))
		{
			debug("fatFindEntry: error getting nxt entry\n");
			return 0;
		}

		int type=fatValidateEntry(entry);

		if(type != EMPTY_ENTRY && type != BAD_ENTRY)
		{
			if(type == END_ENTRY || fat_ent->eof_disk)
			{
				break;
			}
			else
			{
				i=0;
				while((i<(NAME_SIZE+EXT_SIZE)) && (ent->fatName[i] == entry->name[i]))
					i++;
				if(i==NAME_SIZE+EXT_SIZE)
					done=true;
			}
		}
	}

	if(!done)
	{
		return 0;
	}
	else
	{
		if(!fatPrevEntry(fat_ent))
		{
			debug("fatFindEntry: error getting prev entry\n");
			return 0;
		}
		return 1;
	}

}
//******************************************************
// fatCreateEntry
// add an entry to dir (fat_ent)
//
// returns 0 on error
//******************************************************
int fatCreateEntry(struct fatent * fat_ent,struct dirent* ent,const char * name)
{
	// first intitialize a dirEntry structure
	struct dirEntry entry;

	memset(&entry,0,sizeof(struct dirEntry));

	int cluster=getNxtFreeCluster(-1);

	entry.fatCluHI=(cluster >> 16) & 0xffff;
	entry.fatCluLO=cluster & 0xffff;
	entry.size=0;

	memcpy(entry.name,name,NAME_SIZE+EXT_SIZE);

	fatGetData(ent,&entry);
	ent->dirCluster=fat_ent->startCluster;
	ent->startCluster=cluster;


	// we will need to add some stuff here related to time/date
	///////////////////////////////////////////

	// let's find and empty entry;
	struct dirEntry entry2;

	fat_ent->curCluster=-1;
	if(!fatNxtSector(fat_ent,false))
	{
		debug("fatCreateEntry: error getting nxt sector\n");
		return 0;
	}

	if(!fatRWSector(fat_ent,false))
	{
		debug("fatCreateEntry: error reading sector\n");
		return 0;
	}

	do
		if(!fatNxtEntry(fat_ent,&entry2)) // let's read entry and move to nxt entry
		{
			debug("fatCreateEntry: error getting nxt entry\n");
			return 0;
		}
	while(entry2.name[0] != 0x0 && entry2.name[0] != 0xe5 && !fat_ent->eof_disk);

	if(fat_ent->eof_disk)
	{
		debug("fatCreateEntry: error end of cluster chain\n");
		return 0;
	}

	// we have found an empty entry => we have to go 1 entry back

	if(!fatPrevEntry(fat_ent))
	{
		debug("fatCreateEntry: error going back\n");
		return 0;
	}

	if(!fatRWEntry(fat_ent,&entry,true)) // let's write new entry
	{
		debug("fatCreateEntry: error writing new entry\n");
		return 0;
	}

	if(entry2.name[0] == 0x0) //we need to add a 0x0 entry after
	{
		memset(&entry2,0,sizeof(struct dirEntry));
		if(!fatRWEntry(fat_ent,&entry2,true)) // let's add new 0x0 entry;
		{
			debug("fatCreateEntry: error writing 0x0 entry\n");
			return 0;
		}
	}

	// let's reset the position to first dir entry:
	fat_ent->curCluster=-1;
	if(!fatNxtSector(fat_ent,false))
	{
		debug("fatCreateEntry: error getting nxt sector\n");
		return 0;
	}
	return 1;
}

//******************************************************
// fatValidateEntry
//
// returns entry type
//******************************************************
int fatValidateEntry(struct dirEntry * entry)
{
	if(entry->name[0] == 0x0)
		return END_ENTRY;

	if(entry->name[0] == 0xe5)
		return EMPTY_ENTRY;

	if(((entry->attr & FAT_ATTR_VOLUME_ID) == FAT_ATTR_VOLUME_ID) || entry->attr == (FAT_ATTR_LONG_NAME))
		return BAD_ENTRY;

	return GOOD_ENTRY;
}

//******************************************************
// fatGetData
// stores dirEntry data in dirent format
//
//******************************************************
void fatGetData(struct dirent *theent,struct dirEntry * entry)
{
	int i,j;

	for(i=0;i<NAME_SIZE;i++)
		if(entry->name[i] == ' ')
			break;
		else
		{
			theent->entryName[i]=entry->name[i];
			theent->name[i]=entry->name[i];
		}

	theent->name[i]=0x0;

	if(entry->ext[0]!=' ')
	{
		theent->entryName[i]='.';
		i++;

		for(j=0;j<EXT_SIZE;j++)
			if(entry->ext[j] == ' ')
				break;
			else
			{
				theent->entryName[i]=entry->ext[j];
				theent->ext[j]=entry->ext[j];
				i++;
			}
	}

	theent->entryName[i]=0x0;
	theent->ext[j]=0x0;

	theent->attribute=entry->attr;
	int val = entry->fatCluHI << 16;
	theent->startCluster = val + entry->fatCluLO;
	theent->size=entry->size;
}

//******************************************************
// fatOpendir
// fat operations to open a dir
//
//******************************************************
void fatOpendir(struct fatent * fat_ent,int startCluster)
{
	if(startCluster==-1 || startCluster == 0)
	{
		fat_ent->isRootDir=true;
		if(curFat->fatType != 32)
			fat_ent->startCluster=-1;
		else
			fat_ent->startCluster=curFat->rootClu;
	}
	else
	{
		fat_ent->startCluster=startCluster;
		fat_ent->isRootDir=false;
	}

	fat_ent->curCluster=-1;
	fat_ent->cacheoffset=0;
	fat_ent->eof_disk=false;
	fat_ent->fatId=curFatId;

	fatNxtSector(fat_ent,false);
	fatRWSector(fat_ent,false);
}


//******************************************************
// getClusterSize
//
//******************************************************
int getClusterSize() {
    return curFat->secPerClu*curFat->sectorSize;
}

//******************************************************
// fatTruncate
// truncate the file based on the position in fat_ent
// dir entry is not updated
//
// returns 0 on error
//******************************************************
int fatTruncate(struct fatent * fat_ent,bool total_remove)
{
	int curCluster=fat_ent->curCluster;
	int nxtCluster;
	int res;

	if((nxtCluster=readFatCache(curCluster))<0)
	{
		debug("[fatTruncate] error getting first cluster of file\n");
		return 0;
	}

	if(total_remove)
		res=writeFatCache(curCluster,0);
	else
		res=writeFatCache(curCluster,fatEndValue());

	if(!res)
	{
		debug("[fatTruncate] error writing first cluster of file\n");
		return 0;
	}

	curCluster=nxtCluster;

	while(!isEOChain(curCluster))
	{
		if((nxtCluster=readFatCache(curCluster))<0)
		{
			debug("[fatTruncate] error getting next cluster of file\n");
			return 0;
		}

		if(!writeFatCache(curCluster,0))
		{
			debug("[fatTruncate] error writing next cluster of file\n");
			return 0;
		}

		curCluster=nxtCluster;
	}

	if(!flushFatCache())
	{
		debug("[fatTruncate] error flushing FAT cache\n");
		return 0;
	}

	return 1;
}

char chkChar(char c)
{
	if(c == 0xe5)
		return 0x05;

	if(c>0x7e || c<0x21)
		return 0;

	switch(c)
    {
        case 0x22:
        case 0x2a:
        case 0x2b:
        case 0x2c:
        case 0x2e:
		case 0x2f:
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
            c = 0;
            break;

        default:
				c=toupper(c);
            break;
    }
    return c;
}

void createDosName(char *name, char *fatName)
{

	int i=0;
	int j=0;
	char c;

	if(name[0] == '.') // name can't start by a . => adding _ at the beg of the name
	{
		fatName[0]='_';
		i++;
	}

	while(name[j]!='.' && name[j] != 0x0 && i<NAME_SIZE)
	{
		if((c=chkChar(name[j])))
		{
			fatName[i]=c;
			i++;
		}
		j++;
	}

	if(i==NAME_SIZE && name[j]!='.') // name too big, let's skip it
	{
		while(name[j]!='.' && name[j] != 0x0)
			j++;
	}

	if(name[j]=='.')
	{
		j++;
		for(;i<NAME_SIZE;i++) // before copying ext, let's fill the name with space
			fatName[i]=' ';
		while(name[j] != 0x0 && i<NAME_SIZE+EXT_SIZE) // copying the ext
		{
			if((c=chkChar(name[j])))
			{
				fatName[i]=c;
				i++;
			}
			j++;
		}
		for(;(i-NAME_SIZE)<EXT_SIZE;i++) // let's fill ext with space if not full
			fatName[i]=' ';
	}
	else
		for(;i<(NAME_SIZE+EXT_SIZE);i++) // we didn't get an ext let's fill the rest with space
			fatName[i]=' ';
}


///////////////////////////////////////////////////////////////////////////////////
// from initial fat.c

int fatDirFilter(struct dirEntry dirIn[], struct dirEntry dirOut[], int n) {
    int cpin=0,cpo=0;

    for (cpin=0;cpin<n;cpin++) {
        if (dirIn[cpin].name[0]==0)
		{
            break;
        }
		else
		{
			if (dirIn[cpin].name[0]!=0xe5)
			{
				if ((!(dirIn[cpin].attr & FAT_ATTR_VOLUME_ID)) && (dirIn[cpin].attr != (FAT_ATTR_LONG_NAME)))
				{
					dirOut[cpo++] = dirIn[cpin];
				}
			}
		}
    }
    return cpo;
}

int fatReadFile(int cluster, char* buffer) {
    int c;
    while(1) {
		debug("[fat.c] reading cluster %x\n",cluster);
        c = fatReadCluster(cluster, buffer);        // Read data...
        if (c!=ATA_ERROR_NONE) return c;
        buffer = buffer + (curFat->secPerClu*curFat->sectorSize);      // Move along,
        cluster = fatTrace(cluster);            // Trace the fat
        if (cluster<0) return cluster;          // ATA Error!
        if (isEOChain(cluster)) return 0;       // End of chain...
    }
	return ATA_ERROR_NONE;
}

int getRootClu() {
    return curFat->rootClu;
}
