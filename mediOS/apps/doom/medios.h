#ifndef __MEDIOS__
#define __MEDIOS__

#include <stdarg.h>

#include <api.h>
#include <fs_io.h>
#include <sys_def/ctype.h>
#include <sys_def/string.h>
#include <sys_def/section_types.h>
#include <graphics.h>

#include <kernel/osd.h>
#include <kernel/buttons.h>

#include <kernel/timer.h>
#include <kernel/uart.h>

//TODO: ugly hack
#define alloca malloc

#define stderr -1
#define stdout -2

#define ROOTFOLDER "/doom/"

#define REALSCREENWIDTH 224
#define REALSCREENHEIGHT 176

int access(char *file, int mode);

int fprintf(int f,const char * s,...);
int fscanf(int f,const char * s,...);
void * getenv(const char * s);
void setbuf(int f,void * p);
void getchar();

#endif
