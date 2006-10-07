#ifndef __AONES_H
#define __AONES_H

#include "medios.h"
#include "unes.h"

#ifdef GMINI4XX
    #define GMINI_OVERCLOCKING

    #define SCREEN_USE_DSP
    #define SCREEN_USE_RESIZE

    #define SOUND_USE_DSP
    #define SOUND_USE_AIC23
#endif

#ifdef GMINI402
    #define GMINI_OVERCLOCKING

    #define SCREEN_USE_RESIZE

    #define SOUND_USE_DSP
    #define SOUND_USE_AIC23
#endif

#ifdef AV4XX
    //#define GMINI_OVERCLOCKING

    #define SCREEN_USE_DSP
    //#define SCREEN_USE_RESIZE

    #define SOUND_USE_DSP
    #define SOUND_USE_AIC23
#endif

#define AONES_PATH "/aoNES/"
#define SAVES_PATH "/aoNES/saves/"
#define CFG_FILE_PATH "/aoNES/aoNES.cfg"

#define AUTOFIRE_INTERVAL 3

extern __IRAM_DATA VirtualNES Vnes;

extern char CurrentROMFile[256];

extern int autoFire;
extern int frameSkip;
extern int f3Use;
extern bool buttonsSwap;
extern bool overclocking;
extern int armFrequency;
extern int dspFrequency;
extern int tvOut;


void dsp_write32(volatile void * p, uint32 value);

void clk_overclock(bool en);
void emu_handleVideoBuffer();
int emu_frameCompleted();
long emu_joypad1State();
long emu_joypad2State();
void display_tvOutSet();


#endif
