/*
* avstring.c
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

#include "font.h"
#include "graphics.h"
#include "avstring.h"
#include "colordef.h"
#include "avevents.h"
#include "events.h"
#include "osd.h"

extern struct plugin msg_box_plugin;

int res; // updated in msgBoxEvtHandler
// we should add some define in msgBox.h such as RES_OK RES_YES RES_NO RES_CANCEL ...
// maybe a param to msgBox to set the type of box: alert, choice yes/no, input text


int stopBoxLoop=0; /* global variable used to stop the private evt loop*/

/*our private loop*/
void msgEvtLoop(void)
{    
    msg_box_plugin.handle_on=1;
    stopBoxLoop=0;
    while(!stopBoxLoop)
        procNxtEvent(waitEvent());
    msg_box_plugin.handle_on=0;
}

/* msg_box event handler */
void msgBoxEvtHandler(int evt)
{
    switch(evt)
    {
        case BTN_OFF:
            stopBoxLoop=1;
            break;
    }
}

// called in avwm.c main()
void iniMsgBox(void)
{
    doRegisterPlugin(&msg_box_plugin,msgBoxEvtHandler,0);
}

/* draw the msg box */
void drawMsgBox(void)
{
    // exmaple in osdtest
    setSize(BMAP2,160,100,8);
    setPos(BMAP2,0x14,0x13+20);
    showPlane(BMAP2);
    setPlane(BMAP2);
    fillRect(COLOR_RED, 0, 0, 160, 100);
    setPlane(BMAP1);
}

/* restore the previous state */
void eraseMsgBox(void)
{
    hidePlane(BMAP2);
    setPlane(BMAP1);
}

/* main function */
int msgBox(unsigned char* caption, unsigned char* msg, int type, int icon)
{
    drawMsgBox();
    msgEvtLoop();
    eraseMsgBox();
    return res;  
}
