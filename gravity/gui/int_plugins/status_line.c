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

#include <sys_def/string.h>

//#include <evt.h>

#include <sys_def/colordef.h>
#include <graphics.h>

/*
#include <rtc.h>
#include <bat_power.h>
#include <fm_remote.h>
#include <usb_fw.h>
#include <cf.h>*/

#include <api.h>

#include <gui/avwm_version.h>
#include <gui/icons.h>

#define BG_COLOR  COLOR_LIGHT_BLUE
#define TXT_COLOR COLOR_DARK_GREY
#define TIME_COLOR COLOR_BLACK

#define FORMAT_MMDDYYYY    0
#define FORMAT_DDMMYYYY    1

#define FORMAT_12          0
#define FORMAT_24          1

BITMAP * st_fwExtIcon;
BITMAP * st_cfIcon;
BITMAP * st_usbIcon;
BITMAP * st_powerIcon;
BITMAP * st_linavLogo;

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
    
    if(getTime(&date_time)==0)
    {   
        sprintf(timeSt,"%02d:%02d%s %02d/%02d/%04d",HOUR(date_time.tm_hour),date_time.tm_min,AMPM_ADD(date_time.tm_hour),
                                                         DATE1(date_time.tm_mday,date_time.tm_mon),
                                                         DATE2(date_time.tm_mday,date_time.tm_mon),
                                                         date_time.tm_year);
        fillRect(BG_COLOR,135,3,106,10);
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
        power = getBatLevel();
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
 #if 0   
    if(FM_is_connected()) /* show bat status on FM remote */
    {
        if(level<7)
            FM_setIcon(FM_BAT,1);
        else if(level<14)
            FM_setIcon(FM_BAT,2);
        else if(level<20)
            FM_setIcon(FM_BAT,3);
        else
            FM_setIcon(FM_BAT,4);
            
    }
 #endif   
}

void drawStatus(void)
{
    if(fwExtState && cfState)
    {
        fwExtState=0;
        cfState=0;
        printf("[drawStatus] FW and CF detected !!!\n");
    }
    
    if(fwExtState)
        drawBITMAP(st_fwExtIcon, 242, 4);
    /*else
        fillRect(BG_COLOR,242,4,15,6);*/
        
    if(cfState)
        drawBITMAP(st_cfIcon, 242, 4);
    /*else
        fillRect(BG_COLOR,242,4,15,6);*/
    if(!cfState && !fwExtState)
        fillRect(BG_COLOR,242,4,15,6);
    
        
    if(usbState)
        drawBITMAP(st_usbIcon, 260, 4);
    else
        fillRect(BG_COLOR,260,4,15,6);

    if(pwrState)
        drawBITMAP(st_powerIcon, 278, 4);
    else
        fillRect(BG_COLOR,278,4,15,6);
}

void drawLogo(void)
{
    drawBITMAP(st_linavLogo, 2, 2);
}

void drawStatusLine(void)
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

#if 0
/* events */
void statusEvtHandler(int evt)
{
    switch (evt) {
        case EVT_REDRAW:
            drawStatusLine();
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
            pwrState=powerConnected();
            batteryRefresh = 0;
            drawStatus();
            break;
        case EVT_USB:
            usbState=usbIsConnected();
            drawStatus();
            break;
        case EVT_FW_EXT:
            fwExtState=FWIsConnected();
            drawStatus();
            break;
        case EVT_CF_IN:
        case EVT_CF_OUT:
            cfState=cfIsConnected();
            drawStatus();
            break;
        
    }
}

struct evt_listener_s status_line_listener;
#endif

void ini_status_bar(void)
{
    /* get icons */
    st_fwExtIcon=&getIcon("fwExtIcon")->bmap_data;
    st_cfIcon=&getIcon("cfIcon")->bmap_data;
    st_usbIcon=&getIcon("usbIcon")->bmap_data;
    st_powerIcon=&getIcon("powerIcon")->bmap_data;
    st_linavLogo=&getIcon("linavLogo")->bmap_data;
        
    /* get initial status */
    pwrState=powerConnected();
    usbState=usbIsConnected();
    fwExtState=FWIsConnected();
    /*cfState=cfIsConnected(); */
    cfState=0;
    
    #if 0
    /* register to evt loop */
    status_line_listener.action=statusEvtHandler;
    add_evt_listener(&status_line_listener);
    #endif
}
