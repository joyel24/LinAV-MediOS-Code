#include <stdio.h>
#include <string.h>

#include "dir.h"
#include "buttons.h"
#include "file.h"
#include "terminal.h"
#include "uart.h"
#include "lcd.h"
#include "hex.h"




static myDIR opendirs[MAX_OPEN_DIRS];

int addFile(int dir,char * fileN)
{

}

void iniDir()
{
	int dd;
	for ( dd=0; dd<MAX_OPEN_DIRS; dd++ )
		opendirs[dd].busy=false;
}

int openDir(char* name)
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
		uartOutsT("Too many dir open\n");
        return -1;
    }

    opendirs[dd].busy = true;

    if ( name[0] != '/' ) {
        uartOutsT("Only absolute paths supported right now\n");
        opendirs[dd].busy = false;
        return -1;
    }

   fatOpendir(&(opendirs[dd]), NULL);

    strncpy(namecopy,name,sizeof(namecopy));
    namecopy[sizeof(namecopy)-1] = 0;

    for ( part = strtok_r(namecopy, "/", &end); part;part = strtok_r(NULL, "/", &end))
	{
		while (1)
		{
            if ((entry=readDir(dd))==NULL)
			{
                opendirs[dd].busy = false;
				uartOutsT("Repertoire non trouvé\n");
                return -1;
            }
            if ( (entry->attribute & ATTR_DIRECTORY) && (strcasecmp(part, entry->name) == 0))
			{
				fatOpendir(&(opendirs[dd]), entry);
                break;
            }
        }
    }

	strncpy(opendirs[dd].name,name,sizeof(namecopy));

    return dd;
}

void fatOpendir(myDIR * fat_dir,struct dirent * dirInfo)
{

	if(dirInfo == NULL)
	{
		fat_dir->fat_ent.startCluster=getFatRootCluT();
		fat_dir->attribute=0;
	}
	else
	{
		fat_dir->fat_ent.startCluster=dirInfo->startcluster;
		fat_dir->attribute=dirInfo->attribute;
	}

	fat_dir->fat_ent.curCluster=0;
	fat_dir->fat_ent.cacheoffset=0;
	fat_dir->fat_ent.eof_disk=false;

	nxtSector(&fat_dir->fat_ent);
}

struct dirent* readDir(int dd)
{
	myDIR* dir=&(opendirs[dd]);
	struct fatent * fat_ent=&dir->fat_ent;
	struct dirent * theent = &(dir->theent);
	char entry[33];
	int count=32;
	int pos=0;

	int v1,v2;
	bool done=false;

	while(!done)
	{
		if(512-fat_ent->cacheoffset<=32)
		{
			memcpy(entry,&fat_ent->cache[fat_ent->cacheoffset],512-fat_ent->cacheoffset);
			if(fat_ent->eof_disk)
			{
				uartOutsT("End of cluster chain, before end of entry\n");
				break;
			}
			pos=512-fat_ent->cacheoffset;
			count=32-pos;
			nxtSector(fat_ent);
		}

		if(count>0)
		{
			memcpy(entry,&(fat_ent->cache[fat_ent->cacheoffset]),count);
			fat_ent->cacheoffset+= count;
		}

		if((entry[11] == 0xf) || ((entry[11] & ATTR_VOLUME_ID)==ATTR_VOLUME_ID) || entry[0]==0xe5)
		{
			count=32;
			pos=0;
		}
		else
		{
			done=true;
			if(entry[0] == 0x0)
			{
				theent=NULL;
			}
			else
			{
				int i,j;

				for(i=0;i<8;i++)
					if(entry[i] == ' ')
						break;
					else
						theent->name[i]=entry[i];

				if(entry[8] != ' ')
				{
					theent->name[i]='.';
					i++;
					for(j=0;j<3;j++)
					{
						if(entry[8+j] == ' ')
							break;
						else
						{
							theent->ext[j]=entry[8+j];
							theent->name[i]=entry[8+j];
							i++;
						}
					}
				}

				theent->name[i]=0x0;
				theent->ext[j]=0x0;

				theent->attribute=entry[11];
				v1=read2T(&entry[20]);
				v1 = v1 << 16;
				v2=read2T(&entry[26]);
				theent->startcluster=v1+v2;
				theent->size=read4T(&entry[28]);
			}
		}
	}

