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

extern struct plugin settings_plugin;

needFont(std6x9);
needFont(std7x13);

#define SLIDER_OFFSET 110

int stopSettingsLoop = 0; /* global variable used to stop the private evt loop*/

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

#define CNT_SETTINGS_ENTRIES 4
/*                                                    tab, x,  y, label string,       min,max,inc,val,act,changed */
struct SettingsDataT sData[CNT_SETTINGS_ENTRIES] = { {  0, 5, 30, "Key Repeat",         1, 10,  1,  5,  1, 0},
                                                     {  0, 5, 44, "Key Freq",           0,  6,  1,  5,  0, 0},
                                                     {  0, 5, 58, "LCD Bat Timeout",    1,180, 10,  1,  0, 0},
                                                     {  0, 5, 72, "LCD DC Timeout",     1,180, 10,  1,  0, 0} };


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


int GetActiveSetting()
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

void GetSettings()
{
    // get actual settings
    sData[0].value = get_mouseRepeat();
    sData[1].value = get_mouseFreq();

    sData[2].value = get_lcd_TimeOutParam (AV_TIMER_ON_BAT);
    sData[3].value = get_lcd_TimeOutParam (AV_TIMER_ON_DC);


    set_mouseParam(6,3); // set to 6,3 for the settings screen only
}

void SetSettings()
{
    if((sData[0].changed) || (sData[1].changed))
        set_mouseParam(sData[1].value,sData[0].value);

    if(sData[2].changed)
        set_lcd_TimeOutParam(AV_TIMER_ON_BAT, sData[2].value);

    if(sData[3].changed)
        set_lcd_TimeOutParam(AV_TIMER_ON_DC, sData[3].value);
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

        case BTN_DOWN:
            active = GetActiveSetting();
            sData[active].active = 0;
            drawSlider(sData[active].x,
                       sData[active].y,
                       sData[active].text,
                       sData[active].min,
                       sData[active].max,
                       sData[active].value,
                       sData[active].active);

            if(active >= CNT_SETTINGS_ENTRIES-1)
                active = 0;
            else
                active++;

            sData[active].active = 1;

            drawSlider(sData[active].x,
                       sData[active].y,
                       sData[active].text,
                       sData[active].min,
                       sData[active].max,
                       sData[active].value,
                       sData[active].active);

            break;

        case BTN_UP:
            active = GetActiveSetting();
            sData[active].active = 0;
            drawSlider(sData[active].x,
                       sData[active].y,
                       sData[active].text,
                       sData[active].min,
                       sData[active].max,
                       sData[active].value,
                       sData[active].active);

            if(active == 0)
                active = CNT_SETTINGS_ENTRIES-1;
            else
                active--;

            sData[active].active = 1;

            drawSlider(sData[active].x,
                       sData[active].y,
                       sData[active].text,
                       sData[active].min,
                       sData[active].max,
                       sData[active].value,
                       sData[active].active);

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
}

void drawSlider(int x, int y, char* text, int min, int max, int value, int active)
{
	int filler;
	char tmp[10];

    setPlane(BMAP2);
    setFont(std6x9);

	fillRect(COLOR_GREY, x, y-2, SCREEN_WIDTH-40, 13); // clear active settings range

    if(active)
        putS(COLOR_RED, COLOR_GREY, x, y, text);
    else
        putS(COLOR_BLACK, COLOR_GREY, x, y, text);


    filler = ((int)(((value-min) * 128) / (max-min) ));

	sprintf(tmp,"%d", min);
    putS(COLOR_BLACK, COLOR_GREY, x+SLIDER_OFFSET-15, y, tmp);
	sprintf(tmp,"%d", max);
    putS(COLOR_BLACK, COLOR_GREY, x+SLIDER_OFFSET+128+5, y, tmp);

	fillRect(COLOR_BLUE, x+SLIDER_OFFSET, y, 128, 9);

    drawBITMAP (&SettingsSliderBitmap, x+filler+SLIDER_OFFSET-3,y-2); // -3 = half of bitmap width

	sprintf(tmp,"%d", value);
    if(filler < 128/2)
        putS(COLOR_BLACK, COLOR_BLUE, x+filler+SLIDER_OFFSET+7, y, tmp);
    else
        putS(COLOR_BLACK, COLOR_BLUE, x+filler+SLIDER_OFFSET-15, y, tmp);

    setPlane(BMAP1);
}

void drawSettings()
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

    GetSettings();

    for(i = 0; i < CNT_SETTINGS_ENTRIES; i++)
    {
        drawSlider(sData[i].x, sData[i].y, sData[i].text, sData[i].min, sData[i].max, sData[i].value, sData[i].active);
    }

    showPlane(BMAP2);
    setPlane(BMAP1);
    setFont(std6x9);
}

void ini_settings()
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
int SettingsScreen()
{
    drawSettings();
    SettingsEvtLoop();
    eraseSettings();
    return 1;
}
