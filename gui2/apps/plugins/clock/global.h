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

void draw_analog         (int hour, int minute, int second, int last_s, int last_m, int last_h);
void draw_analog_extras  (int hour, int minute, int second);
void draw_seg_led        (int seg, int x, int y, int width, int height);
void draw_seg_lcd        (int seg, int x, int y, int width, int height);
void draw_7seg_digit     (int digit, int x, int y, int width, int height, int lcd_display);
void draw_7seg_time      (int hour, int minute, int x, int y, int width, int height,
                                int colon, int lcd);
void analog_settings     (void);
