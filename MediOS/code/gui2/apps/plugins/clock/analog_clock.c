/*
* analog_clock.c
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
#include <stdio.h>

#include "colordef.h"
#include "global.h"
#include "cops.h"

int pos;
int pos2;
int i;

char curr_time[10];

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

/* Initialization, REQUIRED */
struct client_operations * cops;

/************************
 * Draws the analog clock
 ***********************/
void draw_analog(int hour, int minute, int second, int last_s, int last_m, int last_h)
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

    if(hour > 12)
        hour -= 12;

    hour = hour*5 + minute/12;
    pos = 90-hour;
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
        cops->drawLine(COLOR_BLACK, LCD_WIDTH/2, LCD_HEIGHT/2, xhour[pos2],
                            yhour[pos2]);
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
    for(i=0; i<60; i++)
    {
        cops->drawPixel(COLOR_WHITE, xminute[i], yminute[i]);
        if(i < (60/5))
        cops->fillRect(COLOR_WHITE, xminute[i*5]-1, yminute[i*5]-1, 3, 3);
    }

    /* draw cover */
    cops->drawLine(COLOR_WHITE, (LCD_WIDTH/2)-1 , (LCD_HEIGHT/2)+3,
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
    cops->drawLine(COLOR_WHITE, (LCD_WIDTH/2)-1 , (LCD_HEIGHT/2)-3,
                        (LCD_WIDTH/2)+1, (LCD_HEIGHT/2)-3);
}

void draw_analog_extras(int hour, int minute, int second)
{
    if(settings.analog_time == 1)
    {
        sprintf(curr_time, "%02d:%02d:%02d", hour, minute, second);
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 10, curr_time);
    }
    else if(settings.analog_time == 2)
    {
        sprintf(curr_time, "%02d:%02d:%02d %s", (hour>12)?(hour-12):(hour), minute, second, (hour>12)?("PM"):("AM"));
        cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 10, curr_time);
    }
}