#ifndef _FILE_H_
#define _FILE_H_

#include <stdbool.h>

#include "fat.h"

#ifndef F_RD
#define F_RD 	1
#endif
#ifndef F_WR
#define F_WR 	2
#endif
#ifndef F_AP
#define F_AP	4
#endif
#ifndef F_RDP
#define F_RDP	8
#endif
#ifndef F_WRP
#define F_WRP	0x10
#endif
#ifndef F_APP
#define F_APP	0x20
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
	char name[NAME_SIZE+EXT_SIZE+2];
	int size;
    int attr;
	int pos;
    bool busy;
	bool write_done;
	int flags;
    bool eof;
	struct fatent fat_ent;
} FILE;

#include "dirent.h"

// initialization
extern void inifile();

// user functions

// mode:
// "r"		: read-only
// "w" 	: write-only (file cleared if exists)
// "a" 	: append-only
// "r+" 	: read/write on existing file
// "w+"	: read/write (file cleared if exists)
// "a+"	: read/append (write only at the end of the file)
extern int fopen(const char* pathname,const char * mode);
//extern int freopen(int fd,int flags);
extern int fclose(int fd);

extern int fread(int fd, void* buf, int count);
//extern int fgetc(int fd);
//extern int fgets(int fd,const char * string,int maxread);
//extern int fprintf(int fd,char *fmt, ...);
extern int fwrite(int fd, void* buf, int count);
//extern int fscanf(int fd,char *fmt, ...);
//extern int fputc(int fd,int ch);
//extern int fputs(int fd,const char * string);

extern int printState(int fd);

// moving in the file
extern int ftell(int fd);
extern int fseek(int fd, long int offset, int whence);
//extern int fsetpos(int fd,t_position pos);
//extern int fgetpos(int fd,t_position pos);
extern int rewind(int fd);
extern bool feof(int fd);

// file utils
extern int remove(const char* pathname);
extern int fsize(int fd);
extern int ftruncate(int fd,int size);
extern int fflush(int fd);

extern int loadFile(char * fileN);

// internal functions
extern int findFileEntry(const char* pathname,struct dirent ** direntry,bool remove);
extern int flush(FILE * file);
extern int freadwrite(int fd, char* buf, int count, int mode);
extern int nxtSector(FILE * file,int mode);
extern int mySetPosF(int fd,long int pos);
extern int fUpdateEntry(FILE * file);
extern int getFlag(const char * mode);

#endif
