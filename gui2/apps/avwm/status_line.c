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

#include "colordef.h"
#include "cops.h"
#include "version.h"
#include "avevents.h"
#include "events.h"
#include "plugin.h"

int batteryRefresh=0;

void drawTime()
{
    char timeSt[50];
    
    if(getTimeS(timeSt))
    {
    	fillRect(COLOR_LIGHT_BLUE,150,2,130,11);
    	putS(COLOR_DARK_GREY,COLOR_LIGHT_BLUE,150,2,timeSt);
    }

}

void drawBat()
{
    int power = 0;
    int color = 0;   
            
    if(power=getBat())
    {    
        if(power < 1320)
            color = COLOR_DARK_RED;
        else if(power < 1380)
            color = COLOR_RED;
        else if(power < 1440)
            color = COLOR_ORANGE2;
        else if(power < 1500)
            color = COLOR_LIGHT_YELLOW;
        else
            color = COLOR_GREEN;
        
        fillRect(COLOR_BLACK,289,2,22,11);
        fillRect(COLOR_BLACK,311,4,3,7);
        fillRect(color,290,3,20,9);
    }

}

void drawGui(void)
{
    int w = 0;
    int h = 0;
    
    char myName[]="AvWm xx.xx";

    getStringS("M", &w, &h);

    fillRect(COLOR_WHITE,0 , 0, 320, h+6+MENU_SHADOW);
    fillRect(COLOR_LIGHT_BLUE,0,0,320,h+6);
    fillRect(COLOR_BLACK,0,h+6,320,MENU_SHADOW);
    
    sprintf(myName,"AvWm %d.%d",MAJOR_V,MINOR_V);
    
    putS(COLOR_DARK_GREY,COLOR_LIGHT_BLUE,2,2,myName);

    drawTime();
    drawBat();
}

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