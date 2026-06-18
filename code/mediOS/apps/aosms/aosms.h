#ifndef __AOSMS_H
#define __AOSMS_H
    
#define AOSMS_PATH "/aoSMS/"
#define SAVES_PATH "/aoSMS/saves/"
#define CFG_FILE_PATH "/aoSMS/aoSMS.cfg"

#ifdef GMINI402

    #define ENABLE_OVERCLOCKING

    #define SMS_BTN_1             BTMASK_BTN1
    #define SMS_BTN_2             BTMASK_BTN2
    #define SMS_BTN_MOD_12        BTMASK_F3
    #define SMS_BTN_INGAME_MENU   BTMASK_F1
    #define SMS_BTN_START         BTMASK_ON
    #define SMS_BTN_RESET         BTMASK_F2
    #define SMS_BTN_QUIT          BTMASK_OFF

    #define GG_WIDTH_DELTA        0
    #define GG_HEIGHT_DELTA       0
    #define LCD_GG_X              -2
    #define LCD_GG_Y              -1
    #define LCD_GG_WIDTH_DELTA    0
    #define LCD_GG_HEIGHT_DELTA   2

    #define SMS_WIDTH_DELTA       4
    #define SMS_HEIGHT_DELTA      0
    #define LCD_SMS_X             -1
    #define LCD_SMS_Y             0
    #define LCD_SMS_WIDTH_DELTA   0
    #define LCD_SMS_HEIGHT_DELTA  0
#endif

#ifdef GMINI4XX

    #define ENABLE_OVERCLOCKING

    #define SMS_BTN_1             BTMASK_BTN1
    #define SMS_BTN_2             BTMASK_BTN2
    #define SMS_BTN_MOD_12        BTMASK_F3
    #define SMS_BTN_INGAME_MENU   BTMASK_F1
    #define SMS_BTN_START         BTMASK_ON
    #define SMS_BTN_RESET         BTMASK_F2
    #define SMS_BTN_QUIT          BTMASK_OFF

    #define GG_WIDTH_DELTA        4
    #define GG_HEIGHT_DELTA       0
    #define LCD_GG_X              8
    #define LCD_GG_Y              -1
    #define LCD_GG_WIDTH_DELTA    0
    #define LCD_GG_HEIGHT_DELTA   2

    #define SMS_WIDTH_DELTA       0
    #define SMS_HEIGHT_DELTA      0
    #define LCD_SMS_X             -8
    #define LCD_SMS_Y             -2
    #define LCD_SMS_WIDTH_DELTA   0
    #define LCD_SMS_HEIGHT_DELTA  4
#endif

#if defined(AV4XX) || defined(PMA)

    #define ENABLE_OVERCLOCKING

    #define SMS_BTN_1             BTMASK_BTN1
    #define SMS_BTN_2             BTMASK_ON
    #define SMS_BTN_MOD_12        0
    #define SMS_BTN_INGAME_MENU   BTMASK_F1
    #define SMS_BTN_START         BTMASK_F3
    #define SMS_BTN_RESET         BTMASK_F2
    #define SMS_BTN_QUIT          BTMASK_OFF

    #define GG_WIDTH_DELTA        0
    #define GG_HEIGHT_DELTA       0
    #define LCD_GG_X              0
    #define LCD_GG_Y              0
    #define LCD_GG_WIDTH_DELTA    0
    #define LCD_GG_HEIGHT_DELTA   0

    #define SMS_WIDTH_DELTA       0
    #define SMS_HEIGHT_DELTA      0
    #define LCD_SMS_X             0
    #define LCD_SMS_Y             0
    #define LCD_SMS_WIDTH_DELTA   0
    #define LCD_SMS_HEIGHT_DELTA  0
#endif

extern bool autoFrameSkip;
extern int  frameSkip;
extern bool swapButtons;
extern int tvOut;
extern bool useResize;
extern bool overclocking;
extern int armFrequency;
extern int f3Use;
extern char romname[256];

void clk_overclock(bool en);
void screen_init();
void codec_setVolume(int vol);

#endif
