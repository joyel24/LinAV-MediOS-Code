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

#include <file.h>
#include <debug.h>

#define DO_READ 1
#define DO_WRITE 2

static FILE openfiles[MAX_OPEN_FILES];

//******************************************************
// inifile
// initialize openfiles array
//
//******************************************************
void inifile()
{
	int dd;
	for ( dd=0; dd<MAX_OPEN_FILES; dd++ )
        openfiles[dd].busy=false;
}

//******************************************************
// fopen
// open a file based on its full path + name and mode
// mode:
// "r"		: read-only
// "w" 	: write-only (file cleared if exists)
// "a" 	: append-only
// "r+" 	: read/write on existing file
// "w+"	: read/write (file cleared if exists)
// "a+"	: read/append (write only at the end of the file)
//
// returns <0 on error
// file descriptor (int) otherwise
//******************************************************
int fopen(const char* pathname,const char * mode)
{
    int fd;
	int flags;
	if(!(flags=getFlag(mode)))
	{
		debug("[fopen] mode is wrong\n");
        return -1;
	}

    FILE * file = NULL;

    if ( pathname[0] != '/' ) {
        debug("[fopen] only absolute paths supported '%s'\n", pathname);
        return -1;
    }

    for ( fd=0; fd<MAX_OPEN_FILES; fd++ )
        if ( !openfiles[fd].busy )
            break;

    if ( fd == MAX_OPEN_FILES ) {
		debug("[fopen] too many files open\n");
        return -2;
    }

	file=&(openfiles[fd]);
	memset(file, 0, sizeof(FILE));

	file->flags = flags;
    file->busy = true;

    // locate filename

	struct dirent* entry=NULL;
	struct dirent entry2;
	memset(&entry2,0,sizeof(entry2));

	if(!findFileEntry(pathname,&entry,false))
		return -4;

	if (entry == NULL) // file does not exist
	{
		if(flags & (F_RD | F_RDP)) // file open in read-only RD+ => error
		{
			debug("[fopen] file not found %s\n",pathname);
			file->busy = false;
			return -5;
		}
		else // create a new file
		{
			if(!createEntry(pathname,T_FILE,&entry2)) // file created => ini FILE structure
			{
				debug("[fopen] failed to create file %s \n",pathname);
				file->busy = false;
				return -6;
			}

			file->size=0;
			file->attr=0;
			file->pos = 0;

			strcpy(file->name,entry2.entryName);
			file->fat_ent.startCluster=entry2.startCluster;
			file->fat_ent.curCluster = -1;
			file->fat_ent.cacheoffset = 0;
			file->fat_ent.eof_disk=false;
			file->fat_ent.isRootDir=false;
			file->fat_ent.dirCluster=entry2.dirCluster;
			file->fat_ent.fatId=entry2.fatId;
			file->eof=false;

			selectFat(file->fat_ent.fatId);

			//debug("file created: start=%d\n",file->fat_ent.startCluster);

			nxtSector(file,DO_READ);
		}
    }
	else // file exists => ini FILE structure
	{
		strcpy(file->name,entry->entryName);
		file->size = entry->size;
		file->attr = entry->attribute;
		file->pos = 0;
		file->fat_ent.startCluster=entry->startCluster;
		file->fat_ent.curCluster = -1;
		file->fat_ent.cacheoffset = 0;
		file->fat_ent.eof_disk=false;
		file->fat_ent.isRootDir=false;
		file->fat_ent.dirCluster=entry->dirCluster;
		file->fat_ent.fatId=entry->fatId;
		file->eof=false;
		file->write_done=false;

		selectFat(file->fat_ent.fatId);

		//debug("file found: %x\n",file->fat_ent.startCluster);

		nxtSector(file,DO_READ);

		if((flags & (F_WR | F_WRP)) && file->size>0) // file need to be cleared
		{
			ftruncate(fd,0);
		}
	}

	if(flags & (F_AP | F_APP)) // append => go to the end of file
		fseek(fd,0,SEEK_END);

    return fd;
}

