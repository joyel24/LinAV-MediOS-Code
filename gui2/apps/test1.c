#include <stdlib.h>
#include "graphics.h"
#include "events.h"
#include "colordef.h"

/* premier programme de test */

needFont(std8x13);

int main(int argc,char ** argv)
{
    int evt;
    char tmp[10];
    
    ini_graphics();
    
    setFont(std8x13);
    
    
    clearScreen(COLOR_BLACK);
    
 while(1)
  {  
       evt=waitEvent();
       sprintf(tmp,"evt=%04x\n",evt);
       printf(tmp);
       drawRect(COLOR_BLACK,10,50,310,10);
       putS(COLOR_WHITE,COLOR_BLACK,10,50,tmp);
  }
}
