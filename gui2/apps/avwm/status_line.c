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
#include "icons.h"

#define BG_COLOR  COLOR_LIGHT_BLUE
#define TXT_COLOR COLOR_DARK_GREY
#define TIME_COLOR COLOR_BLACK
#define BATTERY_REFRESH_VALUE 20

NEED_ICONE(linavLogo)
NEED_ICONE(usbIcon)
NEED_ICONE(powerIcon)

int batteryRefresh=0;
int pwrState=0;
int usbState=0;

/* draw the current time */
void drawTime(void)
{
    char timeSt[50];

    if(getTimeS(timeSt))
    {
        //fprintf(stderr,"%s\n",timeSt);
        putS(TIME_COLOR,BG_COLOR,135,3,timeSt);
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

        drawRect(COLOR_BLACK,293,2,22,10);
        fillRect(COLOR_BLACK,315,4,3,6);
        fillRect(BG_COLOR,294,3,20,8);
        fillRect(color,294,3,level,8);
    }
    else
        fprintf(stderr,"[BAT] error getting bat level\n");

}

void drawStatus(void)
{
    if(usbState)
        drawBITMAP(&usbIcon, 260, 4);
    else
        fillRect(BG_COLOR,260,4,15,6);

    if(pwrState)
        drawBITMAP(&powerIcon, 278, 4);
    else
        fillRect(BG_COLOR,278,4,15,6);
}

void drawLogo(void)
{
    drawBITMAP(&linavLogo, 2, 2);
}

void drawGui(void)
{
    int w = 0;
    int h = 0;

    char avwm[10];

    getStringS("M", &w, &h);

    /* blue background */
    fillRect(BG_COLOR,0,0,320,h+6);

    /* nice little shadow */
    drawLine(COLOR_BLACK,0,h+5,319,h+5);
    drawLine(COLOR_DARK_GREY,0,h+6,319,h+6);
    drawLine(COLOR_LIGHT_GREY,0,h+7,319,h+7);

    /* show version */
    sprintf(avwm,"AvWm %d.%d",MAJOR_V,MINOR_V);
    putS(TXT_COLOR,BG_COLOR,75,3,avwm);

    /* and time, and battery */
    drawTime();
    drawBat();
    drawStatus();
    drawLogo();
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
            if(batteryRefresh == 0)
                drawBat();
            else
            {
                batteryRefresh++;
                if(batteryRefresh == BATTERY_REFRESH_VALUE)
                    batteryRefresh = 0; // Reset
            }

            break;
        case EVT_PWR:
            pwrState=getPwr();
            drawStatus();
            break;
        case EVT_USB:
            usbState=getUSB();
            drawStatus();
            break;
    }
}

void ini_status_bar(struct plugin * status_plugin)
{
    pwrState=getPwr();
    usbState=getUSB();
    doRegisterPlugin(status_plugin,statusEvtHandler,0);
}
