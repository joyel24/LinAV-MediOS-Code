/*
* settings.c
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Goetz Minuth
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
#include "av3xx_common.h"
#include "settings.h"

extern struct plugin settings_plugin;

needFont(std6x9);
needFont(std7x13);

#define SLIDER_OFFSET 115

int stopSettingsLoop = 0; /* global variable used to stop the private evt loop*/
int activeTab = 0;
struct av_tm date_time_setting;

void drawTabs();
void drawParameter();

struct SettingsDataT {
    int  tab;
    int  x;
    int  y;
    char* text;
    int  min;
    int  max;
    int  inc;
    int  value;
    int  active;
    int  changed;
};

struct TabNamesT {
    char* tabname;
    int   tab;
};

#define CNT_SETTINGS_TABS 3
struct TabNamesT tabData[CNT_SETTINGS_TABS] = { { "Screen", 0 },
                                                { "Timeout",  1 },
                                                { "Time/Date",   2 } };

#define CNT_SETTINGS_ENTRIES 16
/*                                                    tab, x,  y, label string,       min,max,inc,val,act,changed */
struct SettingsDataT sData[CNT_SETTINGS_ENTRIES] = { {  0, 5, 50, "Key Repeat",         1, 10,  1,  5,  1, 0},
                                                     {  0, 5, 64, "Key Freq",           0,  6,  1,  5,  0, 0},
                                                     {  0, 5, 92, "Contrast",           1, 10,  1,  5,  0, 0},
                                                     {  1, 5, 50, "LCD Bat",            1,180, 10,  1,  0, 0},
                                                     {  1, 5, 64, "LCD DC",             1,180, 10,  1,  0, 0},
                                                     {  1, 5, 78, "Power Bat",          1,180, 10,  1,  0, 0},
                                                     {  1, 5, 92, "Power DC ",          1,180, 10,  1,  0, 0},
                                                     {  1, 5,106, "HD Bat",             1,180, 10,  1,  0, 0},
                                                     {  1, 5,120, "HD DC",              1,180, 10,  1,  0, 0},
                                                     {  2, 5, 50, "Day",                1, 31,  1,  1,  0, 0},
                                                     {  2, 5, 64, "Month",              1, 12,  1,  1,  0, 0},
                                                     {  2, 5, 78, "Year",            2004,2020,  1, 2004,  0, 0},
                                                     {  2, 5, 92, "Hours",              1, 24,  1,  1,  0, 0},
                                                     {  2, 5,106, "Minutes",            1, 60,  1,  1,  0, 0},
                                                     {  2, 5,120, "Seconds",            1, 60,  1,  1,  0, 0},
                                                     {  2, 5,134, "Format",             1, 2,   1,  1,  0, 0} };


unsigned char SettingsSlider[13][2] =
{   {12,12},
    {12,12},
    {12,12},
    {12,12},
    {12,12},
    {12,12},
    {12,12},
    {12,12},
    {12,12},
    {12,12},
    {12,12},
    {12,12},
    {12,12},
};
BITMAP SettingsSliderBitmap = {(unsigned int) SettingsSlider, 2, 13, 0, 0};


int GetActiveSetting(void)
{
    int i = 0;

    for(i = 0; i < CNT_SETTINGS_ENTRIES; i++)
    {
        if(sData[i].active == 1)
        {
            return i;
        }
    }

    return -1;
}

/*our private loop*/
void SettingsEvtLoop(void)
{
    settings_plugin.handle_on=1;
    stopSettingsLoop=0;
    while(!stopSettingsLoop)
        procNxtEvent(waitEvent());
    settings_plugin.handle_on=0;
}

void GetSettings(void)
{
    // get actual settings
    sData[0].value = get_mouseRepeat();
    sData[1].value = get_mouseFreq();
    sData[2].value = getBrightness();

    sData[3].value = get_lcd_TimeOutParam (AV_TIMER_ON_BAT);
    sData[4].value = get_lcd_TimeOutParam (AV_TIMER_ON_DC);

    sData[5].value = get_halt_TimeOutParam (AV_TIMER_ON_BAT);
    sData[6].value = get_halt_TimeOutParam (AV_TIMER_ON_DC);

    sData[7].value = get_hd_TimeOutParam (AV_TIMER_ON_BAT);
    sData[8].value = get_hd_TimeOutParam (AV_TIMER_ON_DC);

    getTime(&date_time_setting);
    sData[9].value  = date_time_setting.tm_mday;
    sData[10].value = date_time_setting.tm_mon;
    sData[11].value = date_time_setting.tm_year;
    sData[12].value = date_time_setting.tm_hour;
    sData[13].value = date_time_setting.tm_min;
    sData[14].value = date_time_setting.tm_sec;
    sData[15].value = 1;

    set_mouseParam(6,3); // set to 6,3 for the settings screen only
}

