/*
* matrix.c
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

#include <stdio.h>
#include <stdlib.h>

#include "cops.h"
#include "avevents.h"
#include "font.h"
#include "events.h"
#include "colordef.h"

int j;

/* x/y positions of each letter, there are 17 total */
float x[16];
float y[16];

/*************
 * ARRAYS USED
 ************/
/* starting position for each */
const unsigned char starty[16] = {
10, 182, 108, 84, 191, 50, 218, 143,
102, 48, 168, 35, 124, 29, 156, 79
};

/* speed for each */
float speed[16] = {
.5, .7, .6, .3, .8, .4, .6, .9,
.6, .8, .4, .3, .6, .8, .9, .7
};

/* digit each one shows */
char *digits[16] = {
"6", "4", "2", "4", "3", "1", "4", "7",
"5", "3", "5", "7", "2", "2", "6", "3"
};

/*********
 * BITMAPS
 ********/
const unsigned char one[12][8] = {
{01,01,01,01,01,01,01,01},
{01,01,01,01,01,01,01,11},
{01,01,01,01,01,01,11,11},
{01,01,01,01,01,01,11,11},
{01,01,11,11,11,11,11,11},
{01,11,11,11,11,11,11,11},
{01,11,01,01,01,01,11,11},
{01,01,01,01,01,01,11,11},
{01,01,01,01,01,01,11,11},
{11,11,11,11,11,11,11,11},
{11,11,11,11,11,11,11,01},
{01,01,01,01,01,01,01,01} };
const unsigned char two[12][8] = {
{01,01,01,01,01,01,01,01},
{01,11,11,11,11,11,01,01},
{11,11,11,11,11,11,11,01},
{11,11,01,01,01,11,11,11},
{11,11,01,01,01,11,11,11},
{11,11,11,11,11,11,11,01},
{11,11,11,11,11,11,01,01},
{11,11,01,01,01,01,01,01},
{11,11,11,01,01,01,01,01},
{01,11,11,11,11,11,01,01},
{01,01,01,11,11,11,11,01},
{01,01,01,01,01,01,01,01} };
const unsigned char three[12][8] = {
{01,01,01,01,01,01,01,01},
{11,11,11,11,11,11,11,11},
{01,01,11,11,01,01,01,01},
{01,01,11,11,11,11,11,11},
{01,11,11,11,11,11,11,11},
{01,11,11,01,01,01,11,11},
{01,11,11,01,01,01,11,11},
{01,11,11,01,01,01,11,11},
{11,11,01,01,01,11,11,01},
{11,11,01,01,01,11,11,01},
{11,11,01,01,11,11,01,01},
{01,01,01,01,01,01,01,01} };
const unsigned char four[10][8] = {
{01,01,01,01,01,01,01,01},
{01,01,11,11,11,11,01,01},
{01,11,11,11,11,11,11,01},
{11,11,01,11,11,11,11,11},
{11,11,01,11,01,11,11,11},
{01,11,11,01,01,01,11,11},
{01,01,01,01,01,01,11,11},
{01,01,01,01,01,11,11,01},
{01,01,11,11,11,11,01,01},
{01,01,01,01,01,01,01,01} };
const unsigned char five[12][8] = {
{01,01,01,01,01,01,01,01},
{11,11,01,01,01,01,01,01},
{11,11,01,01,01,01,01,01},
{11,11,11,11,01,01,01,01},
{11,11,11,11,11,11,01,01},
{11,11,01,01,11,11,11,01},
{11,11,01,01,01,01,11,11},
{11,11,01,01,01,01,01,11},
{11,11,01,01,01,01,01,11},
{11,11,01,01,01,01,01,01},
{11,11,01,01,01,01,01,01},
{01,01,01,01,01,01,01,01} };
const unsigned char six[11][8] = {
{01,01,01,01,01,01,01,01},
{11,11,11,11,11,11,11,11},
{11,11,11,11,11,11,11,11},
{11,01,11,01,01,11,01,11},
{11,01,11,01,01,11,01,11},
{11,11,01,01,01,11,11,11},
{11,01,01,01,01,01,01,11},
{11,01,01,01,01,01,01,11},
{11,11,11,11,11,11,11,11},
{11,11,11,11,11,11,11,11},
{01,01,01,01,01,01,01,01}, };
const unsigned char seven[12][8] = {
{01,01,01,01,01,01,01,01},
{11,11,11,11,11,11,11,11},
{11,11,11,11,11,11,11,11},
{01,01,11,11,01,01,01,01},
{01,01,11,11,01,01,01,01},
{01,01,11,11,01,01,01,01},
{01,11,11,11,11,11,01,01},
{01,11,11,11,11,11,11,01},
{01,11,11,01,01,11,11,11},
{11,11,11,01,01,01,11,11},
{11,11,01,01,01,01,11,11},
{01,01,01,01,01,01,01,01} };

BITMAP Bitmap1 = {(unsigned int) one,   8, 12, 0, 0};
BITMAP Bitmap2 = {(unsigned int) two,   8, 12, 0, 0};
BITMAP Bitmap3 = {(unsigned int) three, 8, 12, 0, 0};
BITMAP Bitmap4 = {(unsigned int) four,  8, 10, 0, 0};
BITMAP Bitmap5 = {(unsigned int) five,  8, 12, 0, 0};
BITMAP Bitmap6 = {(unsigned int) six,   8, 11, 0, 0};
BITMAP Bitmap7 = {(unsigned int) seven, 8, 12, 0, 0};

/* initialization */
struct client_operations * cops;

/***************
 * Main function
 **************/
int matrix(void)
{
    for(j=0; j<16; j++)
    {
        if(y[j] > 240)
            y[j] = -25;
        else
            y[j]+=speed[j];

        if(digits[j] == "1")
            cops->drawBITMAP(&Bitmap1,x[j],y[j]);
        else if(digits[j] == "2")
            cops->drawBITMAP(&Bitmap2,x[j],y[j]);
        else if(digits[j] == "3")
            cops->drawBITMAP(&Bitmap3,x[j],y[j]);
        else if(digits[j] == "4")
            cops->drawBITMAP(&Bitmap4,x[j],y[j]);
        else if(digits[j] == "5")
            cops->drawBITMAP(&Bitmap5,x[j],y[j]);
        else if(digits[j] == "6")
            cops->drawBITMAP(&Bitmap6,x[j],y[j]);
        else if(digits[j] == "7")
            cops->drawBITMAP(&Bitmap7,x[j],y[j]);
    }
}

/***************
 * Event handler
 **************/
int eventHandler(int evt)
{
    switch(evt)
    {
        case BTN_OFF:
        case EVT_QUIT:
            RELEASE(cops);
            break;
    }
}

int main(int argc,char * * argv)
{
    /*****************************
     * Initialization
     ****************************/
    REGISTER(cops,eventHandler,0);
    cops->hideSBar(); /* shut off that status bar */
    cops->clearScreen(COLOR_BLACK); /* clear the lcd */
    cops->disableMenu(); /* disable F3=menu function */
    cops->setFont(STD8X13); /* set the font to a larger size */

    /* initialize locations */
    for(j=0; j<16; j++)
    {
        x[j] = j*20;
        y[j] = starty[j];
    }

    /* .. and loop. */
    PACK(cops,matrix);
    STOPME(cops);

    return 0;
}