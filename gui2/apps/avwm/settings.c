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

extern struct plugin settings_plugin;

needFont(std6x9);
needFont(std7x13);

int stopSettingsLoop = 0; /* global variable used to stop the private evt loop*/

/*our private loop*/
void SettingsEvtLoop(void)
{
    settings_plugin.handle_on=1;
    stopSettingsLoop=0;
    while(!stopSettingsLoop)
        procNxtEvent(waitEvent());
    settings_plugin.handle_on=0;
}

/* events */
int settingsEvtHandler(int evt)
{
    switch (evt) {
        case EVT_REDRAW:
            break;
        case EVT_TIMER:
            break;

        case BTN_OFF:
            stopSettingsLoop=1;
            break;
    }
}

void drawSettings()
{
    setSize(BMAP2,SCREEN_WIDTH,SCREEN_HEIGHT, 8);
    setPos(BMAP2,0x14,0x13);

    // show box
    setPlane(BMAP2);
    setFont(std7x13);

    clearScreen(COLOR_WHITE);

    putS(COLOR_BLACK, COLOR_WHITE, 2, 20, "AVWM Settings");
    putS(COLOR_BLACK, COLOR_WHITE, 2, 100, "There are many things to do :)");
    putS(COLOR_BLACK, COLOR_WHITE, 2, 200, "Press OFF to leave");

    showPlane(BMAP2);
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
