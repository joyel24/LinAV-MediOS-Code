#ifndef __MEDIOS_H
#define __MEDIOS_H

#include "dspshared.h"

//#ifdef BUILD_STDALONE
#if 1 //TODO: need a way to propagate BUILD_STDALONE GCC flag in subdirs
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
    #include <kernel/sound.h>
    #include <kernel/bat_power.h>
    #include <kernel/videnc.h>
    #include <kernel/icons.h>
    #include <kernel/widgetlist.h>
    #include <kernel/widgetmenu.h>
    #include <kernel/cache.h>
    #include <kernel/lcd.h>
    extern int gui_pal[256][3];
#else
    #include <api.h>
#endif

#include <gui/file_browser.h>

#include <int_timer.h>
#include <buttons.h>
#include <kernel/io.h>

#include <sys_def/section_types.h>

#include <sys_def/ctype.h>
#include <sys_def/string.h>
#include <sys_def/alloca.h>
#include <sys_def/timer.h>
#include <sys_def/font.h>
#include <sys_def/colordef.h>

#ifdef DM320
#undef __IRAM_CODE
#define __IRAM_CODE
#undef __IRAM_DATA
#define __IRAM_DATA
#endif

#endif
