#include <string.h>
#include <stdio.h>

#include "ata.h"
#include "terminal.h"
#include "hex.h"


void swapChar(char * txt,int size);

static char buffer[512];
static struct ataInfo info;
static struct partition partList[4];

void ataIni()
{
	ataInit();

}

int ataIdent()
{

	int res;

	if((res=ataIdentify( buffer ))!=0)
	{
		if(res == 1)
			terminalPutsT("Erreur ata_identify, not rdy\n");
		else
			if(res == 2)
				terminalPutsT("Erreur ata_identify, Xfer not started\n");
		return res;
	}

	strncpy(info.serial, &buffer[20], 20);
	swapChar(info.serial,20);
	info.serial[20]=0x0;
	strncpy(info.firmware, &buffer[46], 8);
	swapChar(info.firmware,8);
	info.firmware[8]=0x0;
	strncpy(info.model, &buffer[54], 40);
	swapChar(info.model,40);
	info.model[40]=0x0;

	return 0;
}

void showAtaInfo()
{
	char txt[80];
	snprintf (txt, 80, "Serial:   %s\n",info.serial);
	terminalPutsCT(txt,0xfdfd);
	snprintf (txt, 80, "Firmware: %s\n",info.firmware);
	terminalPutsCT(txt,0xfdfd);
	snprintf (txt, 80, "Model:    %s\n",info.model);
	terminalPutsCT(txt,0xfdfd);
}

void swapChar(char * txt,int size)
{
	int i;
	for(i=0;i<size/2;i++)
	{
		char c=txt[2*i];
		txt[2*i]=txt[2*i+1];
		txt[2*i+1]=c;
	}
}

int ataReadMBR()
{
	int res,i;
	if((res=ataReadLBA( buffer, 1, 1 ))!=0)
	{
		if(res == 1)
			terminalPutsT("Error ataReadMBR, not rdy\n");
		else
			if(res == 2)
				terminalPutsT("Error ataReadMBR, Xfer not started\n");
		return res;
	}

	int offset,j;
	char txt[80];

	char * fatStr[]={"zero", "FAT12", "FAT16<32MB", "ExtMSDOS", "FAT16>32MB", "FAT32<2048GB", "FAT32-13hExt",
					"FAT16>32MB-13hExt", "ExtMSDOS-13hExt"};
	int fatId[]={0x00, 0x01, 0x04, 0x05, 0x06, 0x0B, 0x0C, 0x0E, 0x0F};


	for(i=0;i<4;i++)
	{
		offset=446+i*16;
		partList[i].type=buffer[offset+4];
		partList[i].start=read4T(&buffer[offset+8]);
		partList[i].size=read4T(&buffer[offset+12]);
		j=0;
		while(j<9 && fatId[j]!=partList[i].type) j++;
		if(j<9)
			strcpy(partList[i].strType,fatStr[j]);
		else
		{
			snprintf (txt, 80, "Error: partition type not supported: %x\n",partList[i].type);
			terminalPutsT(txt);
			if(i==0)
			{
				terminalPutsT("Halt\n");
				return 1;
			}
		}


	}

	/*if(buffer[510]!=0x55 || buffer[511]!=0xAA)
	{
		snprintf (txt, 80, "Error: MBR wrong, missing 55AA at the end of MBR: %x %x\n",buffer[510],buffer[511]);
		terminalPutsT(txt);
		return 1;
	}*/

	return 0;
}

void showPartitionInfo()
{
	char txt[80];
	int i;

	for(i=0;i<4;i++)
	{
		snprintf (txt, 80, "Partition: %d %x %s %x %x\n",i,partList[i].type,partList[i].strType,
			partList[i].start,partList[i].size);
		terminalPutsCT(txt,0xfdfd);
	}
}

struct partition * getPartition(int i)
{
	return &partList[i];
}