void SetSettings(void)
{
    if((sData[0].changed) || (sData[1].changed))
        set_mouseParam(sData[1].value,sData[0].value);

    if(sData[2].changed)
        setBrightness(sData[2].value);

    if(sData[3].changed)
        set_lcd_TimeOutParam(AV_TIMER_ON_BAT, sData[3].value);

    if(sData[4].changed)
        set_lcd_TimeOutParam(AV_TIMER_ON_DC, sData[4].value);

    if(sData[5].changed)
        set_halt_TimeOutParam(AV_TIMER_ON_BAT, sData[5].value);

    if(sData[6].changed)
        set_halt_TimeOutParam(AV_TIMER_ON_DC, sData[6].value);

    if(sData[7].changed)
        set_hd_TimeOutParam(AV_TIMER_ON_BAT, sData[7].value);

    if(sData[8].changed)
        set_hd_TimeOutParam(AV_TIMER_ON_DC, sData[8].value);

    if(sData[9].changed)
        date_time_setting.tm_mday = sData[9].value;
    if(sData[10].changed)
        date_time_setting.tm_mon = sData[10].value;
    if(sData[11].changed)
        date_time_setting.tm_year = sData[11].value;
    if(sData[12].changed)
        date_time_setting.tm_hour = sData[12].value;
    if(sData[13].changed)
        date_time_setting.tm_min = sData[13].value;
    if(sData[14].changed)
        date_time_setting.tm_sec = sData[14].value;

    if( (sData[9].changed  == 1) ||
        (sData[10].changed == 1) ||
        (sData[11].changed == 1) ||
        (sData[12].changed == 1) ||
        (sData[13].changed == 1) ||
        (sData[14].changed == 1) )
    {
        setTime(&date_time_setting);
    }
}

int GetNextValidParameter()
{
    int active = 0;
    int oldActive = 0;

    active = GetActiveSetting();
    oldActive = active;

    do
    {
        active++;

        if(active >= CNT_SETTINGS_ENTRIES)
            active = 0;
    }
    while(sData[active].tab != activeTab);

    sData[oldActive].active = 0;

    drawSlider(sData[oldActive].x,
            sData[oldActive].y,
            sData[oldActive].text,
            sData[oldActive].min,
            sData[oldActive].max,
            sData[oldActive].value,
            sData[oldActive].active);

    sData[active].active = 1;

    drawSlider(sData[active].x,
            sData[active].y,
            sData[active].text,
            sData[active].min,
            sData[active].max,
            sData[active].value,
            sData[active].active);

    return active;
}

int GetPrevValidParameter()
{
    int active = 0;
    int oldActive = 0;

    active = GetActiveSetting();
    oldActive = active;

    do
    {
        active--;

        if(active < 0)
            active = CNT_SETTINGS_ENTRIES-1;
     }
    while(sData[active].tab != activeTab);

    sData[oldActive].active = 0;

    drawSlider(sData[oldActive].x,
            sData[oldActive].y,
            sData[oldActive].text,
            sData[oldActive].min,
            sData[oldActive].max,
            sData[oldActive].value,
            sData[oldActive].active);

    sData[active].active = 1;

    drawSlider(sData[active].x,
            sData[active].y,
            sData[active].text,
            sData[active].min,
            sData[active].max,
            sData[active].value,
            sData[active].active);

    return active;
}

/* events */
int settingsEvtHandler(int evt)
{
    int active = 0;

    switch (evt) {
        case EVT_REDRAW:
            break;
        case EVT_TIMER:
            break;

        case BTN_F1:
            activeTab--;
            if(activeTab < 0)
                activeTab = CNT_SETTINGS_TABS-1;

            drawTabs();
            setNewActiveparam();
            drawParameter();
            break;

        case BTN_F2:
            activeTab++;
            if(activeTab >= CNT_SETTINGS_TABS)
                activeTab = 0;

            drawTabs();
            setNewActiveparam();
            drawParameter();
            break;

        case BTN_DOWN:

            GetNextValidParameter();
            break;

        case BTN_UP:

            GetPrevValidParameter();
            break;

        case BTN_LEFT:
            active = GetActiveSetting();
            sData[active].value-=sData[active].inc;
            if(sData[active].value < sData[active].min)
                sData[active].value = sData[active].min;

            drawSlider(sData[active].x,
                       sData[active].y,
                       sData[active].text,
                       sData[active].min,
                       sData[active].max,
                       sData[active].value,
                       sData[active].active);

            sData[active].changed = 1;
            break;

        case BTN_RIGHT:
            active = GetActiveSetting();
            sData[active].value+=sData[active].inc;
            if(sData[active].value > sData[active].max)
                sData[active].value = sData[active].max;            

            drawSlider(sData[active].x,
                       sData[active].y,
                       sData[active].text,
                       sData[active].min,
                       sData[active].max,
                       sData[active].value,
                       sData[active].active);

            sData[active].changed = 1;
            break;

        case BTN_OFF:
            SetSettings();
            stopSettingsLoop=1;
            break;
    }
    return 1;
}

