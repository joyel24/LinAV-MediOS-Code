#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"
#include "events.h"

needFont(std6x9);

int main(int argc,char * * argv)
{
	int i,j,fg;
	char tmp[10];
	ini_graphics();
	
	setFont(std6x9);
			
	fillRect(1,0 , 0, 320, 240);
	for(i=0;i<5;i++)
	{
		sprintf(tmp,"Hello %d",i);
		putS(0,77,10,15*i+5, tmp);
	}
	
	while(nxtEvent()==NO_EVENT) /*nothing*/;
	
	for(i=0;i<50;i+=2)
		scrollWindowHoriz(-1, 10, 5, 100, 100, 2, 1);
		
	while(nxtEvent()==NO_EVENT) /*nothing*/;
	
	for(i=0;i<50;i+=2)
		scrollWindowHoriz(-1, 10+50-i, 5, 100, 100, 2, 0);
	
	//for(i=0;i<1000000;i++);
	while(nxtEvent()==NO_EVENT) /*nothing*/;
	close_graphics();
	return 0;
}