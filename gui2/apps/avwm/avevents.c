/*
* avevents.c
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
#include <stdio.h>
#include "events.h"
#include "plugin.h"
#include "avevents.h"

extern struct plugin cur_plugin;
extern struct plugin menu_plugin;
extern struct plugin status_bar_plugin;

extern int stopWM;

int nbOff=MAX_OFF;

struct wkUP_evt nxt_action = {
    app : NO_APP
};

int stopApp=0,inLoop=0;

void pack(void (*loopFct)(void))
{
    cur_plugin.handle_on=1;
    stopApp=0;
    if(loopFct)
    {
        inLoop=1;
        while(!stopApp)
            loopFct();
    }
    else
    {
        inLoop=0;
        pause_app();
    }
}

void myRelease_app(void)
{
    if(inLoop)
        stopApp=1;
    else
        release_app();
}

void eventLoop()
{
    int evt;
    
    while(!stopWM)
    {
        evt=waitEvent();
        
        if(evt==EVT_TIMER)
            printf("tick:%d usb:%d pwr:%d\n",getTick(),getUSB(),getPwr());
        if(evt==EVT_PWR)
            printf("Power changed\n");
        if(evt==EVT_USB)
            printf("USB changed\n");
        
        if(evt==BTN_OFF)
        {
            if(nbOff)
                nbOff--;
            else
            {
                stopWM=1;
                continue;
            }
        }
        else
            nbOff=MAX_OFF;
        
        if(evt==EVT_WKUP)        
        {
            switch(nxt_action.app)
            {
                case NO_APP:
                    break;
                case APP_MP3:
                    loadPlugin("/mnt/avwm/plugins/play",nxt_action.arg);
                    menu_plugin.handle_on=0;
                    break;
            }
            nxt_action.app=0;
            continue;          
        }
           

        if(status_bar_plugin.handle_on)
            sendEvt(&status_bar_plugin,evt);
                
        if(menu_plugin.handle_on)
            sendEvt(&menu_plugin,evt);
        else
        {        
            if(evt==BTN_F3 && menuStatus() )
            {
                clearEventQueue();
                sendEvt(&cur_plugin,EVT_SUSPEND);
                // Show status line, maybe disabled by plugin before
                showSBar();
                menu_plugin.handle_on=1;
                sendEvt(&menu_plugin,EVT_REDRAW);
            }
            else
            {
                sendEvt(&cur_plugin,evt);
            }
        }
    }
}

void sendEvt(struct plugin * plug,int evt)
{
	if(plug->handle_on && plug->handler)
		plug->handler(evt);
}
