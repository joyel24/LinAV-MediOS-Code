/*
* osc_peak_meter.c
*
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
#include "mp3_player.h"
#include "cops.h"
#include "colordef.h"


/* client graphical ops */
extern struct client_operations * cops;

/*extern variables */
extern const char colortable[];
extern int window;
extern int pause;
/*******************/

/*******************
 * Ints for GRAPHICS
 ******************/
int peakmeters = 1;  /* "1" for peak meters, "0" for oscillograph */
int scroll_osci = 1; /* "1" if scrolling instead of looping */
int osci_x = 0;      /* used for looping mode */
int lpos, rpos;      /* used to smoothen peak meters */
int peak_decay = 3;  /* number of pixels to decrease for peak meter */

/*****************
 * Ints for COLORS
 ****************/
int peak_levelcolor = 3; /* color of peak meter bars 0-9 */
int peak_bgcolor = 4;    /* bg color of peak meters  0-9 */
int osci_levelcolor = 3; /* color of peaks for oscillograph 1-10 */
int osci_bgcolor = 4;    /* color of bg for oscillograph    1-10 */

struct av_peak av_p;  /* left/right levels */

void drawPeak(void)
{
    if(window == MAIN_WIN)
    {
        /* read peaks */
        cops->readPeak(&av_p);            

        /* get peak values */
        av_p.left=(av_p.left*200)/0x7FFF;
        av_p.right=(av_p.right*200)/0x7FFF;

        /* smoothen out if desired */
        if(peak_decay > 0)
        {
            if(av_p.left < lpos)
                lpos -= peak_decay;
            else
                lpos = av_p.left;

            if(av_p.right < rpos)
                rpos -= peak_decay;
            else
                rpos = av_p.right;
        }
        else
        {
            lpos = av_p.left;
            rpos = av_p.right;
        }

        /* draw the peak meter, or the oscillograph */
        if(peakmeters)
            peak_meters(lpos, rpos);
        else
        {
            if(!pause)
                oscillograph(av_p.left, av_p.right);
        }
     }
}

/**********************
 * Draw an oscillograph
 *********************/
void oscillograph(int l, int r)
{
    /* lower peak values */
    l = l/12;
    r = r/12;

    /* make sure the values aren't too high */
    if(l > 16)
        l = 16;
    if(r > 16)
        r = 16;

    /* keep to the right if not scrolling */
    if(scroll_osci)
        osci_x = 268;

    if(!scroll_osci)
    {
        /* draw a "cursor */
        cops->drawLine(COLOR_BLACK, osci_x+1, 208, osci_x+1, 240);
        /* clear trails */
        cops->drawLine(colortable[osci_bgcolor], osci_x, 208, osci_x, 240);
    }
    /* left */
    cops->drawLine(colortable[osci_levelcolor], osci_x, 224, osci_x, 224-l);
    /* right */
    cops->drawLine(colortable[osci_levelcolor], osci_x, 224, osci_x, 224+r);

    /* move down a pixel, or back to the start */
    if(!scroll_osci)
    {
        if(osci_x < 270)
            osci_x++;
        else
            osci_x = 0;
    }
    /* stay in one spot, and just scroll */
    else
        cops->scrollWindowHoriz(colortable[osci_bgcolor], 1, 208, 269, 32, 1, 0);
}

/***************************
 * Draw a set of peak meters
 **************************/
void peak_meters(int l, int r)
{
    /* left meter */
    cops->fillRect(colortable[peak_bgcolor], lpos, 208, 270-lpos, 15);
    cops->fillRect(colortable[peak_levelcolor], 0, 208, lpos, 10);

    /* right meter */
    cops->fillRect(colortable[peak_bgcolor], rpos, 224, 270-rpos, 15);
    cops->fillRect(colortable[peak_levelcolor], 0, 224, rpos, 10);
}

void erase_peak(void)
{
    cops->fillRect(COLOR_BLACK, 0, 208, 270, 31);
}
