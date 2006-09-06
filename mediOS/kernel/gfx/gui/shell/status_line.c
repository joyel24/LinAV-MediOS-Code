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

#include <kernel/status_line.h>

#include <kernel/kernel.h>
#include <kernel/version.h>
#include <kernel/graphics.h>
#include <kernel/rtc.h>
#include <kernel/bat_power.h>
#include <kernel/usb_fw.h>
#include <kernel/icons.h>

#include <sys_def/colordef.h>
#include <sys_def/font.h>
#include <sys_def/string.h>

#include <evt.h>

#ifdef HAVE_EXT_MODULE
#include <kernel/cf_module.h>
#endif

#define FORMAT_MMDDYYYY    0
#define FORMAT_DDMMYYYY    1

#define FORMAT_12          0
#define FORMAT_24          1

BITMAP * st_fwExtIcon;
BITMAP * st_cfIcon;
BITMAP * st_usbIcon;
BITMAP * st_powerIcon;
BITMAP * st_mediosLogo;

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

extern struct statusLine_data lineData;

int clk_w=0,clk_h=0;

/* draw the current time */
void drawTime(void)
{
    char timeSt[50];
    struct med_tm  date_time;

    if(rtc_getTime(&date_time)==MED_OK)
    {
        if(lineData.has_date)
            sprintf(timeSt,"%02d:%02d%s %02d/%02d/%04d",HOUR(date_time.tm_hour),date_time.tm_min,
                    AMPM_ADD(date_time.tm_hour),
                    DATE1(date_time.tm_mday,date_time.tm_mon),
                    DATE2(date_time.tm_mday,date_time.tm_mon),
                    date_time.tm_year);
        else
            sprintf(timeSt,"%02d:%02d%s",HOUR(date_time.tm_hour),date_time.tm_min,
                    AMPM_ADD(date_time.tm_hour));
        gfx_fillRect(lineData.bg_color,lineData.clk_x,lineData.clk_y,clk_w,clk_h);
        gfx_getStringSize(timeSt,&clk_w,&clk_h);
        gfx_fontSet(STD6X9);
        gfx_putS(lineData.clk_color,lineData.bg_color,lineData.clk_x,lineData.clk_y,timeSt);
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
        power = batLevel();

        if(power < 2)
            color = COLOR_DARK_RED;
        else if(power < 4)
            color = COLOR_RED;
        else if(power < 6)
            color = COLOR_ORANGE2;
        else
            color = COLOR_GREEN;

        if(power > 2)
            level = (int)(power - 1) * 3;
        if(level > 21)
            level = 21;
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
            level = 21;

        if(chargeProgress < 3)
            chargeProgress++;
        else
            chargeProgress = 0;

        color = COLOR_GREEN;
    }

    gfx_drawRect(COLOR_BLACK,lineData.bat_x,lineData.bat_y,23,10);
    gfx_fillRect(COLOR_BLACK,lineData.bat_x+23,lineData.bat_y+2,3,6);
    gfx_fillRect(lineData.bg_color,lineData.bat_x+1,lineData.bat_y+1,21,8);
    gfx_fillRect(color,lineData.bat_x+1,lineData.bat_y+1,level,8);
#if 0
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
#endif
}

void drawStatus(void)
{
    if(fwExtState && cfState)
    {
        fwExtState=0;
        cfState=0;
        printk("FW and CF detected !!!\n");
    }
    
    if(fwExtState)
        gfx_drawBitmap(st_fwExtIcon, lineData.module_x, lineData.module_y);
    /*else
        fillRect(lineData.bg_color,242,4,15,6);*/
        
    if(cfState)
        gfx_drawBitmap(st_cfIcon, lineData.module_x, lineData.module_y);
    /*else
        fillRect(lineData.bg_color,242,4,15,6);*/
    if(!cfState && !fwExtState)
        gfx_fillRect(lineData.bg_color,lineData.module_x,lineData.module_y,15,6);
    
        
    if(pwrState)
        gfx_drawBitmap(st_powerIcon, lineData.pwr_x, lineData.pwr_y);
    else
        gfx_fillRect(lineData.bg_color,lineData.pwr_x,lineData.pwr_y,15,6);

    if(usbState)
        gfx_drawBitmap(st_usbIcon, lineData.usb_x, lineData.usb_y);
    else
        gfx_fillRect(lineData.bg_color,lineData.usb_x,lineData.usb_y,15,6);
}

void drawLogo(void)
{
    gfx_drawBitmap(st_mediosLogo, lineData.logo_x, lineData.logo_y);
}

void drawGui(void)
{
    int w = 0;
    int h = 0;
    char medios_ver[10];

    gfx_fontSet(STD6X9);

    gfx_getStringSize("M", &w, &h);

    /* blue background */
    gfx_fillRect(lineData.bg_color,lineData.x,lineData.y,lineData.w,h+6);

    /* nice little shadow */
    gfx_drawLine(COLOR_BLACK,lineData.x,lineData.y+h+5,lineData.w-1,h+5);
    gfx_drawLine(COLOR_DARK_GREY,lineData.x+0,lineData.y+h+6,lineData.w-1,h+6);
    gfx_drawLine(COLOR_LIGHT_GREY,lineData.x+0,lineData.y+h+7,lineData.w-1,h+7);

    /* show version */
    sprintf(medios_ver,"%d.%d",VER_MAJOR,VER_MINOR);
    gfx_putS(lineData.ver_color,lineData.bg_color,lineData.ver_x,lineData.ver_y,medios_ver);

    /* and time, and battery */
    drawTime();
    drawBat();
    drawStatus();
    drawLogo();
}

/* events */
void statusLine_handleEvent(int evt)
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
        case EVT_PWR_IN:
        case EVT_PWR_OUT:
#warning we could use here and for usb / FW ... the data of the evt
            pwrState=POWER_CONNECTED;
            batteryRefresh = 0;
            drawStatus();
            break;
        case EVT_USB_IN:
        case EVT_USB_OUT:
            usbState=kusbIsConnected();
            drawStatus();
            break;
        case EVT_FW_EXT_IN:
        case EVT_FW_EXT_OUT:
            fwExtState=kFWIsConnected();
            drawStatus();
            break;
#ifdef HAVE_EXT_MODULE
        case EVT_CF_IN:
        case EVT_CF_OUT:
            cfState=CF_IS_CONNECTED;
            drawStatus();
            break;    
#endif
    }
}

void statusLine_init(void)
{
    /* get icons */
    st_fwExtIcon=&icon_get("fwExtIcon")->bmap_data;
    st_cfIcon=&icon_get("cfIcon")->bmap_data;
    st_usbIcon=&icon_get("usbIcon")->bmap_data;
    st_powerIcon=&icon_get("powerIcon")->bmap_data;
    st_mediosLogo=&icon_get("mediosLogo")->bmap_data;
        
    pwrState=POWER_CONNECTED;
    usbState=kusbIsConnected();
    fwExtState=kFWIsConnected();
#ifdef HAVE_EXT_MODULE
    cfState=CF_IS_CONNECTED;
#else
    cfState=0;
#endif
}
