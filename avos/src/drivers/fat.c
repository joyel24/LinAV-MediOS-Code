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
static int fatSize;
static int rootSize;
static int firstDataSector;
static int fatStart;
static int numFats;
static int totSec;
static int countOfClusters;
static int fatType;
static int LBAFat1;
static int LBAFat2;
static int secPerClu;
static int LBAData;
static int rootClu;
static int rootLBA;
static int maxCluster;

static struct fatCache fat_cache;               // 1 sector fatcache

//******************************************************
// fatInit
// do all fat initialization
//
// returns 0 on error
//******************************************************
int fatInit(struct partInfo * partition) {
    int c,lba;
	lba=partition->start;

    c=ataReadSectorsA(lba, 1, boot);      // Read the bootsector

    if (c!=ATA_ERROR_NONE) {
        debug("Error reading BPB (ata returned:%d)\n",c);
        return 0;
    }

	// some sanity check
	if(bootRead(11,2)!=512)
	{
		debug("[FatIni] bad BPB: bytes per sector != 512 (%d)\n",bootRead(11,2));
		return 0;
	}

	if(bootRead(14,2)==0)
	{
		debug("[FatIni] bad BPB: nb of reserved sectors == 0 (%d)\n",bootRead(14,2));
		return 0;
	}

	if(bootRead(510,1) != 0x55 || bootRead(511,1) != 0xAA)
	{
		debug("[FatIni] bad BPB: wrong end signature (%x%x)\n",bootRead(510,1),bootRead(511,1));
		return 0;
	}

	// now we can start computing iteresting values
	//rootSize=((BPB_RootEntCnt*32)+(BPB_BytsPerSec-1))/BPB_BytsPerSec;
	rootSize=(bootRead(17,2)*32+bootRead(11,2)-1)/bootRead(11,2);
	debug("[FatIni] rootSize=%x,rootEntryCnt=%x\n",rootSize,bootRead(17,2));

	if(bootRead(22,2)!=0) //BPB_FatSz16
		fatSize=bootRead(22,2); //BPB_FatSz16
	else
		fatSize=bootRead(36,4); //BPB_FatSz32
	debug("[FatIni] fatSize=%x\n",fatSize);

	if(bootRead(19,2)!=0) //BPB_TotSec16
		totSec=bootRead(19,2); //BPB_TotSec16
	else
		totSec=bootRead(32,4); //BPB_TotSec32
	debug("[FatIni] totSec=%x\n",totSec);

	secPerClu = bootRead(13,1); //BPB_SecPerClus
    debug("[FatIni] secPerClu = %x\n",secPerClu);

	numFats= bootRead(16,1); //BPB_NumFATs
	debug("[FatIni] NumFATs = %x\n",numFats);

	fatStart=bootRead(14,2); //BPB_RsvdSecCnt
	LBAFat1 = lba + fatStart;
	debug("[FatIni] fatStart1=%x (%x)\n",fatStart,LBAFat1);
	if(numFats > 1) // we just use 2 FAT
	{
		LBAFat2 = LBAFat1 + fatSize;
		debug("[FatIni] fatStart2 = %x  (%x)\n",fatStart+ fatSize,LBAFat2);
	}
	rootLBA=lba+fatStart+(numFats*fatSize);
	firstDataSector=fatStart+(numFats*fatSize)+rootSize;
	LBAData=firstDataSector+lba;
	debug("[FatIni] firstDataSector= %x  (%x)\n",firstDataSector,LBAData);

	//countOfClusters=(totSec-firstDataSector)/BPB_SecPerClus;
	countOfClusters=(totSec-firstDataSector)/bootRead(13,1);

	if(countOfClusters < 4085)
	{
		fatType=12;
		rootClu=-1;
	}
	else if(countOfClusters < 65525)
		{
			fatType=16;
			rootClu=-1;
		}
		else
		{
			fatType=32;
			rootClu=bootRead(44,4); //BPB_RootClus
		}
	debug("[FatIni] countOfClusters=%x => fatType=%d\n",countOfClusters,fatType);

	debug("[FatIni] rootClu=%x\n",rootClu);

	maxCluster=(fatSize*bootRead(11,2)*8)/fatType;

 	fat_cache.fatBufferLba=-1;
	fat_cache.write_done=false;
	fat_cache.lastFree=-1;

    return 1;
}

