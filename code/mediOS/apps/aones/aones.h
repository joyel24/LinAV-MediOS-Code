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
    
    #define NES_BTN_UP      BTMASK_UP
    #define NES_BTN_DOWN    BTMASK_DOWN
    #define NES_BTN_RIGHT   BTMASK_RIGHT
    #define NES_BTN_LEFT    BTMASK_LEFT
    #define NES_BTN_A       BTMASK_BTN1
    #define NES_BTN_B       BTMASK_BTN2
    #define NES_BTN_MOD_AB  BTMASK_F3
    
    #define NES_BTN_INGAME_MENU   BTMASK_F1
    #define NES_BTN_HALT          BTMASK_OFF
    #define NES_BTN_START         BTMASK_ON
    #define NES_BTN_SELECT        BTMASK_F2
    
    #define NES_LCD_X -8
    #define NES_LCD_Y -2
    #define NES_LCD_W 224
    #define NES_LCD_H 176  
    
#endif

#ifdef GMINI402
    #define GMINI_OVERCLOCKING

    #define SCREEN_USE_RESIZE

    #define SOUND_USE_DSP
    #define SOUND_USE_AIC23
    
    #define NES_BTN_UP      BTMASK_UP
    #define NES_BTN_DOWN    BTMASK_DOWN
    #define NES_BTN_RIGHT   BTMASK_RIGHT
    #define NES_BTN_LEFT    BTMASK_LEFT
    #define NES_BTN_A       BTMASK_BTN1
    #define NES_BTN_B       BTMASK_BTN2
    #define NES_BTN_MOD_AB  BTMASK_F3
    
    #define NES_BTN_INGAME_MENU   BTMASK_F1
    #define NES_BTN_HALT          BTMASK_OFF
    #define NES_BTN_START         BTMASK_ON
    #define NES_BTN_SELECT        BTMASK_F2

    #define NES_LCD_X -6
    #define NES_LCD_Y 0
    #define NES_LCD_W 224
    #define NES_LCD_H 176 
    
#endif

#if defined(AV4XX) || defined(PMA)
    //#define GMINI_OVERCLOCKING

    #define SCREEN_USE_DSP
    #define SCREEN_USE_RESIZE

    #define SOUND_USE_DSP
    #define SOUND_USE_AIC23
    
    #define NES_BTN_UP      BTMASK_UP
    #define NES_BTN_DOWN    BTMASK_DOWN
    #define NES_BTN_RIGHT   BTMASK_RIGHT
    #define NES_BTN_LEFT    BTMASK_LEFT
    
    #define NES_BTN_A       BTMASK_BTN1
    #define NES_BTN_B       BTMASK_ON
    #define NES_BTN_MOD_AB  0x0
    
    #define NES_BTN_INGAME_MENU   BTMASK_F2
    #define NES_BTN_HALT          BTMASK_F3
    #define NES_BTN_START         BTMASK_F1
    #define NES_BTN_SELECT        BTMASK_OFF
    
    #define NES_LCD_X 0
    #define NES_LCD_Y -7
    #define NES_LCD_W 320
    #define NES_LCD_H 256 

    
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

void clk_overclock(bool en);
void emu_handleVideoBuffer();
int emu_frameCompleted();
long emu_joypad1State();
long emu_joypad2State();
void display_tvOutSet();


#endif
