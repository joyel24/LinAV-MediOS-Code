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

int open(const char* pathname);
void close(int fd);
int read(int fd, void* buf, int count);

int launchFile(char * fileN);

void iniFile();

#endif
