#include "string.h"

#include <ata.h>
#include <fat.h>
#include <file.h>
#include <system.h>
#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <fonts.h>
#include <debug.h>
#include <usb.h>

#include "parse_cfg.h"

/**************************************************************
***************************************************************
I should remove return -1 with clean halt
***************************************************************
**************************************************************/

static struct graphicsBuffer screenBitmap2;
static unsigned int * screenDirect = (unsigned int*) 0x03a00000;
static int pal32[2] = {0x6c706c93, 0xffffffff};
static int pal16[2] = {0x0000, 0xffff};
static struct graphicsBuffer sprite6_9 = {0, 1, 6, 9, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
static struct graphicsBuffer sprite8_13 = {0, 1, 8, 13, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
void (*binCaller)()=(void (*)())0x03000000;

struct config_image cfg[MAX_CFG];
struct config_gene cfgG;

void iniHD();
void err();
void iniGraph();
void affUSB();
void drawMenu();
void processDefault(int key,int nbCfg);

int usbstate;
int keyPressed,oldpos=-1,cnt=0,cursorPos=0,delayCnt=0x30000;

int main(int argc,char **argv)
{
	int ret,nbCfg,key,redraw;
	
	void (*systemRelocateAdjusted)();

    	systemRelocateAdjusted = systemRelocateMeA - 0x00400000;    
    	systemRelocateAdjusted();
	
	iniGraph();
	
	iniHD();
	
	if((ret=file_open("/avlo.cfg"))<0)
		err();

	graphicsStringA(&screenBitmap2, 0, 0, &sprite8_13, std8x13_, 8, 0,"AVL");
		
	if((nbCfg=do_parse(&cfg,&cfgG))<0)
	{
		debug("Error getting config info\n");
		err();
	}
	
	file_close();
	
	if(cfgG.defBin[0]==0)
	{
		keyPressed=1;
	}
	else
	{
		keyPressed=0;
	}
	
	graphicsStringA(&screenBitmap2, 0, 0, &sprite8_13, std8x13_, 8, 0,"AVLO");	
	graphicsStringA(&screenBitmap2, 20, 20, &sprite8_13, std8x13_, 8, 0,"Av3xx Loader Version 0.1 by OxyGen");	
	graphicsBoxfA(&screenBitmap2, 110, 52, 100, 100, 0x466c4696);	
	
	usbstate=!usbIsConnectedA();
	
	redraw=1;
	oldpos=-1;
	cnt=0;
		
	while(1)
	{		
		if(redraw)
		{
			drawMenu(nbCfg);
			redraw=0;
		}
		
		affUSB();
		
		key=buttonsGetStatusA();
		
		processDefault(key,nbCfg);
		
		if (key & BUTTONS_AV300_DOWN) {
			if(cursorPos<nbCfg)
			{
				cursorPos++;
				redraw=1;
			}
		}
		
		if (key & BUTTONS_AV300_UP) {
			if(cursorPos>0)
			{
				cursorPos--;
				redraw=1;
			}
		}
		
		if (key & BUTTONS_AV300_ON) {
			if(loadFile(cfg[cursorPos].image))
			{
				binCaller();
				err();
			}
			else
				debug("error loading %s\n",cfg[cursorPos].image);
			redraw=1;
		}
	}
}

void err()
{
	debug("end let's loop\n");
	while(1);
}

void iniGraph()
{
	int i,j;
	
	osdInitA();
	
	osdSetComponentConfigA(OSD_VIDEO1, 0);
    	osdSetComponentConfigA(OSD_VIDEO2, 0);
    	osdSetComponentConfigA(OSD_BITMAP1, 0);
    	osdSetComponentConfigA(OSD_BITMAP2, 0);
    	osdSetComponentConfigA(OSD_CURSOR1, 0);
    	osdSetComponentConfigA(OSD_CURSOR2, 0);
	
	screenBitmap2.offset = 0x03a00000;
    	screenBitmap2.bytesPerLine = 320*4;
    	screenBitmap2.width = 320;
    	screenBitmap2.height =240;
    	screenBitmap2.bitsPerPixelShift = 5;
    	screenBitmap2.bitsPerPixel = 32;
	
	 for (j=0;j<240;j++)
        	for (i=0;i<320;i++)            
            		screenDirect[j*320 + i] = 0x6c706c93;
			
	osdSetComponentSizeA(OSD_VIDEO1, 320*2, 240);
	osdSetComponentPositionA(OSD_VIDEO1, 0x14, 0x12);
	osdSetComponentOffsetA(OSD_VIDEO1, 0x03a00000);
	osdSetComponentSourceWidthA(OSD_VIDEO1, 0x28);
	osdSetComponentConfigA(OSD_VIDEO1, OSD_COMPONENT_ENABLE);
}

void iniHD()
{
	int fatHD;
	
	graphicsStringA(&screenBitmap2, 0, 0, &sprite8_13, std8x13_, 8, 0,"A");
    
	inifile();
	inidir();
	inifatinfo();
	
	ataSelectHDDA();
	ataPowerUpHDDA();
	ataReadMBR();
	
	if((fatHD = fatInit(getPartition(0)))<0)
		err();
	
	graphicsStringA(&screenBitmap2, 0, 0, &sprite8_13, std8x13_, 8, 0,"AV");
}

void affUSB()
{
	if(usbstate != usbIsConnectedA())
	{
		usbstate=usbIsConnectedA();
		if(usbstate)
		{
			graphicsStringA(&screenBitmap2, 290, 231, &sprite6_9, std6x9_, 6, 0,"USB");
		}
		else
		{
			graphicsBoxfA(&screenBitmap2, 290, 230, 20, 10, 0x466c4696);
		}
	}
}

void processDefault(int key,int nbCfg)
{
	int pos;
	if(cfgG.defBin[0]!=0)
	{
		if(!(key & BUTTONS_AV300_ANY) && cnt < delayCnt && !keyPressed)
		{
			if(oldpos!=(320*cnt)/delayCnt)
			{
				graphicsBoxfA(&screenBitmap2, (320*cnt)/delayCnt, 210, 1, 5, 0x466c4696);
				oldpos=(320*cnt)/delayCnt;
			}
			cnt++;
		}
		
		if(!(key & BUTTONS_AV300_ANY) && cnt==delayCnt)
		{	
			pos=0;
			while(pos<nbCfg+1 && strcmp(cfg[pos].label,cfgG.defBin))
				pos++;
			if(pos<nbCfg+1)
			{
				if(loadFile(cfg[cursorPos].image))
				{
					binCaller();
					err();
				}
				else
					debug("error loading %s\n",cfg[cursorPos].image);
			}
			graphicsBoxfA(&screenBitmap2, 0, 210, 320, 5, 0x6c706c93);
			keyPressed=1;
			cnt=0;
			
		}
		
		if(key & BUTTONS_AV300_ANY)
		{
			if(!keyPressed)
			{
				graphicsBoxfA(&screenBitmap2, 0, 210, 320, 5, 0x6c706c93);
				keyPressed=1;
				cnt=0;
			}
		}
	}
	else
		debug("no default in config\n");
}

void drawMenu(int nbCfg)
{
	int pos;
	for(pos=0;pos<nbCfg+1;pos++)
	{
		pal32[1] = 0xff80ff80;
		if (pos==cursorPos)
		{
			pal32[0] = 0x00800080;
		}
		else
		{
			pal32[0] = 0x466c4696;
		}
			
		graphicsStringA(&screenBitmap2, 110, 52 + pos*9, &sprite6_9, std6x9_, 6, 0,cfg[pos].label);
		
		if(pos==cursorPos)
		{
			pal32[1]=0xc476c491;
			pal32[0] = 0x466c4696;
			graphicsBoxfA(&screenBitmap2, 10, 230, 280, 10, 0x466c4696);
			graphicsStringA(&screenBitmap2, 10, 230, &sprite6_9, std6x9_, 6, 0,cfg[pos].image);
		}
	}
}
