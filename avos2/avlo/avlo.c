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
void printErr(int key);
void waitKeyReleased(void);

int usbstate,usbenable=0,cleanUSBMsg=0;
int chkdefault,cnt=0,cursorPos=0,delayCnt=0x5000;
int errNoDefault=0,cntNoDefault=0,stateNoDefault=0;

int main(int argc,char **argv)
{
	int ret,nbCfg,key,redraw;
	int i;
	
	void (*systemRelocateAdjusted)();

    	systemRelocateAdjusted = systemRelocateMeA - 0x00400000;    
    	systemRelocateAdjusted();
	
	iniGraph();
	
	iniHD();
loop:
	if((ret=file_open("/avlo.cfg"))<0)
		err();

	pal32[0] = 0x6c706c93;
	pal32[1] = 0xffffffff;
	
	graphicsStringA(&screenBitmap2, 0, 0, &sprite8_13, std8x13_, 8, 0,"AVL");
		
	if((nbCfg=do_parse(&cfg,&cfgG))<0)
	{
		debug("Error getting config info\n");
		err();
	}
	
	file_close();	
	
	chkdefault=(!cfgG.defBin[0]==0);
	
	
	graphicsStringA(&screenBitmap2, 0, 0, &sprite8_13, std8x13_, 8, 0,"AVLO");	
	graphicsStringA(&screenBitmap2, 20, 20, &sprite8_13, std8x13_, 8, 0,"Av3xx Loader Version 0.1 by OxyGen");	
		
	
	usbstate=!usbIsConnectedA();
	
	redraw=1;
		
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
		printErr(key);
		
		if(key & BUTTONS_AV300_ANY)
		{
			if ((key & BUTTONS_AV300_DOWN) && !usbenable) {
				if(cursorPos<nbCfg)
				{
					cursorPos++;
					redraw=1;
				}
			}
			
			if ((key & BUTTONS_AV300_UP) && !usbenable) {
				if(cursorPos>0)
				{
					cursorPos--;
					redraw=1;
				}
			}
			
			if ((key & BUTTONS_AV300_ON) && !usbenable) {
				if(loadFile(cfg[cursorPos].image))
				{
					binCaller();
					while(1);
				}
				else
					debug("error loading %s\n",cfg[cursorPos].image);
				redraw=1;
			}
			
			if(key & BUTTONS_AV300_MENU3)
			{
				if(usbenable)
				{
					debug("disable usb\n");
					usbenable=0;
					usbDisableA();
					ataPowerUpHDDA();
					redraw=1;
					cleanUSBMsg=1;
					waitKeyReleased();
					goto loop;
				}
				else
				{
					if(usbstate)
					{
						debug("enable usb\n");
						usbenable=1;
						
						graphicsBoxfA(&screenBitmap2, 60, 100, 200, 40, 0x466c4696);	
						pal32[1]=0xc476c491;
						pal32[0] = 0x466c4696;
			graphicsStringA(&screenBitmap2,   65, 120, &sprite6_9, std6x9_, 6, 0,"USB Enable, PRESS F3 to resume");
						usbEnableA();
					}
				}
			}
			waitKeyReleased();
		}
	}
}

void err()
{
	debug("end let's loop\n");	
	graphicsBoxfA(&screenBitmap2, 60, 100, 200, 40, 0x466c4696);	
						pal32[1]=0xc476c491;
						pal32[0] = 0x466c4696;
	graphicsStringA(&screenBitmap2, 65, 100, &sprite6_9, std6x9_, 6, 0,"Error (check config)");
	graphicsStringA(&screenBitmap2, 65, 120, &sprite6_9, std6x9_, 6, 0,"USB Forced, reboot when done");
	usbEnableA();
	while(1) ;
	
}

void waitKeyReleased(void)
{
	while(buttonsGetStatusA()&BUTTONS_AV300_ANY);
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
	if(chkdefault)
	{
		if(!(key & BUTTONS_AV300_ANY) && cnt < delayCnt)
			graphicsBoxfA(&screenBitmap2, (320*(cnt++))/delayCnt, 210, 1, 5, 0x466c4696);

		if(cnt==delayCnt)
		{	
			pos=0;
			while(pos<nbCfg+1 && strcmp(cfg[pos].label,cfgG.defBin))
				pos++;
			if(pos<nbCfg+1)
			{
				if(loadFile(cfg[pos].image))
				{
					binCaller();
					err();
				}
				else
					debug("error loading %s\n",cfg[cursorPos].image);
			}
			
			errNoDefault=1;
			chkdefault=0;
			cnt=0;
			
		}
		
		if(key & BUTTONS_AV300_ANY)
		{
			graphicsBoxfA(&screenBitmap2, 0, 210, 320, 5, 0x6c706c93);
			chkdefault=0;
			cnt=0;
		}
	}
}

void printErr(int key)
{
	if(errNoDefault)
	{
		if(cntNoDefault<0x2000)
			cntNoDefault++;
		else
		{
			if(stateNoDefault)
			{
				graphicsBoxfA(&screenBitmap2, 0, 210, 320, 10, 0x6c706c93);
			}
			else
			{
				graphicsBoxfA(&screenBitmap2, 0, 210, 320, 10, 0x6c706c93);
				pal32[1]=0xc476c491;
				pal32[0] = 0x6c706c93;//0x466c4696;
				graphicsStringA(&screenBitmap2, 80, 210, &sprite6_9, std6x9_, 6, 0,"default image can't be found");
			}
			stateNoDefault=!stateNoDefault;
			cntNoDefault=0;
		}
		
		if(key & BUTTONS_AV300_ANY)
		{
			graphicsBoxfA(&screenBitmap2, 0, 210, 320, 10, 0x6c706c93);
			errNoDefault=0;
		}
	}
}

void drawMenu(int nbCfg)
{
	int pos;
	if(cleanUSBMsg)
	{
		graphicsBoxfA(&screenBitmap2, 60, 100, 200, 40, 0x6c706c93);
		cleanUSBMsg=0;
	}
	graphicsBoxfA(&screenBitmap2, 110, 52, 100, 100, 0x466c4696);
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
