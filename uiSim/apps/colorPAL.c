#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"

needFont(std6x9);

int main(int argc,char * * argv)
{
	int i,j,fg;
	char tmp[10];
	ini_graphics();
	
	setFont(std6x9);
			
	
	for(i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
		{
			fillRect(i*16+j, j*20 , i*15, 20, 15);
			if(i*16+j==1)
				fg=0;
			else
				fg=1;
			sprintf(tmp,"%03d",i*16+j);
			putS(i*16+j,fg,j*20+1, i*15+3, tmp);
		}
	}
		
	
	while(1);
	close_graphics();
	return 0;
}