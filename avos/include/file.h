#ifndef _FILE_H_
#define _FILE_H_

#include <stdbool.h>
#include "fat.h"
#include "dirent.h"

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
} FILE;

extern void codecpy(char * image,u32 offset,u32 size);
extern int startProg(int argCount, char * arg);

extern int fopen(const char* pathname);
extern void fclose(int fd);
extern int fread(int fd, void* buf, int count);
extern int fsize(int fd);
extern int ftell(int fd);
extern int fseek(int fd, long int offset, int whence);
extern int fsetpos(int fd,long int newpos);
extern int rewind(int fd);

extern int launchFile(char * fileN);

extern void inifile();

#endif
