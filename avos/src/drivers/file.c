#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "uart.h"

static FILE openfiles[MAX_OPEN_FILES];

void inifile()
{
	int dd;
	for ( dd=0; dd<MAX_OPEN_FILES; dd++ )
        openfiles[dd].busy=false;
}

int fopen(const char* pathname)
{
    int dir;

    int fd;
    char* name;
    FILE * file = NULL;

    if ( pathname[0] != '/' ) {
        uartOuts("Only absolute paths supported right now\n");
        return -1;
    }

    for ( fd=0; fd<MAX_OPEN_FILES; fd++ )
        if ( !openfiles[fd].busy )
            break;

    if ( fd == MAX_OPEN_FILES ) {
		uartOuts("Too many files open\n");
        return -2;
    }

	file=&(openfiles[fd]);

    file->busy = true;

    // locate filename
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
        uartOuts("Failed to open dir\n");
        file->busy = false;
        return -4;
    }

	struct dirent* entry;

    // scan dir for name
    while ((entry = readdir(dir))!=NULL) {
        if (strcasecmp(name, entry->entryName) == 0)
		{
            file->size = entry->size;
            file->attr = entry->attribute;
			file->count = 0;
            break;
        }
    }

    if ( entry ==NULL)
	{
		uartOuts("Failed to find file\n");
		file->busy = false;
		return -5;
    }

    closedir(dir);

	file->fat_ent.startCluster=entry->startcluster;
    file->fat_ent.curCluster = 0;
	file->fat_ent.cacheoffset = 0;
	file->fat_ent.eof_disk=false;
	file->eof=false;

	fatfatNxtSector(&file->fat_ent);

    return fd;
}

void fclose(int fd)
{
	openfiles[fd].busy=false;
}

int fsize(int fd)
{
	if(openfiles[fd].busy)
	{
		return openfiles[fd].size;
	}
	else
		return -1;
}

int ftell(int fd)
{
	if(openfiles[fd].busy)
	{
		return openfiles[fd].count;
	}
	else
		return -1;
}

int fread(int fd, void* buf, int count)
{
	FILE * file;
	struct fatent * fat_ent;
	int pos=0;

	if(openfiles[fd].busy)
	{
		if(count ==0)
		{
			return 0;
		}

		int cpySize;
		file=&(openfiles[fd]);
		fat_ent=&file->fat_ent;

		if(count+file->count>file->size)
			count=file->size-file->count;

		if(file->eof)
			return -1;

		while(fat_ent->cacheoffset+count > BUFFER_SIZE)
		{
			cpySize=BUFFER_SIZE-fat_ent->cacheoffset;
			memcpy(&buf[pos],&(fat_ent->cache[fat_ent->cacheoffset]),cpySize);
			count -= cpySize;
			pos+=cpySize;
			if(fat_ent->eof_disk)
			{
				file->eof=true;
				return pos;
			}
			fatfatNxtSector(fat_ent);
		}

		if(fat_ent->cacheoffset+count <=BUFFER_SIZE && count !=0)
		{
			memcpy(&buf[pos],&(fat_ent->cache[fat_ent->cacheoffset]),count);
			fat_ent->cacheoffset+= count;
			pos+=count;
		}

	}
	else
	{
		uartOuts("File not open yet\n");
		return -1;
	}

	file->count+=pos;
	return pos;
}

int mySetPosF(int fd,long int pos)
{
	FILE * file=&(openfiles[fd]);
	struct fatent * fat_ent=&file->fat_ent;

	if(pos == file->count)
		return 1;

	int bufferPos = file->count - fat_ent->cacheoffset;

	if(pos >= bufferPos && pos < bufferPos+BUFFER_SIZE) // we stay in the buffer
	{
		fat_ent->cacheoffset=pos-bufferPos;
		file->count=pos;
		return 1;
	}

	if(pos < file->count) // we need to start from the beginning
	{
		file->fat_ent.curCluster = 0;
		file->count=0;
		fatfatNxtSector(&file->fat_ent);
	}
	else
	{
		file->count+=BUFFER_SIZE-fat_ent->cacheoffset;
		fatfatNxtSector(&file->fat_ent);
	}

	while((file->count + BUFFER_SIZE) < pos)
	{
		fatfatNxtSector(&file->fat_ent);
		file->count +=BUFFER_SIZE;
	}


	fat_ent->cacheoffset=pos - file->count;
	file->count=pos;
	return 1;



}

int fseek(int fd, long int offset, int whence)
{
	if(openfiles[fd].busy)
	{
		FILE * file=&(openfiles[fd]);
		int newpos;

		if(whence == SEEK_END)
		{
			if(offset > 0 || (offset + file->size)<0)
				return 0;
			else
				newpos = offset + file->size;
		}

		if(whence == SEEK_SET)
		{
			if (offset < 0 || (offset > file->size))
				return 0;
			else
				newpos = offset;
		}

		if(whence == SEEK_CUR)
		{
			if((file->count+offset)<0 || (file->count+offset) > file->size)
				return 0;
			else
				newpos = file->count+offset;
		}

		return mySetPosF(fd,newpos);
	}
	else
		return 0;
}

int fsetpos(int fd,long int pos)
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

int rewind(int fd)
{
	if(openfiles[fd].busy)
	{
		return mySetPosF(fd,0);
	}
	else
		return 0;
}

bool feof(int fd)
{
	if(openfiles[fd].busy)
	{
		return openfiles[fd].eof;
	}
	else
		return true;
}


