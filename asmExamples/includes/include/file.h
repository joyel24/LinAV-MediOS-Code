#ifndef _FILE_H_
#define _FILE_H_

#include <stdbool.h>
#include "fat.h"
#include "dir.h"

#define MAX_OPEN_FILES 8

typedef struct {

    int size;
    int attr;
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

int createFileinRoot(char * fileN);

#endif
