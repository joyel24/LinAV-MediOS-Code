#include <stdlib.h>
#include "graphics.h"
#include "events.h"
#include "colordef.h"

/* premier programme de test */

int main(int argc,char ** argv)
{
    ini_graphics();
    
    clearScreen(COLOR_WHITE);
 
 while(1)
  {  
    drawRect(COLOR_BLUE,10,10,100,50); 
    
    nxtEvent();
  }
}
