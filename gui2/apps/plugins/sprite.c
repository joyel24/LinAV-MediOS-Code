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

// Matrix entries are the indexes from the palette entries

static unsigned char test[7][7] =
    { {2, 2, 2, 2, 2, 2, 2},
      {2, 3, 3, 3, 3, 3, 2},
      {2, 3, 2, 3, 2, 3, 2},
      {2, 3, 3, 2, 3, 3, 2},
      {2, 3, 2, 3, 2, 3, 2},
      {2, 3, 3, 3, 3, 3, 2},
      {2, 2, 2, 2, 2, 2, 2} };

/*
static unsigned char test[7][2] =
{
   {2,8},
   {2,8},
   {2,8},
   {2,8},
   {2,8},
   {2,8},
   {2,8}
};
*/
static SPRITE testS = {(unsigned int) test, 7, 7, 1, 2};

//                    Index    0,    1,    2,    3    for the test matrix
static unsigned int pal[4] = {0x00, 0x01, 0xB9, 0xFF};


int eventHandler(int evt)
{
    int i = 0;

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

    cops->drawSprite (&pal, &testS, 10, 10);
/*
	 for(w = 0; w < SCREEN_WIDTH; w+=7)
   	 for(h = 0; h < SCREEN_HEIGHT; h+=7)
         cops->drawSprite (&testS, &pal, w, h);
*/
    while (!stop)
 	 {
	 }

    return ;
}

