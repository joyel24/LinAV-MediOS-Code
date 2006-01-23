#ifndef __MEDIOS__
#define __MEDIOS__

#include <stdarg.h>

#include <api.h>
#include <fs_io.h>
#include <sys_def/ctype.h>
#include <sys_def/string.h>
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

// buttons

#define BUTTON_UP     0
#define BUTTON_DOWN   1
#define BUTTON_LEFT   2
#define BUTTON_RIGHT  3
#define BUTTON_MENU1  4
#define BUTTON_MENU2  5
#define BUTTON_MENU3  6
#define BUTTON_SQUARE 7
#define BUTTON_CROSS  8
#define BUTTON_ON     9
#define BUTTON_OFF    10

#undef  NB_BUTTONS
#define NB_BUTTONS    11

int access(char *file, int mode);

int fprintf(int f,const char * s,...);
int fscanf(int f,const char * s,...);
void * getenv(const char * s);
void setbuf(int f,void * p);
void getchar();

#endif