//******************************************************
// fatReadCluster
//
//******************************************************
int fatReadCluster(int cluster, char* buffer)
{
    int sec = LBAData + ((cluster - 2) * secPerClu);
    int c=ataReadSectorsA(sec, secPerClu, buffer);
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
	switch(fatType)
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
			debug("[isEOChain] wrong fatType: %d",fatType);
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
	switch(fatType)
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
			debug("[fatEndValue] wrong fatType: %d",fatType);
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
	switch(fatType)
	{
		case 32:
		case 16:
			lba=LBAFat1+cluster*fatType/(SECTOR_SIZE*8);
			count=1;
			break;
		case 12:
			lba=LBAFat1+(cluster+cluster/2)/(SECTOR_SIZE);
			if(lba+1>LBAData)
				count=1;
			else
				count=2;
			break;
		default:
			debug("[updateFatCache] wrong fatType: %d",fatType);
			return 0;
	}
	if (fat_cache.fatBufferLba != lba)
	{
		//debug("[updateFatCache] need to chg cache\n");
		if(!flushFatCache())
		{
			debug("[updateFatCache] error flushing cache\n");
			return 0;
		}
        c = ataReadSectorsA(lba, count, (char*) fat_cache.fatBuffer);
        if (c!=ATA_ERROR_NONE)
		{
			debug("[updateFatCache] error reading from disk (ata:%d)\n",c);
			return 0;
		}
        fat_cache.fatBufferLba = lba;
    }
	/*else
		debug("[updateFatCache] cache ok\n");*/
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
	if(fat_cache.write_done)
	{
		printBuffer(fat_cache.fatBuffer,32);
		switch(fatType)
		{
			case 32:
			case 16:
				count=1;
				break;
			case 12:
			if(fat_cache.fatBufferLba+1>LBAData)
				count=1;
			else
				count=2;
				break;
			default:
				debug("[flushFatCache] wrong fatType: %d",fatType);
				return 0;
		}

		c=ataWriteSectorsA(fat_cache.fatBufferLba,count,(char*) fat_cache.fatBuffer);
		if (c!=ATA_ERROR_NONE)
		{
			debug("[flushFatCache] error writing FAT cache in FAT1 (ataerror=%d)\n",c);
			return 0;
		}

		if(numFats>1)
		{
			c=ataWriteSectorsA(fat_cache.fatBufferLba+fatSize,count,(char*) fat_cache.fatBuffer);
			if (c!=ATA_ERROR_NONE)
			{
				debug("[flushFatCache] error writing FAT cache in FAT2 (ataerror=%d)\n",c);
				return 0;
			}
		}

		fat_cache.write_done=false;
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
	switch(fatType)
	{
		case 32:
			offset=((cluster*4) % (SECTOR_SIZE));
			result=(fat_cache.fatBuffer[offset] & 0x000000ff)
				| ((fat_cache.fatBuffer[offset+1]<<8) & 0x0000ff00)
				| ((fat_cache.fatBuffer[offset+2]<<16) & 0x00ff0000)
				| ((fat_cache.fatBuffer[offset+3]<<24) & 0xff000000);
			result&=0x0fffffff;
			break;
		case 16:
			offset=((cluster*2) % (SECTOR_SIZE));
			result=(fat_cache.fatBuffer[offset] & 0xff)
				| ((fat_cache.fatBuffer[offset+1]<<8) & 0xff00);
			result=result & 0x0000ffff;
			break;
		case 12:
			offset=((cluster+cluster/2) % (SECTOR_SIZE));
			result=(fat_cache.fatBuffer[offset] & 0xff) | ((fat_cache.fatBuffer[offset+1]<<8) & 0xff00);
			if(cluster & 0x0001)
				result= result >> 4;
			else
				result= result & 0x0fff;
			result=result & 0x00000fff;
			break;
		default:
			debug("[readFatCache] wrong fatType: %d",fatType);
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

	switch(fatType)
	{
		case 32:
			offset=((cluster*4) % (SECTOR_SIZE));
			fat_cache.fatBuffer[offset]=val & 0x000000ff;
			fat_cache.fatBuffer[offset+1]=(val>>8) & 0x000000ff;
			fat_cache.fatBuffer[offset+2]=(val>>16) & 0x000000ff;
			fat_cache.fatBuffer[offset+3]=fat_cache.fatBuffer[offset+3] & 0xf0;
			fat_cache.fatBuffer[offset+3]=fat_cache.fatBuffer[offset+3] | ((val>>24) & 0x0000000f);
			break;
		case 16:
			offset=((cluster*2) % (SECTOR_SIZE));
			fat_cache.fatBuffer[offset]=val & 0x00ff;
			fat_cache.fatBuffer[offset+1]=(val>>8) & 0x00ff;
			break;
		case 12:
			offset=((cluster+cluster/2) % (SECTOR_SIZE));
			if(cluster & 0x0001)
			{
				val = val << 4;
				fat_cache.fatBuffer[offset] = fat_cache.fatBuffer[offset] & 0x0f;
				fat_cache.fatBuffer[offset+1]=0x00;
			}
			else
			{
				val = val & 0x0fff;
				fat_cache.fatBuffer[offset]=0x00;
				fat_cache.fatBuffer[offset+1] = fat_cache.fatBuffer[offset+1] & 0xf0;
			}
			fat_cache.fatBuffer[offset]=fat_cache.fatBuffer[offset] | (val & 0xff);
			fat_cache.fatBuffer[offset+1]=fat_cache.fatBuffer[offset+1] | ((val>>8) & 0xff);
			break;
		default:
			debug("[writeFatCache] wrong fatType: %d",fatType);
			return 0;
	}

	fat_cache.write_done=true;

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

	for(cluster=2;cluster<maxCluster;cluster++)
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
	if(cluster==-1 && fat_cache.lastFree!=-1)
	{
		if((cluster=getNxtFreeCluster(fat_cache.lastFree))>=0)
		{
			fat_cache.lastFree=cluster;
			writeFatCache(cluster,fatEndValue());
			return cluster;
		}
		else
		{
			cluster=2;
			fat_cache.lastFree=-1;
		}
	}

	if(cluster<2) cluster=2; // we can't use cluster 0 nor cluster 1

	while(cluster < maxCluster && (readFatCache(cluster) & 0x0fffffff))
		cluster++;

	if(cluster < maxCluster)
	{
		fat_cache.lastFree=cluster;
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
	int endCluster=maxCluster;

	while(curCluster<endCluster && (readFatCache(curCluster) & 0x0fffffff))
		curCluster++;

	if(curCluster<endCluster)
	{
		fat_cache.lastFree=curCluster;
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

	if(fat_ent->isRootDir && fatType !=32)
	{
		if(fat_ent->sectorNumber > rootSize)
		{
			fat_ent->eof_disk=true;
			return 0;
		}
		lba=rootLBA+fat_ent->sectorNumber;
	}
	else
	{
		if(fat_ent->curCluster < rootClu || fat_ent->curCluster > countOfClusters+1)
		{
			debug("error accessing bad cluster number: %d\n",fat_ent->curCluster);
			return 0;
		}
		lba=(fat_ent->curCluster-2)*secPerClu+LBAData+fat_ent->sectorNumber;
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
		if(fat_ent->isRootDir && fatType !=32 )
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

	if(fat_ent->isRootDir && fatType !=32)
	{
		if( fat_ent->sectorNumber > rootSize)
		{
			fat_ent->eof_disk=true;
		}
		fat_ent->cacheoffset=0;
		return 1;
	}


	if(fat_ent->sectorNumber>=secPerClu) // need to change cluster
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

	if(fat_ent->isRootDir && fatType !=32)
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
			fat_ent->sectorNumber=secPerClu-1;
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
	int lba=(cluster-2)*secPerClu+LBAData;

	for(secNum=0;secNum<secPerClu;secNum++)
		if((c=ataWriteSectorsA(lba+secNum,1, cache))!=ATA_ERROR_NONE)
		{
			debug("error writing sector (ataerror=%d)\n",c);
			break;
		}

	if(secNum<secPerClu)
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
	if(BUFFER_SIZE<=fat_ent->cacheoffset) // let's move to next cluster
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

	if(BUFFER_SIZE-fat_ent->cacheoffset<FAT_ENTRY_SIZE)
	{
		if(write)
		{
			memcpy(&fat_ent->cache[fat_ent->cacheoffset],entry,BUFFER_SIZE-fat_ent->cacheoffset);
			if(!fatRWSector(fat_ent,true))
			{
				debug("[fatRWEntry] error writing cur sector\n");
				return 0;
			}
		}
		else
		{
			memcpy(entry,&fat_ent->cache[fat_ent->cacheoffset],BUFFER_SIZE-fat_ent->cacheoffset);
		}

		pos=BUFFER_SIZE-fat_ent->cacheoffset;
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
	int nxtOffset=BUFFER_SIZE-(FAT_ENTRY_SIZE-fat_ent->cacheoffset);
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

	// is this the last entry:

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
		if(fatType != 32)
			fat_ent->startCluster=-1;
		else
			fat_ent->startCluster=rootClu;
	}
	else
	{
		fat_ent->startCluster=startCluster;
		fat_ent->isRootDir=false;
	}

	fat_ent->curCluster=-1;
	fat_ent->cacheoffset=0;
	fat_ent->eof_disk=false;

	fatNxtSector(fat_ent,false);
	fatRWSector(fat_ent,false);

	printBuffer(fat_ent->cache,512);
}


//******************************************************
// getClusterSize
//
//******************************************************
int getClusterSize() {
    return secPerClu*SECTOR_SIZE;
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


// not working with FAT 12
/*int chkFAT()
{
	int c,sector,cluster,count;

	//debug("max cluster:%d\n",(fatSize*SECTOR_SIZE)/4);

	count=0;
	struct fatCache myFAT1;

	struct fatCache myFAT2;


	for(sector=0;sector<fatSize;sector++)
	{
		c = ataReadSectorsA(LBAFat1+sector, 1, (char*) myFAT1.fatBuffer);
		if (c!=ATA_ERROR_NONE)
		{
			debug("countFreeCluster error in FAT1: ata:%d\n",c);
			return -1;
		}

		c = ataReadSectorsA(LBAFat2+sector, 1, (char*) myFAT2.fatBuffer);
		if (c!=ATA_ERROR_NONE)
		{
			debug("countFreeCluster error in FAT2: ata:%d\n",c);
			return -1;
		}

		for(cluster=0;cluster<(SECTOR_SIZE/4);cluster++)
			if(myFAT1.fatBuffer[cluster] != myFAT2.fatBuffer[cluster])
				count++;
	}


	return count;
}*/

///////////////////////////////////////////////////////////////////////////////////
// from initial fat.c

int bootRead(int addr, int n) {
    int v=0;
    int c=0;
    for (c=0;c<n;c++) {
        v = (v << 8) | boot[addr + n - 1 - c];
    }
    return v;
}

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
        buffer = buffer + (secPerClu*512);      // Move along,
        cluster = fatTrace(cluster);            // Trace the fat
        if (cluster<0) return cluster;          // ATA Error!
        if (isEOChain(cluster)) return 0;       // End of chain...
    }
	return ATA_ERROR_NONE;
}

int getRootClu() {
    return rootClu;
}