//******************************************************
// getFlags
// transform mode
//
// returns 0 on error
// returns flag otherwise
//******************************************************
int getFlag(const char * mode)
{
	int len=strlen(mode);
	int flag=0;
	if(len<1 || len >2)
	{
		debug("[getFlag] mode length is bad (%d)\n",len);
		return 0;
	}

	switch (mode[0])
	{
		case 'r':
		flag=F_RD;
		break;
		case 'w':
		flag=F_WR;
		break;
		case 'a':
		flag=F_AP;
		break;
		default:
			debug("[getFlag] wrong mode (%s)\n",mode);
			return 0;
	}

	if(len==2)
	{
		if(mode[1] == '+')
			flag=flag<<3;
		else
		{
			debug("[getFlag] bad mode modifier (%s)\n",mode);
			return 0;
		}
	}

	return flag;
}

//******************************************************
// findFileEntry
// get file entry based on it's pathname
// if remove=true => remove dir entry
//
// returns 0 on error
//******************************************************
int findFileEntry(const char* pathname,struct dirent ** entry,bool remove)
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
        debug("[findFileEntry] failed to open dir %s\n",pathname);
        return 0;
    }

	debug("dir opened\n");

    // scan dir for name
    while ((*entry = readdir(dir))!=NULL)
	{
		if (strcasecmp(name, (*entry)->entryName) == 0)
		{
			if(remove)
			{
				struct fatent fat_ent;
				fat_ent.curCluster =(*entry)->startCluster;
				fat_ent.isRootDir=false;
				fat_ent.fatId = (*entry)->fatId;

				if(!fatTruncate(&fat_ent,true))
				{
					debug("[findFileEntry] error removing file using fatTruncate\n");
					return 0;
				}

				if(!removeEntry(dir))
				{
					debug("[findFileEntry] error removing entry from fat\n");
					return 0;
				}
			}
            break;
        }
    }

	closedir(dir);

	return 1;
}

//******************************************************
// fclose
// close a file, the buffer if file open for
// writing or append
//
//******************************************************
int fclose(int fd)
{
	if(openfiles[fd].busy)
	{
		selectFat(openfiles[fd].fat_ent.fatId);
		if(openfiles[fd].flags & (F_WR | F_AP | F_RDP | F_RDP | F_APP))
		{
			if(!flush(&openfiles[fd]))
			{
				debug("[fclose] error flushing file (=>file not close)\n");
				return 0;
			}
		}
		openfiles[fd].busy=false;
		return 1;
	}
	else
	{
		debug("[fclose] file %d not open yet\n",fd);
		return 0;
	}
}

void closeAllFile(int fd)
{
	int f;
	for(f=0;f<MAX_OPEN_FILES;f++)
		if(openfiles[f].busy)
			fclose(f);
}

//******************************************************
// remove
// remove a file based on it's name
//
// returns 0 on error
//******************************************************
int remove(const char* pathname)
{
	struct dirent* entry=NULL;

	if(!findFileEntry(pathname,&entry,true) || entry == NULL)
	{
		debug("[remove] can't erase file %s\n",pathname);
		return 0;
	}

	return 1;
}

//******************************************************
// fsize
//
// returns size in bytes
// (-1 on error)
//******************************************************
int fsize(int fd)
{
	if(openfiles[fd].busy)
	{
		return openfiles[fd].size;
	}
	else
	{
		debug("[fsize] error file %d not open\n",fd);
		return -1;
	}
}

//******************************************************
// ftell
//
// returns current psition in file in bytes
// (-1 on error)
//******************************************************
int ftell(int fd)
{
	if(openfiles[fd].busy)
	{
		return openfiles[fd].pos;
	}
	else
	{
		debug("[ftell] error file %d not open\n",fd);
		return -1;
	}
}

