#include "graphics.h"
#include "events.h"
#include "cops.h"
#include "avevents.h"

#define BLACK 1
#define WHITE 16

#define true 1
#define false 0

struct client_operations * cops;

int stop = 0;

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
            stop=1;
            break;
    }
}

/* called function from outside */
int main(int argc,char * * argv)
{
    int w, h, i;

    REGISTER(cops,eventHandler,0);
    cops->hideSBar();
    
    PACK(cops);

    cops->clearScreen(WHITE);

    for(w = 0; w < SCREEN_WIDTH; w+=7)
        for(h = 0; h < SCREEN_HEIGHT; h+=7)
            cops->drawBITMAP (&testB, w, h);

    while (!stop) /* NOTHING */;

    return 1;
}