void drawSlider(int x, int y, char* text, int min, int max, int value, int active)
{
	int filler;
	char tmp[10];

    setPlane(BMAP2);
    setFont(std6x9);

	fillRect(COLOR_LIGHT_GREY, x, y-2, SCREEN_WIDTH-40, 13); // clear active settings range

    if(active)
        putS(COLOR_RED, COLOR_LIGHT_GREY, x, y, text);
    else
        putS(COLOR_BLACK, COLOR_LIGHT_GREY, x, y, text);


    filler = ((int)(((value-min) * 128) / (max-min) ));

	sprintf(tmp,"%d", min);
    putS(COLOR_BLACK, COLOR_LIGHT_GREY, x+SLIDER_OFFSET-25, y, tmp);
	sprintf(tmp,"%d", max);
    putS(COLOR_BLACK, COLOR_LIGHT_GREY, x+SLIDER_OFFSET+128+5, y, tmp);

	fillRect(COLOR_BLUE, x+SLIDER_OFFSET, y, 128, 9);

    drawBITMAP (&SettingsSliderBitmap, x+filler+SLIDER_OFFSET,y-2);

	sprintf(tmp,"%d", value);
    if(filler < 128/2)
        putS(COLOR_BLACK, COLOR_BLUE, x+filler+SLIDER_OFFSET+7, y, tmp);
    else
        putS(COLOR_BLACK, COLOR_BLUE, x+filler+SLIDER_OFFSET-25, y, tmp);

    setPlane(BMAP1);
}

void drawTabs()
{
    unsigned long i = 0;

    setPlane(BMAP2);
    setFont(std7x13);

    for(i = 0; i < CNT_SETTINGS_TABS; i++)
    {
        if(activeTab == i)
        {
            fillRect(COLOR_RED, 5+80*i, 25, 80, 15);
            putS(COLOR_BLACK, COLOR_RED, 5+80*i+2, 25, tabData[i].tabname);
        }
        else
        {
            fillRect(COLOR_GREY, 5+80*i, 25, 80, 15);
            putS(COLOR_BLACK, COLOR_GREY, 5+80*i+2, 25, tabData[i].tabname);
        }
    }

    setPlane(BMAP1);
    setFont(std6x9);
}

void setNewActiveparam()
{
    unsigned long i = 0;
    int active = 0;

    active = GetActiveSetting();
    sData[active].active = 0;

    for(i = 0; i < CNT_SETTINGS_ENTRIES; i++)
    {
        if(activeTab == sData[i].tab)
        {
            sData[i].active = 1;
            return;
        }
    }
}

void drawParameter()
{
    int i = 0;

    setPlane(BMAP2);

    fillRect(COLOR_LIGHT_GREY, 0, 45, 290, 140);

    for(i = 0; i < CNT_SETTINGS_ENTRIES; i++)
    {
        if(activeTab == sData[i].tab)
            drawSlider(sData[i].x, sData[i].y, sData[i].text, sData[i].min, sData[i].max, sData[i].value, sData[i].active);
    }

    setPlane(BMAP1);
}

void drawSettings(void)
{
    int i = 0;

    setSize(BMAP2,SCREEN_WIDTH-40,SCREEN_HEIGHT-60, 8);
    setPos(BMAP2,0x14+40,0x13+20);

    // show box
    setPlane(BMAP2);
    setFont(std7x13);

    clearScreen(COLOR_GREY);

    drawLine(COLOR_RED, 0, 2, SCREEN_WIDTH-40, 2);
    drawLine(COLOR_RED, 0, 4, SCREEN_WIDTH-40, 4);
    drawLine(COLOR_RED, 0, 6, 95, 6); drawLine(COLOR_RED, 162, 6, SCREEN_WIDTH-40, 6);
    drawLine(COLOR_RED, 0, 8, 95, 8); drawLine(COLOR_RED, 162, 8, SCREEN_WIDTH-40, 8);
    drawLine(COLOR_RED, 0,10, 95,10); drawLine(COLOR_RED, 162,10, SCREEN_WIDTH-40,10);
    drawLine(COLOR_RED, 0,12, 95,12); drawLine(COLOR_RED, 162,12, SCREEN_WIDTH-40,12);
    drawLine(COLOR_RED, 0,14, 95,14); drawLine(COLOR_RED, 162,14, SCREEN_WIDTH-40,14);
    drawLine(COLOR_RED, 0,16, 95,16); drawLine(COLOR_RED, 162,16, SCREEN_WIDTH-40,16);
    putS(COLOR_BLACK, COLOR_GREY, 100, 5, "AVWM Settings");
    drawLine(COLOR_RED, 0, 18, SCREEN_WIDTH-40, 18);
    drawLine(COLOR_RED, 0, 20, SCREEN_WIDTH-40, 20);

    drawTabs();

    GetSettings();

    drawParameter();

    showPlane(BMAP2);
    setPlane(BMAP1);
    setFont(std6x9);
}

void ini_settings(void)
{
    doRegisterPlugin(&settings_plugin,settingsEvtHandler,0);
}

/* restore the previous state */
void eraseSettings(void)
{
    hidePlane(BMAP2);
    setPlane(BMAP1);
    setFont(std6x9);
}

/* main function */
int SettingsScreen(void)
{
    drawSettings();
    SettingsEvtLoop();
    eraseSettings();
    return 1;
}
