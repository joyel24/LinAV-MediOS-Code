#ifndef __MEDIOS__
#define __MEDIOS__

#include <stdarg.h>

#ifdef BUILD_STDALONE
    #include <kernel/graphics.h>
    #include <kernel/stdfs.h>
    #include <kernel/malloc.h>
    #include <kernel/buttons.h>
    #include <kernel/timer.h>
    #include <kernel/delay.h>
    #include <kernel/osd.h>
    #include <kernel/uart.h>
    #include <kernel/exit.h>
    #include <sys_def/string.h>
    extern int gui_pal[256][3];
#else
    #include <api.h>
    #include <sys_def/malloc.h>
    #include <sys_def/stddef.h>
    #include <kernel/gui_pal.h>
#endif

#include <buttons.h>
#include <kernel/io.h>

#include <sys_def/ctype.h>
#include <sys_def/section_types.h>


#include <sys_def/alloca.h>
#include <sys_def/timer.h>
#include <sys_def/font.h>
#include <sys_def/colordef.h>
#include <gui/file_browser.h>
#include <gui/icons.h>

#define stderr -1
#define stdout -2

#define ROOTFOLDER "/doom/"

#ifdef GMINI4XX
#define REALSCREENWIDTH 224
#define REALSCREENHEIGHT 176
#endif

#ifdef AV3XX
#define REALSCREENWIDTH 320
#define REALSCREENHEIGHT 200
#endif
/*
#undef __IRAM_CODE
#define __IRAM_CODE
#undef __IRAM_DATA
#define __IRAM_DATA
*/
extern void app_exit();



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
