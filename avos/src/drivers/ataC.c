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

#include "stdlib.h"
#include "string.h"

#include <ata.h>
#include <debug.h>

char * fatStr[]={"zero", "FAT12", "FAT16<32MB", "ExtMSDOS", "FAT16>32MB", "FAT32<2048GB", "FAT32-LBA",
					"FAT16>32MB-LBA", "ExtMSDOS-LBA"};
int fatId[]={0x00, 0x01, 0x04, 0x05, 0x06, 0x0B, 0x0C, 0x0E, 0x0F};

struct partInfo partitions[4];

int ataReadMBR()
{
	int i,j,c,offset;
	char buffer[512];

	for(i=0;i<4;i++)
		memset(&partitions[i],0,sizeof(struct partInfo));

	if((c=ataReadSectorsA(0, 1, buffer))!=ATA_ERROR_NONE)
	{
		debug("ata error:%d\n",c);
		return 0;
	}

	for(i=0;i<4;i++)
	{
		offset=446+i*16;
		partitions[i].type=buffer[offset+4];
		partitions[i].start=buffer[offset+8] | (buffer[offset+8+1]<<8) | (buffer[offset+8+2]<<16) | (buffer[offset+8+3]<<24);
		partitions[i].size=buffer[offset+12] | (buffer[offset+12+1]<<8) | (buffer[offset+12+2]<<16) | (buffer[offset+12+3]<<24);

		if(i==0 && partitions[i].start == 0x00) // fix for buggy buffer
		{
			partitions[i].start = 0x3F;
			debug("Fix wrong mbr start to 3F\n" );
		}

		j=0;
		while(j<9 && fatId[j]!=partitions[i].type) j++;

		if(j<9)
			strcpy(partitions[i].strType,fatStr[j]);
		else
			debug("Error: partition type not supported: %x\n",partitions[i].type);
	}

	if(buffer[510]!=0x55 || buffer[511]!=0xAA)
		debug("Warnig: mbr wrong, missing 55AA at the end of mbr: %x %x\n",buffer[510],buffer[511]);

	return 1;
}

struct partInfo * getPartition(int i)
{
	return &partitions[i];
}

void printPartInfo(int i)
{
	debug("Part %d: start=%x, size=%x, type:%s (%x)\n",i,partitions[i].start,partitions[i].size,partitions[i].strType,partitions[i].type);
}
