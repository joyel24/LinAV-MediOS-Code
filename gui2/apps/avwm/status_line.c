/*
* status_line.c
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/
#include <stdlib.h>
#include <stdio.h>

#include "colordef.h"
#include "cops.h"
#include "version.h"
#include "avevents.h"
#include "events.h"
#include "plugin.h"

int batteryRefresh=0;

/* draw the current time */
void drawTime(void)
{
    char timeSt[50];

    if(getTimeS(timeSt))
    {
        fillRect(COLOR_LIGHT_BLUE,150,2,130,11);
        putS(COLOR_DARK_GREY,COLOR_LIGHT_BLUE,150,3,timeSt);
    }

}

void drawBat(void)
{
    int power = 0;
    int color = 0;
    int level = 0;

    /* get batt levels and draw a meter */
    if(power=getBat())
    {
        if(power < 1320)
        {
            color = COLOR_DARK_RED;
            level = 4;
        }
        else if(power < 1380)
        {
            color = COLOR_RED;
            level = 8;
        }
        else if(power < 1440)
        {
            color = COLOR_ORANGE2;
            level = 12;
        }
        else if(power < 1500)
        {
            color = COLOR_LIGHT_YELLOW;
            level = 16;
        }
        else
        {
            color = COLOR_GREEN;
            level = 20;
        }
        
        //fprintf(stderr,"[BAT] P=%d,C=%d,L=%d\n",power,color,level);

        drawRect(COLOR_BLACK,289,2,22,10);
        fillRect(COLOR_BLACK,311,4,3,6);
        fillRect(color,290,3,level,8);
    }
    else
        fprintf(stderr,"[BAT] error getting bat level\n");

}

void drawGui(void)
{
    int w = 0;
    int h = 0;

    char myName[]="AvWm xx.xx";

    getStringS("M", &w, &h);

    /* blue background */
    fillRect(COLOR_LIGHT_BLUE,0,0,320,h+6);

    /* nice little shadow */
    drawLine(COLOR_BLACK,0,h+5,319,h+5);
    drawLine(COLOR_DARK_GREY,0,h+6,319,h+6);
    drawLine(COLOR_LIGHT_GREY,0,h+7,319,h+7);

    /* show version */
    sprintf(myName,"AvWm %d.%d",MAJOR_V,MINOR_V);
    putS(COLOR_DARK_GREY,COLOR_LIGHT_BLUE,2,3,myName);

    /* and time, and battery */
    drawTime();
    drawBat();
}

/* events */
int statusEvtHandler(int evt)
{
    switch (evt) {
        case EVT_REDRAW:
            drawGui();
            break;
        case EVT_TIMER:
            drawTime();
            if(batteryRefresh >= 20)
            {
                drawBat();
                batteryRefresh = 0;
            }
            else
                batteryRefresh++;
            break;
    }
}

void ini_status_bar(struct plugin * status_plugin)
{
    doRegisterPlugin(status_plugin,statusEvtHandler,0);
}