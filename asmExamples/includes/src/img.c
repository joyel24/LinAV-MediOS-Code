#include "img.h"
#include "file.h"
#include "terminal.h"
#include "osd.h"
#include "lcd.h"

int loadRGB(char *fileN)
{
	int curFile;
	if((curFile=open(fileN))>=0)
	{
		terminalPutsCT("Opening image..", 0xfbfb);

		char image[90];
		int offset=0;

		int size=320*240;

		while(read(curFile,image,90)>0 && size>0)
		{
			offset=loadImg(image,offset,30);
			size-=30;
			if(size%5000==0)
				terminalPutsCT(".", 0xfbfb);

		}
		close(curFile);

		osdSetBackConfigT(0x14, 0x03);
        osdSetMainConfigT(0x14, 0xdd);
		return 1;
	}
	else
	{
		terminalPutsCT("Error file\n", 0xfbfb);
		return -1;
	}
}
