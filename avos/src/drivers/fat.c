#include <ata.h>
#include <fat.h>
#include <uart.h>

static char boot[512];
static int fatSize;
static int numFats;
static int rsvdSecCnt;
static int rootClu;
static int secPerClu;

static int LBAFat1;
static int LBAData;

static int fatCacheLBA=-1;
static int fatCache[128];                     // 1 sector fatcache

char hex8[] = "xxxxxxxx";

int getRootClu() {
    return rootClu;
}

int fatDirFilter(struct dirEntry dirIn[], struct dirEntry dirOut[], int n) {
    int cpin=0,cpo=0;

    for (cpin=0;cpin<n;cpin++) {
        if (dirIn[cpin].name[0]==0) {
            break;
        } else if (dirIn[cpin].name[0]!=0xe5) {
            if (!(dirIn[cpin].attr & FAT_ATTR_VOLUME_ID) &&
               (dirIn[cpin].attr!= FAT_ATTR_LONG_NAME)) {
                dirOut[cpo++] = dirIn[cpin];
            }
        }
        
    }
    return cpo;
}


int fatInit(u32 lba) {
    int c;
    
    fatCacheLBA=-1;
    
    c=ataReadSectors(lba, 1, boot);      // Read the bootsector

    if (c!=ATA_ERROR_NONE) {
        uartOuts("ataReadSectors returned error!\n");
        return c;
    }

    fatSize = bootRead(36, 4);
    stringPutHex(hex8, fatSize, 8);
    uartOuts("[fat.c] FATSz32 = ");
    uartOuts(hex8);
    uartOuts("\n");

    numFats = bootRead(16, 1);
    stringPutHex(hex8, numFats, 8);
    uartOuts("[fat.c] NumFATs = ");
    uartOuts(hex8);
    uartOuts("\n");

    rsvdSecCnt = bootRead(14, 2);
    stringPutHex(hex8, rsvdSecCnt, 8);
    uartOuts("[fat.c] rsvdSecCnt = ");
    uartOuts(hex8);
    uartOuts("\n");

    LBAFat1 = lba + rsvdSecCnt;

    rootClu = bootRead(44, 4);
    stringPutHex(hex8, rootClu, 8);
    uartOuts("[fat.c] rootClu = ");
    uartOuts(hex8);
    uartOuts("\n");

    LBAData = LBAFat1 + (fatSize * numFats);
    
    secPerClu = bootRead(13, 1);
    stringPutHex(hex8, secPerClu, 8);
    uartOuts("[fat.c] secPerClu = ");
    uartOuts(hex8);
    uartOuts("\n");

    stringPutHex(hex8, LBAFat1, 8);
    uartOuts("[fat.c] LBAFat1 = ");
    uartOuts(hex8);
    uartOuts("\n");

    stringPutHex(hex8, LBAData, 8);
    uartOuts("[fat.c] LBAData = ");
    uartOuts(hex8);
    uartOuts("\n");
    
    return 0;
}

int fatReadCluster(int cluster, char* buffer) {
    int sec = LBAData + ((cluster - 2) * secPerClu);
    return ataReadSectors(sec, secPerClu, buffer);
    
    // Use safe sector by sector mode for now (SLOW....) eugh need to fix multi
    //int c,i;
    //int sec = LBAData + (cluster * secPerClu);
    //for (i=0;i<secPerClu;i++) {
    //    c = ataReadSectors(sec, 1, buffer);
    //    if (c!=ATA_ERROR_NONE) return c;
    //    sec++;
    //    buffer+=512;
    //}
    //return c;
}


int fatTrace(int cluster) {
    int lba, c;
    lba = LBAFat1 + ((cluster*4) >> 9);   // 4 - 32bit, 9-512 byte sectors.

    stringPutHex(hex8, lba, 8);
    uartOuts("[fat.c] fatTrace.lba = ");
    uartOuts(hex8);
    uartOuts("\n");
    
    if (fatCacheLBA != lba) {
        c = ataReadSectors(lba, 1, (char*) fatCache);    
        if (c!=ATA_ERROR_NONE) return c;    // Error!
        fatCacheLBA = lba;
    }
    
    return fatCache[cluster & 127];
}

