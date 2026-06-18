/*
* settings_menu.c
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Zakk Roberts
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/
#include "cops.h"
#include "colordef.h"
#include "global.h"

int settings_cursorpos = 1;

/* Initialization, REQUIRED */
struct client_operations * cops;

void analog_settings(void)
{
    if(settings.analog_time == 1)
        cops->putS((settings_cursorpos==1)?(COLOR_RED):(COLOR_WHITE), COLOR_BLACK, 10, 10, "Time Display: 24-hour mode");
    else if(settings.analog_time == 2)
        cops->putS((settings_cursorpos==1)?(COLOR_RED):(COLOR_WHITE), COLOR_BLACK, 10, 10, "Time Display: 12-hour mode");
    else
        cops->putS((settings_cursorpos==1)?(COLOR_RED):(COLOR_WHITE), COLOR_BLACK, 10, 10, "Time Display: OFF");
}