	if(!done)
	{
		return NULL;
	}
	else
	{
		return theent;
	}
}

void closeDir(int dd)
{
	myDIR* dir=&(opendirs[dd]);
    dir->busy=false;
}

void dispDir(char * nameUP,void (*action[])(char * name),char * * ext,int nbExt)
{
	int dir;
	struct dirent* entry;

	bool launch=false;
	bool error=false;

	char nameCur[MAX_PATH];
	strcpy(nameCur,nameUP);

	char fileName[20][13];
	char fileAttr[20];
	char fileExt[20][4];
	char tmpName[13];

	int color;

	while(!error)
	{
		if((dir=openDir(nameCur))>=0)
		{
			int i=0;
			while((entry=readDir(dir))!=NULL && i<20)
			{
				strcpy(fileName[i],entry->name);
				strcpy(fileExt[i],entry->ext);
				fileAttr[i]=entry->attribute;
				i++;
			}
			closeDir(dir);

			int pos=0;
			int max=i;
			int posX=220;
			int posY=36;
			int nbDisp = 18;

			int cursor=0;

			int key;
			bool done=false;

			lcdSetBgT(0x0000);
			for(i=0;i<nbDisp;i++)
				lcdPutsT(posX,posY+6*i,"            ",0xffff);

			while(!done)
			{
				for(i=0;i+pos<max && i<nbDisp;i++)
				{
					if(i == cursor)
						lcdSetBgT(0x0404);
					else
						lcdSetBgT(0x0000);

					if((fileAttr[i+pos] & ATTR_DIRECTORY)==ATTR_DIRECTORY)
						color=0xfbfb;
					else
						color=0xffff;

					strcpy(tmpName,fileName[i+pos]);
					int len=strlen(tmpName);
					if(len<12)
						for(;len<12;len++)
							tmpName[len]=' ';

					lcdPutsT(posX,posY+6*i,tmpName,color);
				}

				key=waitForKeyPress();
				waitForKeyReleased(20000);
				if(!(key & upButton) )
				{
					if(cursor<=0 && pos > 0)
						pos--;
					else
						if(cursor!=0)
							cursor--;
				}
				else
					if(!(key & downButton) && ((pos+cursor+1) < max))
					{
						if((cursor+1) == nbDisp)
							pos++;
						else
							cursor++;
					}
					else
						if(!(key & menu1Button) && ((fileAttr[pos+cursor] & ATTR_DIRECTORY)==ATTR_DIRECTORY) && strcmp(fileName[cursor],".")!=0)
						{
							done=true;
						}
						else
							if(!(key & menu2Button) && (fileAttr[pos+cursor] & ATTR_DIRECTORY)!=ATTR_DIRECTORY)
							{
								done=true;
								launch=true;
							}
			}

			if(!launch)
			{
				if(strcmp(fileName[pos+cursor],"..")==0)
				{
					char * namePos=strrchr(nameCur+1,'/');
					if ( namePos )
					{
						*namePos = 0;
					}
					namePos=strrchr(nameCur,'/');
					if ( namePos )
					{
						*(namePos+1) = 0;
					}

				}
				else
				{
					strcat(nameCur,fileName[pos+cursor]);
					strcat(nameCur,"/");
				}
			}
			else
			{
				char fileN[MAX_PATH];
				strcpy(fileN,nameCur);
				strcat(fileN,fileName[pos+cursor]);
				int i;

				for(i=0;i<nbExt;i++)
				{
					if(strcmp(fileExt[pos+cursor],ext[i])==0)
						action[i](fileN);
				}
			}

			launch = false;
		}
		else
		{
			terminalPutsCT("Error opening dir\n", 0xfbfb);
			error=true;
		}
	}
}

