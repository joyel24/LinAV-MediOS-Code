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
                   cops->hidePlane(BMAP2);
                   RELEASE(cops)
                break;
        }
}

int cnt,x,y;

void mainLoop(void)
{
    if(cnt>1000)
    {
        cnt=0;
        cops->fillRect(COLOR_WHITE,x,y,20,20);
        x+=10;
        if(x>300)
        {
            x=0;y+=10;
            if(y>220)
                y=0;
        }
        cops->putS(COLOR_BLUE,COLOR_WHITE,x,y,"T");
    }
    cnt++;
}


int main(int argc,char ** argv)
{
    REGISTER(cops,eventHandler,0);
    cops->hideSBar();  
    cops->disableMenu();
     
    cops->setPlane(BMAP2);
    cops->clearScreen(COLOR_WHITE);
       
    cops->showPlane(VID1);
    cops->setPlane(VID1);
    //cops->cfgPlane(BMAP2,MERGE_BACK|BMAP_A3);
    cops->showPlane(BMAP2);    
    cops->drawBITMAP (&mp3BMAP, 0, 0);
    cops->setPlane(BMAP2);
    cnt=0;x=0;y=0;
    PACK(cops,mainLoop)
    return 1;
}