//******************************************************
// ftruncate
// truncate file using size
// (only possible in write mode (F_WR, F_RDP, F_WRP)
//
// returns 0 on error
//******************************************************
int ftruncate(int fd,int size)
{
	if(openfiles[fd].busy)
	{
		if(openfiles[fd].flags & (F_WR | F_RDP | F_WRP))
		{
			selectFat(openfiles[fd].fat_ent.fatId);
			FILE * file;
			struct fatent * fat_ent;

			file=&(openfiles[fd]);
			fat_ent=&file->fat_ent;

			if(size >= file->size)
			{
				debug("[ftruncate] trying to truncate file %d after its end\n",fd);
				return 0;
			}

			fseek(fd,size,SEEK_SET); // go to truncate position

			if(!fatTruncate(fat_ent,false))
			{
				debug("[ftruncate] error in fatTruncate\n");
				return 0;
			}

			file->size=size;

			if(!fUpdateEntry(file))
			{
				debug("[ftruncate] error in fUpdateEntry\n");
				return 0;
			}
		}
		else
		{
			debug("[ftruncate] file %d not open for writing\n",fd);
			return 0;
		}
	}
	else
	{
		debug("[ftruncate] file %d not open yet\n",fd);
		return 0;
	}
	return 1;
}

//******************************************************
// fflush
// flush current buffer: try to write the buffer
// available for external use
//
// returns 0 on error
//******************************************************
int fflush(int fd)
{
	if(openfiles[fd].busy)
	{
		selectFat(openfiles[fd].fat_ent.fatId);
		return flush(&openfiles[fd]);
	}
	else
	{
		debug("[fflush] file %d not open yet\n",fd);
		return 0;
	}
}

//******************************************************
// flush
// internal flush
//
// returns 0 on error
//******************************************************
int flush(FILE * file)
{
	if((file->flags & (F_WR | F_AP | F_RDP | F_WRP | F_APP)) && file->write_done)
	{
		selectFat(file->fat_ent.fatId);
		file->write_done=false;
		if(fatRWSector(&(file->fat_ent),true))
		{
			if(!fUpdateEntry(file))
			{
				debug("[flush] error updating entry\n");
				return 0;
			}
			return 1;
		}
		else
		{
			debug("[flush] error writing file\n");
			return 0;
		}
	}
	return 1;
}

//******************************************************
// fUpdateEntry
// file specific part for updating a dir entry
//
// returns 0 on error
//******************************************************
int fUpdateEntry(FILE * file)
{
	struct dirent dir_ent;
	memset(&dir_ent,0,sizeof(struct dirent));

	createDosName(file->name,dir_ent.fatName);

	dir_ent.attribute=file->attr;
	dir_ent.startCluster=file->fat_ent.startCluster;
	dir_ent.size=file->size;
	dir_ent.dirCluster=file->fat_ent.dirCluster;


	char tmpName[NAME_SIZE+EXT_SIZE+1];
	strncpy(tmpName,dir_ent.fatName,NAME_SIZE+EXT_SIZE);
	tmpName[NAME_SIZE+EXT_SIZE]=0x0;

	return fatUpdateEntry(&dir_ent);
}

//******************************************************
// fread
// copy 'count' bytes to buf
//
// returns nb bytes copyed (or <0 on error)
//******************************************************
int fread(int fd, void* buf, int count)
{
	if(openfiles[fd].busy)
	{
		if(openfiles[fd].flags & (F_RD | F_RDP | F_WRP | F_APP))
		{
			if(count == 0)
			{
				return 0;
			}

			return freadwrite(fd,buf,count,DO_READ);
		}
		else
		{
			debug("[fread] file %d not open for reading\n",fd);
			return -2;
		}
	}
	else
	{
		debug("[fread] file %d not open yet\n",fd);
		return -1;
	}
}

char fgetc(int fd)
{
	char c;
	int ret;
	if((ret=fread(fd, &c, 1))<0)
		return ret;
	else
		return c;
}

//******************************************************
// fread
// copy 'count' bytes from buf to disk
//
// returns nb bytes copyed (or <0 on error)
//******************************************************
int fwrite(int fd, void* buf, int count)
{
	if(openfiles[fd].busy)
	{
		if(openfiles[fd].flags & (F_WR | F_AP | F_RDP | F_WRP | F_APP))
		{
			if(count == 0)
			{
				debug("[fwrite] count=0\n");
				return 0;
			}

			if(openfiles[fd].flags & (F_AP | F_APP) && openfiles[fd].size != openfiles[fd].pos)
			{
				fseek(fd,0,SEEK_END);
			}

			return freadwrite(fd,buf,count,DO_WRITE);
		}
		else
		{
			debug("[fwrite] file %d not open for writing\n",fd);
			return -2;
		}
	}
	else
	{
		debug("[fwrite] file %d not open yet\n",fd);
		return -1;
	}
}

