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
#include "avwm.h"

extern struct plugin cur_plugin;
extern struct plugin menu_plugin;
extern struct plugin status_bar_plugin;
extern struct plugin msg_box_plugin;
extern struct plugin edit_box_plugin;
extern struct plugin settings_plugin;
extern struct plugin CF_mnt_plugin;
extern struct plugin FM_plugin;
extern struct plugin helper_menu_plugin;

extern int stopWM;

int nbOff=MAX_OFF;

struct wkUP_evt nxt_action = {
    app : NO_APP
};

int stopApp=0,inLoop=0;

void pack(void (*loopFct)(void))
{    
    int evt;
    cur_plugin.handle_on=1;
    stopApp=0;
    
    while(!stopApp && !stopWM)
    {
        if(loopFct)
            evt=nxtEvent();
        else
            evt=waitEvent();
        procNxtEvent(evt);
        if(loopFct)
            loopFct();
    }
    stop_me();
}

void myRelease_app(void)
{
    stopApp=1;    
}

void procNxtEvent(int evt)
{                          
    if(evt==BTN_OFF)
    {
        if(nbOff)
            nbOff--;
        else
        {
            stopWM=1;
            return;
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
        return;          
    }
    
    
    sendEvt(&FM_plugin,evt);
        
    sendEvt(&CF_mnt_plugin,evt);
    
    if(status_bar_plugin.handle_on)
        sendEvt(&status_bar_plugin,evt);

    if(settings_plugin.handle_on)
    {
        sendEvt(&settings_plugin,evt);
        return;
    }

    if(msg_box_plugin.handle_on)
    {
        sendEvt(&msg_box_plugin,evt);
        return;
    }

    if(edit_box_plugin.handle_on)
    {
        sendEvt(&edit_box_plugin,evt);
        return;
    }

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

void eventLoop()
{
    int evt;
    while(!stopWM)
    {
       evt=waitEvent();
       procNxtEvent(evt); 
    }
}

void sendEvt(struct plugin * plug,int evt)
{
	if(plug->handle_on && plug->handler)
		plug->handler(evt);
}
