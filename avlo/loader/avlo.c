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

#define MAX_OFF_PRESS    500
#define MAX_REPEAT       1000
#define MAX_DELAY        25000

#define __clf()                            \
    ({                            \
        unsigned long temp;                \
    __asm__ __volatile__(                    \
    "mrs    %0, cpsr        @ clf\n"        \
"    orr    %0, %0, #64\n"                    \
"    msr    cpsr_c, %0"                    \
    : "=r" (temp)                        \
    :                            \
    : "memory");                        \
    })

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

char * errorMsg[]={
"Error initializing HD",                  /*err(0)*/
"Error: should not come back from OS",    /*err(1)*/
"Error opening file avlo.cfg",            /*err(2)*/
"Bad config file (avlo.cfg)",             /*err(3)*/
};

int  iniHD();
void moveCursor(int direction);
void err(int i);
void iniGraph();
void affUSB();
void drawMenu();
int  processDefault(int key,int nbCfg);
void printErr(int key);
void waitKeyReleased(void);
void chkOFF(int key);
void drawProgress(int offset,int length,int mode);

extern int loadCJBM(char * filename,char * key);

int usbstate,usbenable=0,cleanUSBMsg=0;
int chkdefault,cnt=0,cursorPos=0,delayCnt;
int errNoDefault=0,cntNoDefault=0,stateNoDefault=0;
int nbOff=0;
int * wdt = (int*)0x30a1a;
int maxRepeat;


int main(int argc,char **argv)
{
    int ret,nbCfg,key,redraw;
    int i;
    
    void (*systemRelocateAdjusted)();

        systemRelocateAdjusted = systemRelocateMeA - 0x00400000;    
        systemRelocateAdjusted();
loopErr:    

usbenable=0;cleanUSBMsg=0;cnt=0;cursorPos=0;errNoDefault=0;cntNoDefault=0;stateNoDefault=0;nbOff=0;
    iniGraph();
   
    if(iniHD()<0)
        goto loopErr;
        
loop:
    if((ret=file_open("/avlo.cfg"))<0)
    {
        err(2);
        goto loopErr;
    }

    pal32[0] = 0x6c706c93;
    pal32[1] = 0xffffffff;
        
    graphicsStringA(&screenBitmap2, 0, 0, &sprite8_13, std8x13_, 8, 0,"AVL");
        
    if((nbCfg=do_parse(&cfg,&cfgG))<0)
    {
        debug("Error getting config info\n");
        err(3);
        goto loopErr;
    }
    
    file_close();    
    
    chkdefault=(!cfgG.defBin[0]==0);
    
    if(cfgG.repeat==0)
    	maxRepeat=MAX_REPEAT;
    else
    	maxRepeat=cfgG.repeat;
        
    if(cfgG.timeOut==0)
    	delayCnt=MAX_DELAY;
    else
    	delayCnt=cfgG.timeOut;
       
    debug("Gal opt:\n -default=%s,\n-key=%s,\n-repeat=%d,\n-time out=%d\n",cfgG.defBin,cfgG.key,maxRepeat,delayCnt);
        
    graphicsStringA(&screenBitmap2, 0, 0, &sprite8_13, std8x13_, 8, 0,"AVLO");
        
    graphicsStringA(&screenBitmap2, 20, 20, &sprite8_13, std8x13_, 8, 0,"Av3xx Loader Version 1.2 by OxyGen");    
        
    
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
        
        if(processDefault(key,nbCfg)<0)
            goto loopErr;
            
        printErr(key);
        
        if(key & BUTTONS_AV300_ANY)
        {
            if ((key & BUTTONS_AV300_DOWN) && !usbenable) {
                if(cursorPos<nbCfg)
                {
                    moveCursor(+1);                                       
                }
            }
            
            if ((key & BUTTONS_AV300_UP) && !usbenable) {
                if(cursorPos>0)
                {
                    moveCursor(-1);
                }
            }
            
            if (((key & BUTTONS_AV300_ON)||(key & BUTTONS_AV300_RIGHT)) && !usbenable) {
            	// let's find out the file extension
                char * ext=strrchr(cfg[cursorPos].image,'.')+1;
                int launch=0;
                
                if(ext[0]=='a' && ext[1]=='j' && ext[2]=='z' && ext[3]=='\0' 
                		&& cfgG.key[0]!=0 && loadCJBM(cfg[cursorPos].image,cfgG.key))
                	launch=1;
                else if (loadFile(cfg[cursorPos].image))
                	launch=1;
                
                if(launch)
                {
                    debug("File loaded, now launching\n");
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
                    for(i=0;i<0x10000;i++); /* Nothing */
                    ataPowerUpHDDA();
                    for(i=0;i<0x10000;i++); /* Nothing */
                    ataSelectHDDA();
                    for(i=0;i<0x10000;i++); /* Nothing */
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
                        for(i=0;i<0x10000;i++); /* Nothing */
                        ataPowerUpHDDA();
                        for(i=0;i<0x10000;i++); /* Nothing */
                        ataSelectHDDA();
                        for(i=0;i<0x10000;i++); /* Nothing */
                    }
                }
            }
            waitKeyReleased();
        }
    }
}

