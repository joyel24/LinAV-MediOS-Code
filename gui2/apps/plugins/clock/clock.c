/*
* clock.c
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

/* general stuff */
#include <stdio.h>
#include <stdlib.h>

#include "cops.h" /* for plugin API functions */
#include "avevents.h" /* for events */
#include "events.h" /* for events */
#include "font.h" /* for font definitions/switching */
#include "colordef.h" /* for color definitions (COLOR_BLACK, etc) */
#include "global.h"

/* Initialization, REQUIRED */
struct client_operations * cops;

/******
 * INTS
 *****/
int done = 0;
int event;
int cursorpos = 1;
int old_mode;

extern int settings_cursorpos;

/***********
 * TIME INTS
 **********/
int hour, hour2, minute, second;
int last_h, last_m, last_s;

/*******
 * Misc
 ******/
struct av_tm current_time;
char modayyr[10];
char settings_text[50];

/************************
 * Setting default values
 ***********************/
void reset_settings(void)
{
    /* general */
    settings.clock = MODE_ANALOG; /* 1: analog, 2: digital, 3: lcd, 4: full, 5: binary */
    settings.backlight_on = 1;
    settings.save_mode = 1; /* 1: on exit, 2: automatically, 3: manually */
    settings.display_counter = 1;

    /* analog */
    settings.analog_digits = 0;
    settings.analog_date = 0; /* 0: off, 1: american, 2: european */
    settings.analog_secondhand = 1;
    settings.analog_time = 1; /* 0: off, 1: 24h, 2: 12h */

    /* digital */
    settings.digital_seconds = 1; /* 0: off, 1: digital, 2: bar, 3: fullscreen */
    settings.digital_date = 1; /* 0: off, 1: american, 2: european */
    settings.digital_blinkcolon = 0;
    settings.digital_12h = 1;

    /* LCD */
    settings.lcd_seconds = 1; /* 0: off, 1: lcd, 2: bar, 3: fullscreen */
    settings.lcd_date = 1; /* 0: off, 1: american, 2: european */
    settings.lcd_blinkcolon = 0;
    settings.lcd_12h = 1;

    /* fullscreen */
    settings.fullscreen_border = 1;
    settings.fullscreen_secondhand = 1;
    settings.fullscreen_invertseconds = 0;
}

/***************
 * Mode Selector
 **************/
void mode_selector(void)
{
    cops->putS(COLOR_WHITE, COLOR_BLACK, LCD_WIDTH/2-(13*13/2), 10, "MODE SELECTOR");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 300, 10, "Go");
    cops->putS(COLOR_WHITE, COLOR_BLACK, 270, 40, "Cancel");

    if(cursorpos == 1)
        cops->putS(COLOR_RED, COLOR_BLACK, 10, 50, "Analog Clock");
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 50, "Analog Clock");
    if(cursorpos == 2)
        cops->putS(COLOR_RED, COLOR_BLACK, 10, 70, "Digital Clock");
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 70, "Digital Clock");
    if(cursorpos == 3)
        cops->putS(COLOR_RED, COLOR_BLACK, 10, 90, "LCD-style Clock");
    else
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 90, "LCD-style Clock");
}

/***************
 * Handle events
 **************/
int eventHandler(int evt)
{
    switch(evt)
    {
        case BTN_OFF:
        case EVT_QUIT:
            if(settings.clock == MODE_SELECTOR || settings.clock == MODE_SETTINGS)
            {
                cops->clearScreen(COLOR_BLACK);
                settings.clock = old_mode;
            }
            else
                RELEASE(cops); /* we're done */
            break;

        case BTN_ON:
            if(settings.clock != MODE_SELECTOR || settings.clock == MODE_SETTINGS)
            {
                cops->clearScreen(COLOR_BLACK);
                cursorpos = old_mode = settings.clock;
                settings.clock = MODE_SELECTOR;
            }
            else if(settings.clock == MODE_SELECTOR)
            {
                cops->clearScreen(COLOR_BLACK);
                settings.clock = cursorpos;
            }
            break;

        case BTN_UP:
            if(settings.clock == MODE_SELECTOR)
                cursorpos > 1 ? (cursorpos--) : (cursorpos = 3);
            break;

        case BTN_DOWN:
            if(settings.clock == MODE_SELECTOR)
                cursorpos < 3 ? (cursorpos++) : (cursorpos = 1);
            break;

        case BTN_RIGHT:
            if(settings.clock == MODE_SETTINGS)
            {
                if(settings.analog_time < 2)
                    settings.analog_time++;
                else
                    settings.analog_time = 0;
            }
            break;

        case BTN_LEFT:
            if(settings.clock == MODE_SETTINGS)
            {
                if(settings.analog_time > 0)
                    settings.analog_time--;
                else
                    settings.analog_time = 2;
            }
            break;

        case BTN_F2:
            if(settings.clock == MODE_ANALOG || settings.clock == MODE_DIGITAL ||
               settings.clock == MODE_LCD)
            {
                old_mode = settings.clock;
                cops->setFont(FONT_SETTINGS);
                cops->clearScreen(COLOR_BLACK);
                settings.clock = MODE_SETTINGS;
            }
            else
            {
                cops->setFont(FONT_CLOCK);
                cops->clearScreen(COLOR_BLACK);
                settings.clock = old_mode;
            }
            break;
    }
}

/***************
 * Main function
 **************/
void clock(void)
{
    cops->getTime(&current_time);
    hour = current_time.tm_hour;
    hour2 = current_time.tm_hour;
    minute = current_time.tm_min;
    second = current_time.tm_sec;

    if(settings.clock == MODE_DIGITAL || settings.clock == MODE_LCD)
    {
        if(last_m != minute || last_h != hour)
        {
            cops->fillRect(COLOR_BLACK, 0, 0, 320, 120);
        }
    }

    switch(settings.clock)
    {
        case MODE_ANALOG:
            draw_analog(hour2, minute, second, last_s, last_m, last_h);
            draw_analog_extras(hour, minute, second);
            break;

        case MODE_DIGITAL:
            draw_7seg_time(hour, minute, LCD_WIDTH/2-(32*5)/2, 16, 32, 64, TRUE, FALSE);
            break;

        case MODE_LCD:
            draw_7seg_time(hour, minute, LCD_WIDTH/2-(32*5)/2, 16, 32, 64, TRUE, TRUE);
            break;

        case MODE_SELECTOR:
            mode_selector();
            break;

        case MODE_SETTINGS:
            analog_settings();
            break;
    }

    last_s = second;
    last_m = minute;
    last_h = hour;
}

/****************
 * Draw the clock
 ***************/
int main(int argc,char * * argv)
{
    /* Initialization, REQUIRED */
    REGISTER(cops,eventHandler,0);

    cops->hideSBar(); /* shut off the status bar */
    cops->disableMenu(); /* disable the F3=MENU function */
    cops->clearScreen(COLOR_BLACK); /* clear the LCD to black */
    cops->setFont(FONT_CLOCK);

    reset_settings();

    /* These two functions REQUIRED */
    /* Usage: PACK(cops, <main function>)*/
    PACK(cops, clock);
    /* Stay here in the plugin */
    STOPME(cops);
}