int fatfatNxtSector(struct fatent * fat_ent)
{

	if(fat_ent->curCluster==0) // we start a new fat_ent
	{
		fat_ent->curCluster=fat_ent->startCluster;
		fat_ent->fatoffset=(fat_ent->curCluster-2)*secPerClu+LBAData;
		fat_ent->sectorNumber=0;
	}


	ataReadSectors(fat_ent->fatoffset+fat_ent->sectorNumber, 1, fat_ent->cache);

	fat_ent->sectorNumber++;

	if(fat_ent->sectorNumber>=secPerClu) // need to change cluster
	{
		fat_ent->curCluster=fatTrace(fat_ent->curCluster);
		if(fat_ent->curCluster >= FAT_CHAIN_END)
		{
			fat_ent->eof_disk=true;
		}
		else
		{
			fat_ent->fatoffset=(fat_ent->curCluster-2)*secPerClu+LBAData;
			fat_ent->sectorNumber=0;
		}
	}

	fat_ent->cacheoffset=0;
	return 0;
}

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

void fatGetName(char * name,struct dirEntry * entry)
{
	int i;
	for(i=0;i<NAME_SIZE;i++)
		name[i]=entry->name[i];
	name[i]=0x0;
}

void fatGetEntryName(char * entryName,struct dirEntry * entry)
{
	int i,j;

	for(i=0;i<NAME_SIZE;i++)
		if(entry->name[i] == ' ')
			break;
		else
			entryName[i]=entry->name[i];

	if(entry->ext[0]!=' ')
	{
		entryName[i]='.';
		i++;

		for(j=0;j<EXT_SIZE;j++)
			if(entry->ext[j] == ' ')
				break;
			else
			{
				entryName[i]=entry->ext[j];
				i++;
			}
	}
	entryName[i]=0x0;
}

void fatGetExt(char * ext,struct dirEntry * entry)
{
	int i;
	for(i=0;i<EXT_SIZE;i++)
		ext[i]=entry->ext[i];
	ext[i]=0x0;
}

int fatGetAtr(struct dirEntry * entry)
{
	return entry->attr;
}

int fatGetstrtClu(struct dirEntry * entry)
{
	int val = entry->fatCluHI << 16;
	return val + entry->fatCluLO;
}

int fatGetSize(struct dirEntry * entry)
{
	return entry->size;
}

void fatOpendir(struct fatent * fat_ent,int startCluster)
{
	if(startCluster==-1)
		fat_ent->startCluster=rootClu;
	else
		fat_ent->startCluster=startCluster;

	fat_ent->curCluster=0;
	fat_ent->cacheoffset=0;
	fat_ent->eof_disk=false;

	fatfatNxtSector(fat_ent);
}


int fatReadFile(int cluster, char* buffer) {
    int c;
    while(1) {
        stringPutHex(hex8, cluster, 8);
        uartOuts("[fat.c] reading cluster ");
        uartOuts(hex8);
        uartOuts("\n");

        c = fatReadCluster(cluster, buffer);        // Read data...
        if (c!=ATA_ERROR_NONE) return c;
        buffer = buffer + (secPerClu*512);      // Move along,
        cluster = fatTrace(cluster);            // Trace the fat
        if (cluster<0) return cluster;          // ATA Error!
        if (cluster>FAT_CHAIN_END) return 0;       // End of chain...
    }
}

int fatloadFile(char * fileN)
{
    int curFile;

	curFile=fopen(fileN);
	debug("openFile(): %d\n",curFile);

	if(curFile>=0)
	{

		int offset=0;
		char * buffer=(char*) 0x03000000;

		while((fread(curFile,&buffer[offset],secPerClu*SECTOR_SIZE))>0)
			offset+=secPerClu*SECTOR_SIZE;
		fclose(curFile);
		return 1;
	}
	else
	{
		uartOuts("Error loading file\n");
		return 0;
	}
}


int bootRead(int addr, int n) {
    int v=0;
    int c=0;
    for (c=0;c<n;c++) {
        v = (v << 8) | boot[addr + n - 1 - c]; 
    }
    return v;
}

