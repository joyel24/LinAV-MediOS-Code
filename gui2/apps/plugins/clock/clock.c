/*
* clock
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

/* Initialization, REQUIRED */
struct client_operations * cops;

/*********
 * DEFINEs
 ********/
#define LCD_WIDTH 320
#define LCD_HEIGHT 240

/******
 * INTS
 *****/
int pos;
int i;
int done = 0;
int event;
int pos2 = 0;

/***********
 * TIME INTS
 **********/
int hour, minute, second;
int last_h, last_m, last_s;
int hour2;

/*******
 * Misc
 ******/
struct av_tm current_time;
char modayyr[10];
char time[10];

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

/************************
 * Setting default values
 ***********************/
void reset_settings(void)
{
    /* general */
    settings.clock = 1; /* 1: analog, 2: digital, 3: lcd, 4: full, 5: binary */
    settings.backlight_on = 1;
    settings.save_mode = 1; /* 1: on exit, 2: automatically, 3: manually */
    settings.display_counter = 1;

    /* analog */
    settings.analog_digits = 0;
    settings.analog_date = 0; /* 0: off, 1: american, 2: european */
    settings.analog_secondhand = 1;
    settings.analog_time = 0; /* 0: off, 1: 24h, 2: 12h */

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

/***********************************************************
 * Used for hands to define lengths at a given time - ANALOG
 **********************************************************/
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

/************************
 * Draws the analog hands
 ***********************/
void draw_analog(void)
{
    pos = 90-second;
    if(pos >= 60)
        pos -= 60;

    pos2 = 90-last_s;
    if(pos2 >= 60)
        pos2 -= 60;

    cops->drawLine(COLOR_WHITE, (LCD_WIDTH/2), (LCD_HEIGHT/2),
                        xminute[pos], yminute[pos]);
    if(second != last_s)
    {
        cops->drawLine(COLOR_BLACK, (LCD_WIDTH/2), (LCD_HEIGHT/2),
                       xminute[pos2], yminute[pos2]);
    }

    pos = 90-minute;
    if(pos >= 60)
        pos -= 60;
    pos2 = 90-last_m;
    if(pos2 >= 60)
        pos2 -= 60;

    /* minute hand */
    cops->drawLine(COLOR_WHITE, LCD_WIDTH/2, LCD_HEIGHT/2,
                        xminute[pos], yminute[pos]);
    cops->drawLine(COLOR_WHITE, LCD_WIDTH/2-1, LCD_HEIGHT/2-1,
                        xminute[pos], yminute[pos]);
    cops->drawLine(COLOR_WHITE, LCD_WIDTH/2+1, LCD_HEIGHT/2+1,
                        xminute[pos], yminute[pos]);
    cops->drawLine(COLOR_WHITE, LCD_WIDTH/2-1, LCD_HEIGHT/2+1,
                        xminute[pos], yminute[pos]);
    cops->drawLine(COLOR_WHITE, LCD_WIDTH/2+1, LCD_HEIGHT/2-1,
                        xminute[pos], yminute[pos]);
    /* clear minutehand trails */
    if(minute != last_m)
    {
        cops->drawLine(COLOR_BLACK, LCD_WIDTH/2, LCD_HEIGHT/2,
                            xminute[pos2], yminute[pos2]);
        cops->drawLine(COLOR_BLACK, LCD_WIDTH/2-1, LCD_HEIGHT/2-1,
                            xminute[pos2], yminute[pos2]);
        cops->drawLine(COLOR_BLACK, LCD_WIDTH/2+1, LCD_HEIGHT/2+1,
                            xminute[pos2], yminute[pos2]);
        cops->drawLine(COLOR_BLACK, LCD_WIDTH/2-1, LCD_HEIGHT/2+1,
                            xminute[pos2], yminute[pos2]);
        cops->drawLine(COLOR_BLACK, LCD_WIDTH/2+1, LCD_HEIGHT/2-1,
                            xminute[pos2], yminute[pos2]);
    }

    if(hour2 > 12)
        hour2 -= 12;

    hour2 = hour2*5 + minute/12;
    pos = 90-hour2;
    if(pos >= 60)
        pos -= 60;

    /* hour hand */
    cops->drawLine(COLOR_WHITE, LCD_WIDTH/2, LCD_HEIGHT/2, xhour[pos], yhour[pos]);
    cops->drawLine(COLOR_WHITE, LCD_WIDTH/2-1, LCD_HEIGHT/2-1,
                        xhour[pos], yhour[pos]);
    cops->drawLine(COLOR_WHITE, LCD_WIDTH/2+1, LCD_HEIGHT/2+1,
                        xhour[pos], yhour[pos]);
    cops->drawLine(COLOR_WHITE, LCD_WIDTH/2-1, LCD_HEIGHT/2+1,
                        xhour[pos], yhour[pos]);
    cops->drawLine(COLOR_WHITE, LCD_WIDTH/2+1, LCD_HEIGHT/2-1,
                        xhour[pos], yhour[pos]);
    /* clear hourhand trails */
    if(hour != last_h)
    {
        cops->drawLine(COLOR_BLACK, LCD_WIDTH/2, LCD_HEIGHT/2, xhour[pos2], yhour[pos2]);
        cops->drawLine(COLOR_BLACK, LCD_WIDTH/2-1, LCD_HEIGHT/2-1,
                            xhour[pos2], yhour[pos2]);
        cops->drawLine(COLOR_BLACK, LCD_WIDTH/2+1, LCD_HEIGHT/2+1,
                            xhour[pos2], yhour[pos2]);
        cops->drawLine(COLOR_BLACK, LCD_WIDTH/2-1, LCD_HEIGHT/2+1,
                            xhour[pos2], yhour[pos2]);
        cops->drawLine(COLOR_BLACK, LCD_WIDTH/2+1, LCD_HEIGHT/2-1,
                            xhour[pos2], yhour[pos2]);
    }

    /* draw circle */
    for(i=0; i < 60; i++)
    {
        cops->drawPixel(COLOR_WHITE, xminute[i], yminute[i]);
        if(i < (60/5))
        cops->fillRect(COLOR_WHITE, xminute[i*5]-1, yminute[i*5]-1, 3, 3);
    }

    /* draw cover */
    cops->drawLine(COLOR_WHITE, (LCD_WIDTH/2)-1, (LCD_HEIGHT/2)+3,
                        (LCD_WIDTH/2)+1, (LCD_HEIGHT/2)+3);
    cops->drawLine(COLOR_WHITE, (LCD_WIDTH/2)-3, (LCD_HEIGHT/2)+2,
                        (LCD_WIDTH/2)+3, (LCD_HEIGHT/2)+2);
    cops->drawLine(COLOR_WHITE, (LCD_WIDTH/2)-4, (LCD_HEIGHT/2)+1,
                        (LCD_WIDTH/2)+4, (LCD_HEIGHT/2)+1);
    cops->drawLine(COLOR_WHITE, (LCD_WIDTH/2)-4, LCD_HEIGHT/2,
                        (LCD_WIDTH/2)+4, LCD_HEIGHT/2);
    cops->drawLine(COLOR_WHITE, (LCD_WIDTH/2)-4, (LCD_HEIGHT/2)-1,
                        (LCD_WIDTH/2)+4, (LCD_HEIGHT/2)-1);
    cops->drawLine(COLOR_WHITE, (LCD_WIDTH/2)-3, (LCD_HEIGHT/2)-2,
                        (LCD_WIDTH/2)+3, (LCD_HEIGHT/2)-2);
    cops->drawLine(COLOR_WHITE, (LCD_WIDTH/2)-1, (LCD_HEIGHT/2)-3,
                        (LCD_WIDTH/2)+1, (LCD_HEIGHT/2)-3);
}

void clock(void)
{
    cops->getTime(&current_time);
    hour = current_time.tm_hour;
    hour2 = current_time.tm_hour;
    minute = current_time.tm_min;
    second = current_time.tm_sec;

    if(settings.clock == 1)
        draw_analog();

    cops->putS(COLOR_WHITE, COLOR_BLACK, 275, 45, "Quit");

    sprintf(time, "%02d:%02d:%02d", hour, minute, second);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 10, time);

    last_s = second;
    last_m = minute;
    last_h = hour;
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
            RELEASE(cops); /* we're done */
            break;
    }
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
    cops->setFont(STD8X13);

    reset_settings();

    /* These two functions REQUIRED */
    /* Usage: PACK(cops, <main function>)*/
    PACK(cops, clock);
    /* Stay here in the plugin */
    STOPME(cops);
}