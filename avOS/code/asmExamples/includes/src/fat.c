#include <stdbool.h>
#include <stdio.h>

#include "fat.h"
#include "ata.h"
#include "terminal.h"

int fatInit(int lba)
{
	int res;
	if((res=fatInitT(lba))!=0)
	{
		if(res == 1)
			terminalPutsT("Erreur ataReadMBR, not rdy\n");
		else
			if(res == 2)
				terminalPutsT("Erreur ataReadMBR, Xfer not started\n");
		return res;
	}
	return 0;
}

int nxtSector(struct fatent * fat_ent)
{

	if(fat_ent->curCluster==0) // we start a new fat_ent
	{
		fat_ent->curCluster=fat_ent->startCluster;
		fat_ent->fatoffset=(fat_ent->curCluster-2)*getFatSecPerCluT()+getFatLBADATAT();
		fat_ent->sectorNumber=0;
	}


	ataReadLBA(fat_ent->cache,fat_ent->fatoffset+fat_ent->sectorNumber,1);

	fat_ent->sectorNumber++;

	if(fat_ent->sectorNumber>=getFatSecPerCluT()) // need to change cluster
	{
		fat_ent->curCluster=fatTraceCCT(fat_ent->curCluster);
		if(fat_ent->curCluster >= 0x0ffffff8)
		{
			fat_ent->eof_disk=true;
		}
		else
		{
			fat_ent->fatoffset=(fat_ent->curCluster-2)*getFatSecPerCluT()+getFatLBADATAT();
			fat_ent->sectorNumber=0;
		}
	}

	fat_ent->cacheoffset=0;
	return 0;
}


