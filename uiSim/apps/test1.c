#include <stdlib.h>

#include "graphics.h"
#include "colordef.h"


int main(int argc,char ** argv)
{
    ini_graphics();
    
    clearScreen(COLOR_WHITE);
    
    drawRect(COLOR_BLUE,10,10,100,50);
    
    while(1) /*nothing*/;
}
