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

int launchFile(char * fileN)
{
	int curFile;
	terminalSetPosT(0, 35);
	terminalPutsCT("loading file ", 0xfbfb);

	if((curFile=openF(fileN))>=0)
	{
		char buffer[512];
		u32 offset=0;

		while((readF(curFile,buffer,512))>0)
		{
   			codecpy(buffer,offset,512);
			offset+=512;
			if(offset%10240==0)
				terminalPutsCT(".", 0xfbfb);
		}

		closeF(curFile);

  		startProg(1,fileN);
		return 1;
	}
	else
	{
		terminalPutsCT("Error loading file\n", 0xfbfb);
		return -1;
	}
}

int openF(const char* pathname)
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
			file->count = 0;
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

void closeF(int fd)
{
	openfiles[fd].busy=false;
}

int sizeF(int fd)
{
	if(openfiles[fd].busy)
	{
		return openfiles[fd].size;
	}
	else
		return -1;
}

int tellF(int fd)
{
	if(openfiles[fd].busy)
	{
		return openfiles[fd].count;
	}
	else
		return -1;
}

int readF(int fd, void* buf, int count)
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

		if(count+file->count>file->size)
			count=file->size-file->count;

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

	file->count+=pos;
	return pos;
}

int mySetPosF(int fd,long int pos)
{


	myFILE * file=&(openfiles[fd]);
	struct fatent * fat_ent=&file->fat_ent;

	char txt[80];
	snprintf(txt,80,"posactu: %d Cacheoffset: %d newpos: %d\n",file->count,fat_ent->cacheoffset,pos);
	uartOutsT(txt);

	if(pos == file->count)
		return 1;

	int bufferPos = file->count - fat_ent->cacheoffset;

	snprintf(txt,80,"bufferpos: %d\n",bufferPos);
	uartOutsT(txt);


	if(pos >= bufferPos && pos < bufferPos+512) // we stay in the buffer
	{
		fat_ent->cacheoffset=pos-bufferPos;
		file->count=pos;

		snprintf(txt,80,"same buffer, cacheoffset: %d pos: %d\n",fat_ent->cacheoffset,file->count);
		uartOutsT(txt);

		return 1;
	}

	if(pos < file->count) // we need to start from the beginning
	{
		uartOutsT("go to the beg of file\n");
		file->fat_ent.curCluster = 0;
		file->count=0;
		nxtSector(&file->fat_ent);
	}
	else
	{
		file->count+=512-fat_ent->cacheoffset;

		snprintf(txt,80,"let's move to nxt sector, new pos: %d\n",file->count);
		uartOutsT(txt);
		nxtSector(&file->fat_ent);
	}

	while((file->count + 512) < pos)
	{
		nxtSector(&file->fat_ent);
		file->count +=512;
		snprintf(txt,80,"loop, new pos: %d\n",file->count);
		uartOutsT(txt);
	}


	fat_ent->cacheoffset=pos - file->count;
	file->count=pos;

	snprintf(txt,80,"end loop, new pos: %d, new cacheoffset:%d\n",file->count,fat_ent->cacheoffset);
	uartOutsT(txt);

	return 1;



}

int seekF(int fd, long int offset, int whence)
{
	if(openfiles[fd].busy)
	{
		myFILE * file=&(openfiles[fd]);
		int newpos;

		if(whence == SEEK_END)
		{
			uartOutsT("SEEK_END\n");
			if(offset > 0 || (offset + file->size)<0)
				return 0;
			else
				newpos = offset + file->size;
		}

		if(whence == SEEK_SET)
		{
			uartOutsT("SEEK_SET\n");
			if (offset < 0 || (offset > file->size))
				return 0;
			else
				newpos = offset;
		}

		if(whence == SEEK_CUR)
		{
			uartOutsT("SEEK_CUR\n");
			if((file->count+offset)<0 || (file->count+offset) > file->size)
				return 0;
			else
				newpos = file->count+offset;
		}

		char txt[80];
		snprintf(txt,80,"offset: %d posactu: %d newpos: %d\n",offset,file->count,newpos);
		uartOutsT(txt);

		return mySetPosF(fd,newpos);

	}
	else
		return 0;
}

int setposF(int fd,long int pos)
{
	if(openfiles[fd].busy)
	{
		if (pos <0 || pos > openfiles[fd].size)
			return 0;
		return mySetPosF(fd,pos);
	}
	else
		return 0;
}



bool eofF(int fd)
{
	if(openfiles[fd].busy)
	{
		return openfiles[fd].eof;
	}
	else
		return true;
}


