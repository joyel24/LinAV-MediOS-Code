#include <stdlib.h>
#include "graphics.h"
#include "events.h"
#include "colordef.h"

/* premier programme de test */

int main(int argc,char ** argv)
{
    ini_graphics();

    drawRect(1,10,10,100,50);
    
 while(1)
  {  
    lcd_update();
    nxtEvent();
  }
}