void moveCursor(int direction)
{
        // unhighlight current
        pal32[1] = 0xff80ff80;
        pal32[0] = 0x466c4696;
        graphicsStringA(&screenBitmap2, 110, 52 + cursorPos*9, &sprite6_9, std6x9_, 6, 0,cfg[cursorPos].label);
        // move to nxt
        cursorPos+=direction;
        // highlight nxt
        pal32[1] = 0xff80ff80;
        pal32[0] = 0x00800080;
        graphicsStringA(&screenBitmap2, 110, 52 + cursorPos*9, &sprite6_9, std6x9_, 6, 0,cfg[cursorPos].label);
        // change bottom status
        pal32[1]=0xc476c491;
        pal32[0] = 0x466c4696;
        graphicsBoxfA(&screenBitmap2, 10, 230, 280, 10, 0x466c4696);
        graphicsStringA(&screenBitmap2, 10, 230, &sprite6_9, std6x9_, 6, 0,cfg[cursorPos].image);
}

void err(int i)
{
    int key;
    debug("error, let's loop\n");    
    graphicsBoxfA(&screenBitmap2, 60, 100, 230, 40, 0x466c4696);    
                        pal32[1]=0xc476c491;
                        pal32[0] = 0x466c4696;
    graphicsStringA(&screenBitmap2, 65, 105, &sprite6_9, std6x9_, 6, 0,errorMsg[i]);
    graphicsStringA(&screenBitmap2, 65, 115, &sprite6_9, std6x9_, 6, 0,"USB is enable, you can access the HD");
    graphicsStringA(&screenBitmap2, 65, 125, &sprite6_9, std6x9_, 6, 0,"Press a key to retry");
    usbEnableA();
    usbenable=1;
    while(!((key=buttonsGetStatusA()) & BUTTONS_AV300_ANY))
    {
        chkOFF(key);
    }   
}

void chkOFF(int key)
{
    int i;
    if (key & BUTTONS_AV300_OFF)
    {
        nbOff++;
        if(nbOff>MAX_OFF_PRESS)
        {
            graphicsBoxfA(&screenBitmap2, 60, 100, 200, 40, 0x466c4696);    
            pal32[1]=0xc476c491;
            pal32[0] = 0x466c4696;
            graphicsStringA(&screenBitmap2,   65, 120, &sprite6_9, std6x9_, 6, 0,"Shutting down NOW !!");    
            if(usbenable)
            {
                usbDisableA();
            	for(i=0;i<0x14000;i++); /* Nothing */
            }
            ataSleepCmdA();
            for(i=0;i<0x14000;i++); /* Nothing */
            ataPowerDownHDDA();
            //for(i=0;i<1000;i++); /* Nothing */
            __clf();
            while(1) *wdt=0;
        }
    }
    else
        nbOff=0;
}

void waitKeyReleased(void)
{
    int key;
    int nbPressed=0;;
    key=buttonsGetStatusA();
    while(key&BUTTONS_AV300_ANY && nbPressed < maxRepeat)
    {
        chkOFF(key);
        if(key==BUTTONS_AV300_UP || key==BUTTONS_AV300_DOWN)
        	nbPressed++;
        key=buttonsGetStatusA();
    }
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

int iniHD(void)
{
    int fatHD,c;
    
    graphicsStringA(&screenBitmap2, 0, 0, &sprite8_13, std8x13_, 8, 0,"A");
    
    inifile();
    inidir();
    inifatinfo();
    
    usbDisableA();
    for (c=0;c<0x1000;c++) {}
    ataSelectHDDA();
    for (c=0;c<0x1000;c++) {}
    usbEnableA();
    for (c=0;c<0x1000;c++) {}
    ataPowerUpHDDA();
    for (c=0;c<0x1000;c++) {}
    usbDisableA();
    for (c=0;c<0x14000;c++) {}
    ataReadMBR();
    
    
    if((fatHD = fatInit(getPartition(0)))<0)
    {
        err(0);
        return -1;
    }
    
    graphicsStringA(&screenBitmap2, 0, 0, &sprite8_13, std8x13_, 8, 0,"AV");
    return 0;
}

void affUSB(void)
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

int processDefault(int key,int nbCfg)
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
                    err(1);
                    return -1;
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
    return 0;
}

void drawProgress(int offset,int length,int mode)
{
	if(mode)
		graphicsBoxfA(&screenBitmap2, 10+(300*offset)/length, 210, 1 , 5, 0xffffffff);
        else
        {
        	graphicsBoxfA(&screenBitmap2, 10, 210, (300*offset)/length+1 , 5, 0xffffffff);
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
