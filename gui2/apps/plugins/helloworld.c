/*
* helloworld.c
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

/* general stuff */
#include <stdio.h>
#include <stdlib.h>

#include "cops.h" /* for plugin API functions */
#include "avevents.h" /* for events */
#include "events.h" /* for events */
#include "font.h" /* for font definitions/switching */
#include "colordef.h" /* for color definitions (COLOR_BLACK, etc) */

/* Variables can go here */

/* Initialization, REQUIRED */
struct client_operations * cops;

/***************
 * Main function
 **************/
void hello(void)
{
    cops->putS(COLOR_WHITE, COLOR_BLACK, 10, 10, "Hello World!");
}

/***************
 * Handle events
 **************/
int eventHandler(int evt)
{
    switch(evt)
    {
        case BTN_OFF:
        case EVT_QUIT:
            RELEASE(cops); /* we're done */
            break;
    }
}

int main(int argc,char * * argv)
{
    /* Initialization, REQUIRED */
    REGISTER(cops,eventHandler,0);

    cops->hideSBar(); /* shut off the status bar */
    cops->disableMenu(); /* disable the F3=MENU function */
    cops->clearScreen(COLOR_BLACK); /* clear the LCD to black */

    /* These two functions REQUIRED */
    /* Usage: PACK(cops, <main function>)*/
    PACK(cops, hello);
    
    return 1;
}
