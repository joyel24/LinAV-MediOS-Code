#ifndef _DIR_H_
#define _DIR_H_

#include <stdbool.h>

#define ATTR_READ_ONLY   0x01
#define ATTR_HIDDEN      0x02
#define ATTR_SYSTEM      0x04
#define ATTR_VOLUME_ID   0x08
#define ATTR_DIRECTORY   0x10
#define ATTR_ARCHIVE     0x20
#define ATTR_LONG_NAME   0x0f

#define NAMESIZE 13
#define EXTSIZE 4

#define MAX_OPEN_DIRS 8
#define MAX_PATH 256

#include "fat.h"

struct dirent {
    unsigned char name[NAMESIZE];
	unsigned char ext[EXTSIZE];
    int attribute;
    int startcluster;
	int size;
};

typedef struct {
	unsigned char name[MAX_PATH];
    bool busy;
	int attribute;
	int size;
	struct dirent theent;
	struct fatent fat_ent;
} myDIR;

int openDir(char* name);
void fatOpendir(myDIR * fat_dir,struct dirent * dirInfo);
void closeDir(int dd);
struct dirent* readDir(int dd);
void iniDir();
void dispDir(char * nameUP,void (*action[])(char * name),char * * ext,int nbExt);

int addFile(int dir,char * fileN);

#endif