//******************************************************
// freadwrite
// real read/write operation
//
// returns nb bytes copyed (or <0 on error)
//******************************************************
int freadwrite(int fd, char* buf, int count, int mode)
{
	selectFat(openfiles[fd].fat_ent.fatId);

	FILE * file;
	struct fatent * fat_ent;
	int pos=0;
	int cpySize;
	file=&(openfiles[fd]);
	fat_ent=&file->fat_ent;

	if(file->eof)
		return -1;

	if(mode == DO_READ && (file->pos+count) > file->size)
	{
		count=file->size-file->pos;
		file->eof=true;
	}

	while(fat_ent->cacheoffset+count > BUFFER_SIZE)
	{
		cpySize=BUFFER_SIZE-fat_ent->cacheoffset;

		if(mode == DO_READ)
			memcpy(&buf[pos],&(fat_ent->cache[fat_ent->cacheoffset]),cpySize);
		else
			memcpy((&(fat_ent->cache[fat_ent->cacheoffset])),&buf[pos],cpySize);

		count -= cpySize;
		pos+=cpySize;

		if(fat_ent->eof_disk)
		{
			file->eof=true;
			return pos;
		}

		if(!nxtSector(file,mode))
			return -1;
	}

	if(fat_ent->cacheoffset+count <= BUFFER_SIZE && count !=0)
	{
		if(mode == DO_READ)
			memcpy(&buf[pos],&(fat_ent->cache[fat_ent->cacheoffset]),count);
		else
			memcpy(&(fat_ent->cache[fat_ent->cacheoffset]),&buf[pos],count);

		fat_ent->cacheoffset+= count;
		pos+=count;
	}

	file->pos+=pos;

	if(file->pos > file->size)
	{
		if( mode == DO_WRITE)
			file->size=file->pos;
		else
		{
			debug("freadwrite: pos > size while reading\n");
		}
	}



	if(pos > 0 && mode == DO_WRITE)
		file->write_done=true;

	return pos;
}

//******************************************************
// nxtSector
// internal nxtSectot (uses fat one)
// flush current buffer if needed
//
//******************************************************
int nxtSector(FILE * file,int mode)
{
	//debug("[nxtSector] |beg| cluster=%x\n",file->fat_ent.curCluster);
	if(!flush(file))
	{
		debug("nxtSector: error flushing to disk\n");
		return 0;
	}

	//debug("[nxtSector] |aft flush| cluster=%x\n",file->fat_ent.curCluster);

	if(!fatNxtSector(&(file->fat_ent),mode==DO_WRITE))
	{
		debug("[nxtSector] error getting nxt sector\n");
		return 0;
	}

	//debug("[nxtSector] |aft FatnxtSec| cluster=%x\n",file->fat_ent.curCluster);

	if(mode == DO_READ || (mode == DO_WRITE && file->size >= file->pos))
	{
		if(!fatRWSector(&(file->fat_ent),false))
		{
			debug("[nxtSector] error reading nxt sector\n");
			return 0;
		}
	}
	else
	{
		memset(file->fat_ent.cache,0,BUFFER_SIZE);
		file->fat_ent.cacheoffset=0;
	}

	//debug("[nxtSector] |end| cluster=%x\n",file->fat_ent.curCluster);

	return 1;
}

