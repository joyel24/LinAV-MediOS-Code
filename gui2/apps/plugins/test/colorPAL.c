#include <stdio.h>
#include <stdlib.h>


#include "cops.h" 
#include "avevents.h"
#include "events.h"

struct client_operations * cops;

int eventHandler(int evt)
{
    switch(evt)
    {
        case BTN_OFF:
        case EVT_QUIT:
            RELEASE(cops); /* we're done */
            break;
    }
}

int main(int argc,char * * argv)
{
	int i,j,fg;
	char tmp[10];
        
        REGISTER(cops,eventHandler,0);
	
        cops->hideSBar(); /* shut off the status bar */
        cops->disableMenu(); /* disable the F3=MENU function */
        
	
	for(i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
		{
			cops->fillRect(i*16+j, j*20 , i*15, 20, 15);
			if(i*16+j==1)
				fg=0;
			else
				fg=1;
			sprintf(tmp,"%03d",i*16+j);
			cops->putS(i*16+j,fg,j*20+1, i*15+3, tmp);
		}
	}
		
	PACK(cops, NULL);
        return 0;
}
