/*
* sprite.c
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
#include "colordef.h"

#define true 1
#define false 0

struct client_operations * cops;

// Matrix entries are the indexes from the palette entries

static unsigned char test[7][7] =
    { {2, 2, 2, 2, 2, 2, 2},
      {2, 3, 3, 3, 3, 3, 2},
      {2, 3, 2, 3, 2, 3, 2},
      {2, 3, 3, 2, 3, 3, 2},
      {2, 3, 2, 3, 2, 3, 2},
      {2, 3, 3, 3, 3, 3, 2},
      {2, 2, 2, 2, 2, 2, 2} };

static SPRITE testS = {(unsigned int) test, 7, 7, 1, 2};

//                    Index    0,    1,    2,    3    for the test matrix
PALETTE pal1[4] = {0x00, 0x01, 0xB9, 0xFF};
PALETTE pal2[4] = {0xB9, 0xFF, 0x00, 0x01};


int eventHandler(int evt)
{
    int i = 0;

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
    int w, h, c;

     REGISTER(cops,eventHandler,0);
    cops->hideSBar();

    

     cops->clearScreen(COLOR_WHITE);

     c = 0;

     for(w = 0; w < SCREEN_WIDTH; w+=7)
     {
        if(c == 1) c=0; else c=1;

        for(h = 0; h < SCREEN_HEIGHT; h+=7)
         {
            // Switch see palettes for each line
            if(c==0)
                        cops->drawSprite ((PALETTE*)&pal1, &testS, w, h);
                    else
                        cops->drawSprite ((PALETTE*)&pal2, &testS, w, h);
         }
     }
         
         PACK(cops,NULL)
         
         STOPME(cops)

    return 0;
}

