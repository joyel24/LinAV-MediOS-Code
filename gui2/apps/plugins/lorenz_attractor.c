/*
* lorenz_attractor.c
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
#include <math.h>

#include "cops.h" /* for plugin API functions */
#include "avevents.h" /* for events */
#include "events.h" /* for events */
#include "font.h" /* for font definitions/switching */
#include "colordef.h" /* for color definitions (COLOR_BLACK, etc) */

/* Variables can go here */
#define N 10000

int i=0;
double x_0,y_0,z_0,x_1,y_1,z_1;
double h = 0.01; // 0.01
double a = 10.0; // 1.0
double b = 28.0; // 28.0
double c = 8.0 / 3.0; // 8.0/3.0
double last_x0, last_y0;
int delta_a=0, delta_b=0, delta_c=0, delta_h=0;
int delta_x=7, delta_y=4;

int wait = 0;
int mode = 1;

/* Initialization, REQUIRED */
struct client_operations * cops;

/***************
 * Main function
 **************/
void lorenz(void)
{
    if(i<N)
    {
        x_1 = x_0 + h * a * (y_0 - x_0);
        y_1 = y_0 + h * (x_0 * (b - z_0) - y_0);
        z_1 = z_0 + h * (x_0 * y_0 - c * z_0);
        x_0 = x_1;
        y_0 = y_1;
        z_0 = z_1;

        if (i > 100)
        {
            cops->drawPixel(COLOR_WHITE, (x_0*delta_x)+LCD_WIDTH/2, (y_0*delta_y)+LCD_HEIGHT/2);
            cops->drawLine(COLOR_WHITE, (last_x0*delta_x)+LCD_WIDTH/2,
                           (last_y0*delta_y)+LCD_HEIGHT/2, (x_0*delta_x)+LCD_WIDTH/2, (y_0*delta_y)+LCD_HEIGHT/2);
        }

        last_x0 = x_0;
        last_y0 = y_0;

        i++;

        for(wait=0;wait<1000;wait++)
            wait=wait;
    }
}

/*****************
 * Start up screen
 ****************/
void startup(void)
{
    int w,h;
    int i;

    cops->setFont(STD8X13);
    cops->getStringS("Lorenz Attractor", &w, &h);
    cops->putS(COLOR_WHITE, COLOR_BLACK, LCD_WIDTH/2-w/2, 10, "Lorenz Attractor");
    cops->getStringS("by midk", &w, &h);
    cops->putS(COLOR_WHITE, COLOR_BLACK, LCD_WIDTH/2-w/2, 25, "by midk");
    cops->getStringS("Press [ON] to start", &w, &h);
    cops->putS(COLOR_WHITE, COLOR_BLACK, LCD_WIDTH/2-w/2, 220, "Press [ON] to start");
}

/***********
 * Main loop
 **********/
void mainloop(void)
{
    if(mode == 1)
        startup();
    else
        lorenz();
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

        case BTN_ON:
            if(mode == 1)
            {
                cops->clearScreen(COLOR_BLACK);
                mode = 2;
            }
            else if(mode == 2)
            {
                cops->clearScreen(COLOR_BLACK);
                h = 0.01 + delta_h;
                a = 10.0 + delta_a;
                b = 28.0 + delta_b;
                c = (8.0 / 3.0) + delta_c;
                last_x0 = x_0 = 0.1;
                last_y0 = y_0 = 0;
                z_0 = 0;
                lorenz();
            }
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

    x_0 = 0.1;
    y_0 = 0;
    z_0 = 0;

    /* These two functions REQUIRED */
    /* Usage: PACK(cops, <main function>)*/
    PACK(cops, mainloop);

    return 1;
}
