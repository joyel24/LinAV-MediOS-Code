#ifndef __MEDIOS__
#define __MEDIOS__

#include <stdarg.h>

//#ifdef BUILD_STDALONE
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
    //#define printf printk
//#else
//    #include <api.h>
//#endif

#include <buttons.h>
#include <kernel/io.h>

#include <sys_def/section_types.h>

#include <sys_def/ctype.h>
#include <sys_def/string.h>
#include <sys_def/alloca.h>
#include <sys_def/timer.h>
#include <sys_def/font.h>
#include <sys_def/colordef.h>
#include <gui/file_browser.h>
#include <gui/icons.h>


#define F_HANDLE int

#define DEBUGS printf

#define LJ_ROMSDIR "/"
#define LJ_SAVESDIR "/"

#endif
