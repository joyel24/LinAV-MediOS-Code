#include <stdlib.h>
#include "graphics.h"
#include "events.h"
#include "colordef.h"

/* premier programme de test */

int main(int argc,char ** argv)
{

 while(1)
 {
    ini_graphics();
    
    nxtEvent();
    
    clearScreen(COLOR_WHITE);
       
    drawRect(COLOR_BLUE,10,10,100,50);   
  }
}
