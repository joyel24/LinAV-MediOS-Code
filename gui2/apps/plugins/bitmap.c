/*
* bitmap.c
* by Schoki
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

#include "stdlib.h"

#include "graphics.h"
#include "events.h"
#include "cops.h"
#include "avevents.h"

#define BLACK 1
#define WHITE 16

#define true 1
#define false 0

struct client_operations * cops;

static unsigned char test[7][7] =
    { {0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9},
      {0xB9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xB9},
      {0xB9, 0xff, 0xB9, 0xff, 0xB9, 0xff, 0xB9},
      {0xB9, 0xff, 0xff, 0xB9, 0xff, 0xff, 0xB9},
      {0xB9, 0xff, 0xB9, 0xff, 0xB9, 0xff, 0xB9},
      {0xB9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xB9},
      {0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9, 0xB9} };


static BITMAP testB = {(unsigned int) test, 7, 7, 0, 0};


int eventHandler(int evt)
{
    switch (evt) {
        case BTN_OFF:
        case EVT_QUIT:
            /* get out of here */
            RELEASE(cops)
            break;
    }
}

/* called function from outside */
int main(int argc,char * * argv)
{
    int w, h, i;

    REGISTER(cops,eventHandler,0);
    cops->hideSBar();   

    cops->clearScreen(WHITE);

    for(w = 0; w < SCREEN_WIDTH; w+=7)
        for(h = 0; h < SCREEN_HEIGHT; h+=7)
            cops->drawBITMAP (&testB, w, h);

    PACK(cops,NULL)
   
    return 0;
}