//******************************************************
// mySetPosF
// internal function to change the position in file
//
// no error handled
//******************************************************
int mySetPosF(int fd,long int pos)
{
	selectFat(openfiles[fd].fat_ent.fatId);
	FILE * file=&(openfiles[fd]);
	struct fatent * fat_ent=&file->fat_ent;

	if(pos == file->pos)
		return 1;

	int bufferPos = file->pos - fat_ent->cacheoffset;

	if(pos >= bufferPos && pos < bufferPos+BUFFER_SIZE) // we stay in the buffer
	{
		fat_ent->cacheoffset=pos-bufferPos;
		file->pos=pos;
		return 1;
	}

	if(pos < file->pos) // we need to start from the beginning
	{
		file->fat_ent.curCluster = 0;
		file->pos=0;
		nxtSector(file,DO_READ);
	}
	else
	{
		file->pos+=BUFFER_SIZE-fat_ent->cacheoffset;
		nxtSector(file,DO_READ);
	}

	while((file->pos + BUFFER_SIZE) < pos)
	{
		nxtSector(file,DO_READ);
		file->pos +=BUFFER_SIZE;
	}


	fat_ent->cacheoffset=pos - file->pos;
	file->pos=pos;
	return 1;
}

//******************************************************
// fseek
// move file position to offset starting at whence
// where whence can be: SEEK_SET, SEEK_CUR, SEEK_END
//
// returns 0 on error
//******************************************************
int fseek(int fd, long int offset, int whence)
{
	if(openfiles[fd].busy)
	{
		FILE * file=&(openfiles[fd]);
		int newpos;

		if(whence == SEEK_END)
		{
			if(offset > 0 || (offset + file->size)<0)
			{
				debug("fseek(%d,%d,SEEK_END): bad offset (size=%d)\n",fd,offset,file->size);
				return 0;
			}
			else
				newpos = offset + file->size;
		}

		if(whence == SEEK_SET)
		{
			if (offset < 0 || (offset > file->size))
			{
				debug("fseek(%d,%d,SEEK_SET): bad offset (size=%d)\n",fd,offset,file->size);
				return 0;
			}
			else
				newpos = offset;
		}

		if(whence == SEEK_CUR)
		{
			if((file->pos+offset)<0 || (file->pos+offset) > file->size)
			{
				debug("fseek(%d,%d,SEEK_CUR): bad offset (pos=%d,size=%d)\n",fd,offset,file->pos,file->size);
				return 0;
			}
			else
				newpos = file->pos+offset;
		}

		return mySetPosF(fd,newpos);
	}
	else
	{
		debug("fseek(%d,%d,SEEK_END): file not opened\n",fd,offset);
		return 0;
	}
}

//******************************************************
// fsetpos
// move file position to pos
//
//
// returns 0 on error
//******************************************************
/*int fsetpos(int fd,t_position * pos)
{
	if(openfiles[fd].busy)
	{
		return 0;
	}
	else
		return 0;
}*/

//******************************************************
// rewind
// move file position to beg of file
//(same as seek(fd,0,SEEK_SET))
//
// returns 0 on error
//******************************************************
int rewind(int fd)
{
	if(openfiles[fd].busy)
	{
		return mySetPosF(fd,0);
	}
	else
		return 0;
}

//******************************************************
// feof
//
// returns true if eof is set or file not open
//******************************************************
bool feof(int fd)
{
	if(openfiles[fd].busy)
	{
		return openfiles[fd].eof;
	}
	else
		return true;
}



//******************************************************
// loadFile
// using copying data by 1 cluster chunk
//
//returns 0 on error
//******************************************************
int loadFile(char * fileN)
{
    int curFile;

	curFile=fopen(fileN,"r");
	debug("openFile(): %d\n",curFile);

	if(curFile>=0)
	{

		int offset=0;
		char * buffer=(char*) 0x03000000;

		int clustSize=getClusterSize();

		while((fread(curFile,&buffer[offset],clustSize))>0)
			offset+=clustSize;
		fclose(curFile);
		return 1;
	}
	else
	{
		debug("Error loading file\n");
		return 0;
	}
}

//******************************************************
// printState
// returns current pos in file
//
//returns 0 on error
//******************************************************
int printState(int fd)
{
	if(openfiles[fd].busy)
	{
		debug("pos=%d,size=%d\n",openfiles[fd].pos,openfiles[fd].size);
		return 1;
	}
	else
		return 0;
}

