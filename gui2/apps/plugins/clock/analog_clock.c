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
#include "colordef.h"
#include "global.h"
#include "cops.h"

int pos;
int pos2;
int i;

char curr_time[10];

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