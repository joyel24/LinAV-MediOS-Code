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

#define FORMAT_MMDDYYYY    0
#define FORMAT_DDMMYYYY    1

#define FORMAT_12          0
#define FORMAT_24          1

NEED_ICON(linavLogo);
NEED_ICON(usbIcon);
NEED_ICON(fwExtIcon);
NEED_ICON(cfIcon);
NEED_ICON(powerIcon);

int batteryRefresh=0;
int batteryRefreshValue = 10;
int pwrState=0;
int usbState=0;
int fwExtState=0;
int cfState=0;
int power = 0;
int color = 0;
int level = 0;
int chargeProgress = 0;

extern struct plugin status_bar_plugin;

void showSBar(void)  {status_bar_plugin.handle_on=1;sendEvt(&status_bar_plugin,EVT_REDRAW);}
void hideSBar(void)  {status_bar_plugin.handle_on=0;}
int  sBarStatus(void) {return status_bar_plugin.handle_on;}

int date_format=FORMAT_DDMMYYYY;
int time_format=FORMAT_24;

#define HOUR(HH)       (time_format==FORMAT_12?(HH<=12?HH:(HH-12)):HH)
#define AMPM_ADD(HH)   time_format==FORMAT_12?HH<=12?"A":"P":""
#define DATE1(DD,MM)   (date_format==FORMAT_DDMMYYYY?DD:MM)
#define DATE2(DD,MM)   (date_format==FORMAT_DDMMYYYY?MM:DD)

/* draw the current time */
void drawTime(void)
{
    char timeSt[50];
    struct av_tm  date_time;
    
    if(getTime(&date_time))
    {    
        fillRect(BG_COLOR,135,3,106,10);
        
        sprintf(timeSt,"%02d%s:%02d %02d/%02d/%04d",HOUR(date_time.tm_hour),AMPM_ADD(date_time.tm_hour),date_time.tm_min,
                                                         DATE1(date_time.tm_mday,date_time.tm_mon),
                                                         DATE2(date_time.tm_mday,date_time.tm_mon),
                                                         date_time.tm_year);
        putS(TIME_COLOR,BG_COLOR,135,3,timeSt);
    }

}

void drawBat(void)
{
    if(pwrState == 0)
        batteryRefreshValue = 10;
    else
        batteryRefreshValue = 0;

    if(pwrState == 0)
    {
        power = getBat();
        if(power < 1320)
            color = COLOR_DARK_RED;
        else if(power < 1400)
            color = COLOR_RED;
        else if(power < 1480)
            color = COLOR_ORANGE2;
        else
            color = COLOR_GREEN;

        if(power > 1200)
            level = (int)(power - 1300) / 15;
        if(level > 20)
            level = 20;
    }
    else if(pwrState == 1)
    {
        if(chargeProgress == 0)
            level = 0;
        else if(chargeProgress == 1)
            level = 7;
        else if(chargeProgress == 2)
            level = 14;
        else
            level = 20;

        if(chargeProgress < 3)
            chargeProgress++;
        else
            chargeProgress = 0;

        color = COLOR_GREEN;
    }

    drawRect(COLOR_BLACK,293,2,22,10);
    fillRect(COLOR_BLACK,315,4,3,6);
    fillRect(BG_COLOR,294,3,20,8);
    fillRect(color,294,3,level,8);
    
    if(fmIsConnected()) /* show bat status on FM remote */
    {
        if(level<7)
            fmSetBat(1);
        else if(level<14)
            fmSetBat(2);
        else if(level<20)
            fmSetBat(3);
        else
            fmSetBat(4);
            
    }
    
}

void drawStatus(void)
{
    if(fwExtState && cfState)
    {
        fwExtState=0;
        cfState=0;
        fprintf(stderr,"FW and CF detected !!!\n");
    }
    
    if(fwExtState)
        drawBITMAP(&fwExtIcon, 242, 4);
    /*else
        fillRect(BG_COLOR,242,4,15,6);*/
        
    if(cfState)
        drawBITMAP(&cfIcon, 242, 4);
    /*else
        fillRect(BG_COLOR,242,4,15,6);*/
    if(!cfState && !fwExtState)
        fillRect(BG_COLOR,242,4,15,6);
    
        
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
            if(batteryRefresh < batteryRefreshValue)
                batteryRefresh++;
            else
                batteryRefresh = 0;
            break;
        case EVT_PWR:
            pwrState=getPwr();
            batteryRefresh = 0;
            drawStatus();
            break;
        case EVT_USB:
            usbState=getUSB();
            drawStatus();
            break;
        case EVT_FW_EXT:
            fwExtState=getFwExt();
            drawStatus();
            break;
        case EVT_CF_IN:
        case EVT_CF_OUT:
            cfState=CF_mod_is_connected();
            drawStatus();
            break;
        
    }
    return 1;
}

void ini_status_bar(struct plugin * status_plugin)
{
    pwrState=getPwr();
    usbState=getUSB();
    fwExtState=getFwExt();
    cfState=CF_mod_is_connected();
    doRegisterPlugin(status_plugin,statusEvtHandler,0);
}
