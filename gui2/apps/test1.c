#include <stdlib.h>
#include "graphics.h"
#include "events.h"
#include "colordef.h"
#include "misc.h"

#include "mp3_bg.h"
static BITMAP mp3BMAP = {(unsigned int) mp3_bg, 320, 240, 0, 0};
/* premier programme de test */

needFont(std8x13);

int main(int argc,char ** argv)
{
    int evt;
    char tmp[10];
    extern int stateBAT;
    
    ini_graphics();
    
    setFont(std8x13);  
      
    showPlane(VID1);
    setPlane(VID1);
    clearScreen(COLOR_BLACK);
    
    drawBITMAP (&mp3BMAP, 0, 0);
    
 while(1)
  {         
       /*evt=waitEvent();
       sprintf(tmp,"evt=%04x tick=%d",evt,getTick());
       printf(tmp);*/
       
  }
}
