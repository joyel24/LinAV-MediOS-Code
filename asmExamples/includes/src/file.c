#include <stdio.h>
#include <string.h>

#include "file.h"
#include "terminal.h"
#include "uart.h"

static myFILE openfiles[MAX_OPEN_FILES];

void iniFile()
{
	int dd;
	for ( dd=0; dd<MAX_OPEN_FILES; dd++ )
        openfiles[dd].busy=false;
}

int createFileinRoot(char * fileN)
{
	int dir=openDir("/");
	return addFile(dir,fileN);
}

int launchFile(char * fileN)
{
	int curFile;
	terminalSetPosT(0, 35);
	terminalPutsCT("loading file ", 0xfbfb);

	if((curFile=open(fileN))>=0)
	{
		char buffer[512];
		u32 offset=0;

		while((read(curFile,buffer,512))>0)
		{
   			codecpy(buffer,offset,512);
			offset+=512;
			if(offset%10240==0)
				terminalPutsCT(".", 0xfbfb);
		}

		close(curFile);

  		startProg(1,fileN);
		return 1;
	}
	else
	{
		terminalPutsCT("Error loading file\n", 0xfbfb);
		return -1;
	}
}

int open(const char* pathname)
{
    int dir;
    struct dirent* entry;

    int fd;
    char* name;
    myFILE * file = NULL;

    if ( pathname[0] != '/' ) {
        uartOutsT("Only absolute paths supported right now\n");
        return -1;
    }

    for ( fd=0; fd<MAX_OPEN_FILES; fd++ )
        if ( !openfiles[fd].busy )
            break;

    if ( fd == MAX_OPEN_FILES ) {
		uartOutsT("Too many files open\n");
        return -2;
    }

	file=&(openfiles[fd]);

    file->busy = true;

    // locate filename
    name=strrchr(pathname+1,'/');
    if ( name )
	{
        *name = 0;
        dir = openDir((char*)pathname);
        *name = '/';
        name++;
    }
    else
	{
        dir = openDir("/");
        name = (char*)pathname+1;
    }

    if (dir<0) {
        uartOutsT("Failed to open dir\n");
        file->busy = false;
        return -4;
    }

    // scan dir for name
    while ((entry = readDir(dir))!=NULL) {
        if (strcasecmp(name, entry->name) == 0)
		{
            file->size = entry->size;
            file->attr = entry->attribute;
            break;
        }
    }

    if ( entry ==NULL)
	{
		uartOutsT("Failed to find file\n");
		file->busy = false;
		return -5;
    }

    closeDir(dir);

	file->fat_ent.startCluster=entry->startcluster;
    file->fat_ent.curCluster = 0;
	file->fat_ent.cacheoffset = 0;
	file->fat_ent.eof_disk=false;
	file->eof=false;


	nxtSector(&file->fat_ent);

    return fd;
}

void close(int fd)
{
	openfiles[fd].busy=false;
}

int read(int fd, void* buf, int count)
{
	myFILE * file;
	struct fatent * fat_ent;
	int pos=0;

	char txt[80];


	if(openfiles[fd].busy)
	{
		if(count ==0)
		{
			uartOutsT("Count=0\n");
			return 0;
		}


		int cpySize;
		file=&(openfiles[fd]);
		fat_ent=&file->fat_ent;

		if(file->eof)
			return -1;

		while(fat_ent->cacheoffset+count > 512)
		{
			cpySize=512-fat_ent->cacheoffset;
			memcpy(&buf[pos],&(fat_ent->cache[fat_ent->cacheoffset]),cpySize);
			count -= cpySize;
			pos+=cpySize;
			if(fat_ent->eof_disk)
			{
				snprintf(txt,80,"End of file: %d cpy\n",pos);
				uartOutsT(txt);
				file->eof=true;
				return pos;
			}
			nxtSector(fat_ent);
		}

		if(fat_ent->cacheoffset+count <= 512 && count !=0)
		{
			memcpy(&buf[pos],&(fat_ent->cache[fat_ent->cacheoffset]),count);
			fat_ent->cacheoffset+= count;
			pos+=count;
		}

	}
	else
	{
		uartOutsT("File not open\n");
		return -1;
	}

	return pos;
}
