#include <stdio.h>
#include <stdlib.h>

#include "alias.h"
#include "graphics.h"
#include "events.h"

needFont(std6x9);


int main(int argc,char * * argv)
{

   int evt;                       // button reading
   char *filename;  // current file location

	ini_graphics();

	setFont(std6x9);

   if(argc<1)
      return 0; // Quit
   else
   {
       filename=argv[1];
   }

	drawImage(filename);

	for(;;)
	{
		while((evt=nxtEvent())>0)
		{
			switch(evt)
			{
				case BUTTON_OFF: // quit
						goto end;
						break;
			}
		}
   }

	end:

	close_graphics();
	return 0;
}
