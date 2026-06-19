#include <stdio.h>
#include <string.h>

#include "ata.h"
#include "fat.h"
#include "dir.h"
#include "file.h"
#include "terminal.h"

int iniDisk()
{
	terminalPutsCT("Initialising ATA...                        ",0xfbfb);

	ataIni();

	terminalPutsCT("[ DONE ]\n",0xffff);

	terminalPutsCT("Identifying hard drive...                  ",0xfbfb);

	if (ataIdent()!=0)
	{
		terminalPutsCT("[ FAILED ]\n",0xffff);
		return 1;
	}

	terminalPutsCT("[ DONE ]\n",0xffff);

	showAtaInfo();

	terminalPutsCT("Reading MBR...                        ",0xfbfb);

	if ( ataReadMBR()!=0)
	{
		terminalPutsCT("[ FAILED ]\n",0xffff);
		return 1;
	}

	terminalPutsCT("[ DONE ]\n",0xffff);

	showPartitionInfo();

	terminalPutsCT("Reading BootSector (Partition 1)...        ",0xfbfb);

	if ( fatInit(getPartition(0)->start)!=0)
	{
		terminalPutsCT("[ FAILED ]\n",0xffff);
		return 1;
	}

	terminalPutsT("\n");

	// ini dir and file struct

	iniDir();
	iniFile();

	return 0;
}


