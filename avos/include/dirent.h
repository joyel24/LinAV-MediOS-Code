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

#define MAX_OPEN_DIRS 8
#define MAX_PATH 256

#define END_ENTRY	0
#define EMPTY_ENTRY	1
#define BAD_ENTRY	3
#define GOOD_ENTRY	4

#define T_DIR	1
#define T_FILE	2

#define NAME_SIZE 8
#define EXT_SIZE 3

struct dirent {
	char entryName[NAME_SIZE+EXT_SIZE+2]; // name + '.' + ext + \0
    char name[NAME_SIZE+1]; // name + \0
	char ext[EXT_SIZE+1]; // ext + \0
	char fatName[NAME_SIZE+EXT_SIZE+1];
    int attribute;
    int startCluster;
	int size;
	int dirCluster;
};

#include "fat.h"

typedef struct {
	unsigned char name[MAX_PATH];
    bool busy;
	int attribute;
	int size;
	struct dirent theent;
	struct fatent fat_ent;
} DIR;

extern int opendir(const char* name);
extern void closedir(int dd);
extern struct dirent* readdir(int dd);

extern int removeEntry(int dir);
extern int createEntry(const char * pathname,int type,struct dirent* ent);

extern void inidir();


#endif
