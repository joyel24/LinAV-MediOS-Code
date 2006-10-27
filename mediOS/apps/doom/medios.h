#ifndef __MEDIOS__
#define __MEDIOS__

#include <stdarg.h>

#include "dspshared.h"
#include "doom_gui.h"

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
    #include <kernel/dsp.h>
    #include <kernel/aic23.h>
    #include <kernel/clkc.h>
    #include <kernel/resize.h>
    #include <kernel/evt.h>
    #include <kernel/bat_power.h>
    #include <kernel/videnc.h>
    #include <kernel/icons.h>
    #include <kernel/widgetlist.h>
    #include <kernel/widgetmenu.h>
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

#define CFG_FILE_PATH "/doom/doom.cfg"

#ifdef DM320
#undef __IRAM_CODE
#define __IRAM_CODE
#undef __IRAM_DATA
#define __IRAM_DATA
#endif

extern void app_exit();



// DOOM_BUTTONs
#if defined(GMINI4XX) || defined(GMINI402) || defined(AV3XX)
#define DOOM_BUTTON_UP     0
#define DOOM_BUTTON_DOWN   1
#define DOOM_BUTTON_LEFT   2
#define DOOM_BUTTON_RIGHT  3
#define DOOM_BUTTON_MENU1  4
#define DOOM_BUTTON_MENU2  5
#define DOOM_BUTTON_MENU3  6
#define DOOM_BUTTON_1      7
#define DOOM_BUTTON_2      8
#define DOOM_BUTTON_ON     9
#define DOOM_BUTTON_OFF    10

#undef  NB_DOOM_BUTTONS
#define NB_DOOM_BUTTONS    11
#endif


#if defined(AV4XX)
#define DOOM_BUTTON_UP     0
#define DOOM_BUTTON_DOWN   1
#define DOOM_BUTTON_LEFT   2
#define DOOM_BUTTON_RIGHT  3
#define DOOM_BUTTON_MENU1  9
#define DOOM_BUTTON_MENU2  5
#define DOOM_BUTTON_MENU3  10
#define DOOM_BUTTON_1      4
#define DOOM_BUTTON_2      8
#define DOOM_BUTTON_ON     7
#define DOOM_BUTTON_OFF    6

#undef  NB_DOOM_BUTTONS
#define NB_DOOM_BUTTONS    11
#endif



int access(char *file, int mode);

int fprintf(int f,const char * s,...);
int fscanf(int f,const char * s,...);
void * getenv(const char * s);
void setbuf(int f,void * p);
void getchar();

#endif
