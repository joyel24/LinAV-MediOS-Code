#include <string.h>
#include <stdlib.h>

#include "dirent.h"
#include "uart.h"
#include "debug.h"

static DIR opendirs[MAX_OPEN_DIRS];

void inidir()
{
	int dd;
	for ( dd=0; dd<MAX_OPEN_DIRS; dd++ )
		opendirs[dd].busy=false;
}

int opendir(char* name)
{
    char namecopy[MAX_PATH];
    char* part;
    char* end;
    struct dirent * entry;
    int dd;

    for ( dd=0; dd<MAX_OPEN_DIRS; dd++ )
        if ( !opendirs[dd].busy )
            break;

    if ( dd == MAX_OPEN_DIRS ) {
		uartOutsA("Too many dir open\n");
        return -1;
    }

    opendirs[dd].busy = true;

    if ( name[0] != '/' ) {
        uartOutsA("Only absolute paths supported right now\n");
        opendirs[dd].busy = false;
        return -1;
    }

   fatOpendir(&(opendirs[dd].fat_ent), -1);
   opendirs[dd].attribute=0;

    strncpy(namecopy,name,sizeof(namecopy));
    namecopy[sizeof(namecopy)-1] = 0;

    for ( part = strtok_r(namecopy, "/", &end); part;part = strtok_r(0, "/", &end))
	{
		while (1)
		{
            if ((entry=readdir(dd))==0)
			{
                opendirs[dd].busy = false;
				uartOutsA("Can't find the dir\n");
                return -1;
          	}

            if ( (entry->attribute & FAT_ATTR_DIR) && (strcasecmp(part, entry->entryName) == 0))
			{
				fatOpendir(&(opendirs[dd].fat_ent), entry->startcluster);
				opendirs[dd].attribute=entry->attribute;
                break;
            }
        }
    }

	strncpy(opendirs[dd].name,name,sizeof(namecopy));

    return dd;
}

struct dirent* readdir(int dd)
{
	DIR* dir=&(opendirs[dd]);
	struct fatent * fat_ent=&dir->fat_ent;
	struct dirent * theent = &(dir->theent);
	struct dirEntry entry;
	int count=FAT_ENTRY_SIZE;
	int pos=0;

	bool done=false;

	while(!done)
	{
		if(BUFFER_SIZE-fat_ent->cacheoffset<=FAT_ENTRY_SIZE)
		{
			memcpy(&entry,&fat_ent->cache[fat_ent->cacheoffset],BUFFER_SIZE-fat_ent->cacheoffset);
			if(fat_ent->eof_disk)
			{
				uartOutsA("End of cluster chain, before end of direntry\n");
				break;
			}
			pos=BUFFER_SIZE-fat_ent->cacheoffset;
			count=FAT_ENTRY_SIZE-pos;
			fatNxtSector(fat_ent);
		}

		if(count>0)
		{
			memcpy(&entry,&(fat_ent->cache[fat_ent->cacheoffset]),count);
			fat_ent->cacheoffset+= count;
		}

		int retVal=fatValidateEntry(&entry);
		//debug("fatValidateEntry: (a=%d,n0=%d) -> %d\n",entry.attr,entry.name[0],retVal);

		if(retVal == EMPTY_ENTRY || retVal == BAD_ENTRY)
		{
			count=FAT_ENTRY_SIZE;
			pos=0;
		}
		else
		{
			done=true;
			if(retVal == END_ENTRY)
			{
				theent=NULL;
			}
			else
			{
				fatGetName(theent->name,&entry);
				fatGetEntryName(theent->entryName,&entry);
				fatGetExt(theent->ext,&entry);
				theent->attribute=fatGetAtr(&entry);
				theent->startcluster=fatGetstrtClu(&entry);
				theent->size=fatGetSize(&entry);
			}
		}
	}

	if(!done)
	{
		return NULL;
	}
	else
		return theent;
}

void closedir(int dd)
{
	DIR* dir=&(opendirs[dd]);
    dir->busy=false;
}
