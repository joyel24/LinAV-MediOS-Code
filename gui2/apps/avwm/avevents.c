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

void pack(void)
{
    cur_plugin.handle_on=1;
}

void eventLoop()
{
    int evt;
    
    while(!stopWM)
    {
        evt=waitEvent();       

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
