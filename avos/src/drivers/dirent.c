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
#include "stdlib.h"

#include <dirent.h>
#include <debug.h>

static DIR opendirs[MAX_OPEN_DIRS];

//******************************************************
// inidir
// initialize opendirs array
//
//******************************************************
void inidir()
{
	int dd;
	for ( dd=0; dd<MAX_OPEN_DIRS; dd++ )
		opendirs[dd].busy=false;
}

int opendir(const char* name)
{
    char namecopy[MAX_PATH];
    char * part;
    char * end;
    struct dirent * entry;
    int dd;

    for ( dd=0; dd<MAX_OPEN_DIRS; dd++ )
        if ( !opendirs[dd].busy )
            break;

    if ( dd == MAX_OPEN_DIRS ) {
		debug("Too many dir open\n");
        return -1;
    }

    opendirs[dd].busy = true;

    if ( name[0] != '/' ) {
        debug("Only absolute paths supported\n");
        opendirs[dd].busy = false;
        return -1;
    }

   fatOpendir(&(opendirs[dd].fat_ent), -1);
   opendirs[dd].attribute=0;

    strncpy(namecopy,name,sizeof(namecopy));
    namecopy[sizeof(namecopy)-1] = 0;


    for ( part = strtok_r(namecopy, "/", end); part!=NULL ;part = strtok_r(NULL, "/", end))
	{
		while (1)
		{
            if ((entry=readdir(dd))==0)
			{
                opendirs[dd].busy = false;
				debug("Can't find dir %s\n",name);
                return -1;
          	}

            if ( (entry->attribute & FAT_ATTR_DIR) && (strcasecmp(part, entry->entryName) == 0))
			{
				fatOpendir(&(opendirs[dd].fat_ent), entry->startCluster);
				opendirs[dd].attribute=entry->attribute;
                break;
            }
        }
    }

	strncpy(opendirs[dd].name,name,sizeof(namecopy));

    return dd;
}

//******************************************************
// readdir
// read next dir entry
//
// returns NULL on error
// returns a pointer on the dir entry
//******************************************************
struct dirent* readdir(int dd)
{
	DIR* dir=&(opendirs[dd]);
	struct fatent * fat_ent=&dir->fat_ent;
	struct dirent * theent = &(dir->theent);
	struct dirEntry entry;

	bool done=false;

	while(!done)
	{
		if(!fatNxtEntry(fat_ent,&entry))
			break;

		int retVal=fatValidateEntry(&entry);

		//debug("entry type: %d\n",retVal);

		if(retVal != EMPTY_ENTRY && retVal != BAD_ENTRY)
		{
			done=true;
			if(retVal == END_ENTRY || fat_ent->eof_disk)
			{
				theent=NULL;
			}
			else
			{
				fatGetData(theent,&entry);
				theent->dirCluster=fat_ent->startCluster;
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

//******************************************************
// closedir
//
//******************************************************
void closedir(int dd)
{
	DIR* dir=&(opendirs[dd]);
    dir->busy=false;
}

//******************************************************
// createEntry
//
// type= T_DIR or T_FILE
//******************************************************
int createEntry(const char * pathname, int type,struct dirent * ent) // no test made if file already exists
{

	if(type != T_DIR) // only support file creation
	{
		int dir;
		char* name;

		name=strrchr(pathname+1,'/');
		if ( name )
		{
			*name = 0;
			dir = opendir((char*)pathname);
			*name = '/';
			name++;
		}
		else
		{
			dir = opendir("/");
			name = (char*)pathname+1;
		}

		if (dir<0) {
			debug("Failed to open dir %s\n",pathname);
			return 0;
		}

		char fatName[NAME_SIZE+EXT_SIZE];

		createDosName(name,fatName);

		int res=fatCreateEntry(&opendirs[dir].fat_ent,ent,fatName);

		closedir(dir);

		return res;

	}
	else
	{
		debug("createEntry error: creating dir not supported\n");
		return 0;
	}
}

//******************************************************
// removeEntry
//
//******************************************************
int removeEntry(int dir)
{
	return fatRemoveEntry(&opendirs[dir].fat_ent);
}

