#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"
#include "events.h"

needFont(std6x9);

int main(int argc,char * * argv)
{
	ini_graphics();
	
	setFont(std6x9);
		
	drawImage("/mnt/img.jpg");
	
	waitEvent();	
	
	close_graphics();
	return 0;
}
