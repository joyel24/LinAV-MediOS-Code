#include <stdio.h>
#include <stdlib.h>

#include "graphics.h"

needFont(std6x9);

int main(int argc,char * * argv)
{
	GC_ID gc;
	int i,j;
	char tmp[10];
	ini_graphics();
	gc=createGC(BMAP1);
	gc->fg=0;
	gc->bg=0;
	gc->font=std6x9;
	
		
	//fillColorRect(1, 14 , 0, 10, 10, gc);
	
	for(i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
		{
			fillRect(i*16+j, j*20 , i*15, 20, 15, gc);
			gc->bg=i*16+j;
			if(i*16+j==1)
				gc->fg=0;
			else
				gc->fg=1;
			sprintf(tmp,"%03d",i*16+j);
			putS(USE_GC,USE_GC,j*20+1, i*15+3, tmp, gc);
		}
	}
		
	
	//for(i=0;i<1000000;i++);
	while(1);
	close_graphics();
	return 0;
}