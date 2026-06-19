#ifndef _FILE_H_
#define _FILE_H_

#include <stdbool.h>
#include "fat.h"
#include "dir.h"

#ifndef O_RDONLY
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2
#define O_CREAT  4
#define O_APPEND 8
#define O_TRUNC  0x10
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

#define MAX_OPEN_FILES 8

typedef struct {

    int size;
    int attr;
	int count;
    bool busy;
    bool eof;
	struct fatent fat_ent;
} myFILE;

extern void codecpy(char * image,u32 offset,u32 size);
extern int startProg(int argCount, char * arg);

int openF(const char* pathname);
void closeF(int fd);
int readF(int fd, void* buf, int count);
int sizeF(int fd);
int tellF(int fd);
int seekF(int fd, long int offset, int whence);
int setposF(int fd,long int newpos);

int launchFile(char * fileN);

void iniFile();

#endif
