/*
* global.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Zakk Roberts
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/
/*********
 * DEFINEs
 ********/
#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define TRUE 1
#define FALSE 0

#define FONT_CLOCK    STD8X13
#define FONT_SETTINGS STD6X10

/*******
 * MODES
 ******/
#define MODE_ANALOG 1
#define MODE_DIGITAL 2
#define MODE_LCD 3
#define MODE_FULLSCREEN 4
#define MODE_BINARY 5
#define MODE_SETTINGS 6
#define MODE_SELECTOR 7

/***********************************
 * This is saved to default_filename
 **********************************/
struct saved_settings
{
    /* general */
    int clock; /* 1: analog, 2: digital led, 3: digital lcd, 4: full, 5: binary */
    int backlight_on;
    int save_mode; /* 1: on exit, 2: automatically, 3: manually */
    int display_counter;

    /* analog */
    int analog_digits;
    int analog_date; /* 0: off, 1: american, 2: european */
    int analog_secondhand;
    int analog_time; /* 0: off, 1: 24h, 2: 12h */

    /* digital */
    int digital_seconds; /* 0: off, 1: digital, 2: bar, 3: fullscreen */
    int digital_date; /* 0: off, 1: american, 2: european */
    int digital_blinkcolon;
    int digital_12h;

    /* LCD */
    int lcd_seconds; /* 0: off, 1: lcd, 2: bar, 3: fullscreen */
    int lcd_date; /* 0: off, 1: american, 2: european */
    int lcd_blinkcolon;
    int lcd_12h;

    /* fullscreen */
    int fullscreen_border;
    int fullscreen_secondhand;
    int fullscreen_invertseconds;
} settings;

/**************************************************
 * Used for hands to define lengths at a given time
 *************************************************/
static unsigned char yhour[] = {
167,166,166,164,163,160,158,155,151,147,143,139,134,130,125,120,116,111,107,102,
 98, 94, 90, 86, 83, 81, 78, 77, 75, 75, 74, 75, 75, 77, 78, 81, 83, 86, 90, 94,
 98,102,107,111,116,120,125,130,134,139,143,147,151,155,158,160,163,164,166,166
};
static unsigned char yminute[] = {
190,190,188,187,184,181,177,172,167,161,155,148,142,135,127,120,114,106, 99, 93,
 86, 80, 74, 69, 64, 60, 57, 54, 53, 51, 51, 51, 53, 54, 57, 60, 64, 69, 74, 80,
 86, 93, 99,106,114,120,127,135,142,148,155,161,167,172,177,181,184,187,188,190
};
static unsigned char xhour[] = {
160,166,172,178,184,189,194,199,203,207,211,213,215,217,218,218,218,217,215,213,
211,207,203,199,194,189,184,178,172,166,160,155,149,143,137,132,127,122,118,114,
110,108,106,104,103,103,103,104,106,108,110,114,118,122,127,132,137,143,149,155
};
static unsigned char xminute[] = {
160,169,178,187,196,204,211,219,225,231,236,240,243,246,247,248,247,246,243,240,
236,231,225,219,211,204,196,187,178,169,160,152,143,134,125,117,110,102, 96, 90,
 85, 81, 78, 75, 74, 73, 74, 75, 78, 81, 85, 90, 96,102,110,117,125,134,143,152
};