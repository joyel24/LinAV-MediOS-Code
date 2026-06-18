/*
* font_demo.c
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

/* Initialization, REQUIRED */
struct client_operations * cops;

/***************
 * Main function
 **************/
void font(void)
{
    cops->setFont(STD4X6);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 0, "STD4X6");
    cops->setFont(STD4X8);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 8, "STD4X8");
    cops->setFont(STD5X7);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 18, "STD5X7");
    cops->setFont(STD5X8);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 27, "STD5X8");
    cops->setFont(STD6X10);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 37, "STD6X10");
    cops->setFont(STD6X12);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 49, "STD6X12");
    cops->setFont(STD6X13);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 63, "STD6X13");
    cops->setFont(STD6X9);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 78, "STD6X9");
    cops->setFont(STD7X13);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 89, "STD7X13");
    cops->setFont(STD7X14);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 104, "STD7X14");
    cops->setFont(STD8X13);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 120, "STD8X13");
    cops->setFont(CURSIVE);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 135, "CURSIVE [9x15]");
    cops->setFont(DAGGER);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 152, "DAGGER [9x14]");
    cops->setFont(INKBLOT);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 168, "INKBLOT [9x13]");
    cops->setFont(RADON);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 183, "RADON [8x12]");
    cops->setFont(RADONWIDE);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 197, "RADONWIDE [9x12]");
    cops->setFont(SHADOW);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 211, "SHADOW [12x18]");
    cops->setFont(SHADOWBOLD);
    cops->putS(COLOR_WHITE, COLOR_BLACK, 0, 231, "SHADOWBOLD [12x18]");
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
    return 1;
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
    PACK(cops, font);

    return 1;
}
