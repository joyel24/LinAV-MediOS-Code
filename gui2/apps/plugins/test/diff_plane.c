#include <stdlib.h>
#include "graphics.h"
#include "events.h"
#include "cops.h"
#include "avevents.h"
#include "colordef.h"

#include "mp3_bg.h"
static BITMAP mp3BMAP = {(unsigned int) mp3_bg, 320, 240, 0, 0};

struct client_operations * cops;

int eventHandler(int evt)
{
    int i = 0;

        switch (evt) {
            case BTN_OFF:
             case EVT_QUIT:
                /* get out of here */
                   cops->hidePlane(VID1);
                   RELEASE(cops)
                break;
        }
}

int main(int argc,char ** argv)
{
    REGISTER(cops,eventHandler,0);
    cops->hideSBar();  
    cops->disableMenu();
        
    cops->showPlane(VID1);
    cops->setPlane(VID1);
    cops->clearScreen(COLOR_WHITE);
    
    cops->drawBITMAP (&mp3BMAP, 0, 0);
    
    PACK(cops,NULL)
    return 1;
}
