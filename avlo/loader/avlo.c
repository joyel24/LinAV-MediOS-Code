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
#include <timers.h>

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

#define __cli()							\
	({							\
		unsigned long temp;				\
	__asm__ __volatile__(					\
	"mrs	%0, cpsr		@ cli\n"		\
"	orr	%0, %0, #128\n"					\
"	msr	cpsr_c, %0"					\
	: "=r" (temp)						\
	:							\
	: "memory");						\
	})   
    

//#define COLOR_TSP    0x0000
int COLOR_TSP=       0x0000;
#define COLOR_TXT    0xffff
#define COLOR_BOX    0xD6D6
#define COLOR_SEL    0x0101
#define COLOR_LOAD   0x2525
#define COLOR_WAIT   0x0909

#define ENTRY_X      117
#define ENTRY_Y      53
#define STATUS_X     75
#define STATUS_Y     227
#define BAR_X        60
#define BAR_Y        190
#define BAR_W        (320-2*BAR_X)
#define BAR_H        5

#define BTM_TXT      "V2.0 | image: "
        
/**************************************************************
***************************************************************
I should remove return -1 with clean halt
***************************************************************
**************************************************************/

static struct graphicsBuffer screenBitmap2;
//static unsigned int * screenDirect = (unsigned int*) 0x03a00000;
static unsigned char * screenDirect = (unsigned char*) 0x03a00000;
static int pal32[2] = {0x6c706c93, 0xffffffff};
static int pal16[2] = {0x0000, COLOR_TXT};
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

//extern int loadCJBM(char * filename,char * key);
int fastLoadCJBM(char * filename);

int usbstate,usbenable=0,cleanUSBMsg=0;
int chkdefault,cnt=0,cursorPos=0,delayCnt;
int errNoDefault=0,cntNoDefault=0,stateNoDefault=0;
int nbOff=0;
int * wdt = (int*)0x30a1a;
int maxRepeat;
char tmp_txt[100];

#ifdef INCLUDE_IMG
extern char bg_img[320*4*240];
#else
char *bg_img=0x03b00000;
#endif

int main(int argc,char **argv)
{
    int ret,nbCfg,key,redraw;
    int i,j,val;
    int * ptrScreen=(int*)screenDirect;
    
    void (*systemRelocateAdjusted)();

        systemRelocateAdjusted = systemRelocateMeA - 0x00400000;    
        systemRelocateAdjusted();
loopErr:  
  
pal16[0] = COLOR_TSP;


usbenable=0;cleanUSBMsg=0;cnt=0;cursorPos=0;errNoDefault=0;cntNoDefault=0;stateNoDefault=0;nbOff=0;
    iniGraph();
   
    if(iniHD()<0)
        goto loopErr;
#ifndef INCLUDE_IMG      
    if (loadFile("/avlo.img",bg_img))
    {
        osdSetComponentConfigA(OSD_VIDEO1, OSD_COMPONENT_ENABLE);
        COLOR_TSP=0x0000;
    }
    else
    {
        COLOR_TSP=0xA6A6;
        pal16[0] = COLOR_TSP;
        val=COLOR_TSP | (COLOR_TSP<<8) | (COLOR_TSP<<16) | (COLOR_TSP<<24);
        j=(240*320*2)/4;
        for(i=0;i<j;i++)                
            ptrScreen[i] = val;
        
        graphicsStringA(&screenBitmap2, 20, 20, &sprite8_13, std8x13_, 8, 0,"Av3xx Loader by OxyGen"); 
    }
#endif  
loop:
    if((ret=file_open("/avlo.cfg"))<0)
    {
        err(2);
        goto loopErr;
    }

    pal32[0] = 0x6c706c93;
    pal32[1] = 0xffffffff;
        
    pal16[0] = COLOR_TSP;
    pal16[1] = COLOR_TXT;
    
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
        
    //graphicsStringA(&screenBitmap2, 20, 20, &sprite8_13, std8x13_, 8, 0,"Av3xx Loader Version 1.2 by OxyGen");    
        
    //graphicsStringA(&screenBitmap2, 70, STATUS_Y, &sprite8_13, std8x13_, 8, 0,"V2.0|image:");
    
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
                debug("loading: %s ext:%s\n",cfg[cursorPos].image,ext);
                if(ext[0]=='a' && ext[1]=='j' && ext[2]=='z' && ext[3]=='\0' 
                		//&& cfgG.key[0]!=0 
                                //&& loadCJBM(cfg[cursorPos].image,cfgG.key))
                                && fastLoadCJBM(cfg[cursorPos].image))
                	launch=1;
                else if (loadFile(cfg[cursorPos].image,(char*)0x03000000))
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
                        
                        //graphicsBoxfA(&screenBitmap2, 60, 100, 200, 40, 0x466c4696);    
                        graphicsBoxfA(&screenBitmap2, 60, 100, 200, 40, COLOR_BOX);    
                        pal32[1]=0xc476c491;
                        pal32[0] = 0x466c4696;
                        pal16[0] = COLOR_BOX;
                        pal16[1] = COLOR_TXT;
                        graphicsStringA(&screenBitmap2,   65, 115, &sprite6_9, std6x9_, 6, 0,"USB Enable, PRESS F3 to resume");
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

int (*decode)(char * src,char * dst)=(void (*)(char * src,char * dst))0x03F00470;

int fastLoadCJBM(char * filename)
{
    unsigned char * cptr;
    
    if (!loadFile(filename,(char*)0x03800000))
        return 0;
    debug("File loaded, now decompressing\n");
    graphicsBoxfA(&screenBitmap2, 60, 100, 230, 40, COLOR_BOX);
    pal16[0] = COLOR_BOX;
    pal16[1] = COLOR_TXT;
    graphicsStringA(&screenBitmap2, 65, 115, &sprite6_9, std6x9_, 6, 0,"File Loaded, decompressing...");
    cptr=(unsigned char *)0x03F00860+0x10;
    *cptr=0; 
    
    decode((char*)0x03800000,(char*)0x03000000);
    debug("decompress done\n");    
    
    return 1;
}

void moveCursor(int direction)
{
    // unhighlight current
    pal32[1] = 0xff80ff80;
    pal32[0] = 0x466c4696;
    pal16[0] = COLOR_TSP;
    pal16[1] = COLOR_TXT;
    graphicsStringA(&screenBitmap2, ENTRY_X, ENTRY_Y + cursorPos*9, &sprite6_9, std6x9_, 6, 0,cfg[cursorPos].label);
    // move to nxt
    cursorPos+=direction;
    // highlight nxt
    pal16[0] = COLOR_SEL;
    pal16[1] = COLOR_TXT;
    pal32[1] = 0xff80ff80;
    pal32[0] = 0x00800080;
    graphicsStringA(&screenBitmap2, ENTRY_X, ENTRY_Y + cursorPos*9, &sprite6_9, std6x9_, 6, 0,cfg[cursorPos].label);
    // change bottom status
    pal32[1]=0xc476c491;
    pal32[0] = 0x466c4696;
    pal16[0] = COLOR_TSP;
    pal16[1] = COLOR_TXT;
    //graphicsBoxfA(&screenBitmap2, 10, 230, 280, 10, 0x466c4696);
    graphicsBoxfA(&screenBitmap2, STATUS_X, STATUS_Y, 200, 10, COLOR_TSP);
    snprintf(tmp_txt,100,"%s%s",BTM_TXT,cfg[cursorPos].image);
    graphicsStringA(&screenBitmap2, STATUS_X, STATUS_Y, &sprite6_9, std6x9_, 6, 0,tmp_txt);
}

void err(int i)
{
    int key,stop=0;
    debug("error, let's loop\n");    
    //graphicsBoxfA(&screenBitmap2, 60, 100, 230, 40, 0x466c4696); 
    graphicsBoxfA(&screenBitmap2, 60, 100, 230, 40, COLOR_BOX); 
    pal32[1]=0xc476c491;
    pal32[0] = 0x466c4696;
    pal16[0] = COLOR_BOX;
    pal16[1] = COLOR_TXT;
    graphicsStringA(&screenBitmap2, 65, 105, &sprite6_9, std6x9_, 6, 0,errorMsg[i]);
    graphicsStringA(&screenBitmap2, 65, 115, &sprite6_9, std6x9_, 6, 0,"USB is enable, you can access the HD");
    graphicsStringA(&screenBitmap2, 65, 125, &sprite6_9, std6x9_, 6, 0,"Press a key to retry");
    usbEnableA();
    usbenable=1;
    while(1)
    {
        key=buttonsGetStatusA();
        if(key & BUTTONS_AV300_ANY)
        {
            if(key & BUTTONS_AV300_OFF)
                chkOFF(key);
            else
                break;
        }
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
            //graphicsBoxfA(&screenBitmap2, 60, 100, 200, 40, 0x466c4696);    
            graphicsBoxfA(&screenBitmap2, 60, 100, 200, 40, COLOR_BOX);    
            pal32[1]=0xc476c491;
            pal32[0] = 0x466c4696;
            pal16[0] = COLOR_BOX;
            pal16[1] = COLOR_TXT;
            graphicsStringA(&screenBitmap2,   65, 115, &sprite6_9, std6x9_, 6, 0,"Shutting down NOW !!");    
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
    int i,j,val;
    int * ptr=(int*)screenDirect;
    
    osdInitA();
    
    osdSetComponentConfigA(OSD_VIDEO1, 0);
    osdSetComponentConfigA(OSD_VIDEO2, 0);
    osdSetComponentConfigA(OSD_BITMAP1, 0);
    osdSetComponentConfigA(OSD_BITMAP2, 0);
    osdSetComponentConfigA(OSD_CURSOR1, 0);
    osdSetComponentConfigA(OSD_CURSOR2, 0);
    
    /*screenBitmap2.offset = 0x03a00000;
        screenBitmap2.bytesPerLine = 320*4;
        screenBitmap2.width = 320;
        screenBitmap2.height =240;
        screenBitmap2.bitsPerPixelShift = 5;
        screenBitmap2.bitsPerPixel = 32;*/
        
    screenBitmap2.offset = 0x03a00000;
    screenBitmap2.bytesPerLine = 320*2;
    screenBitmap2.width = 320;
    screenBitmap2.height = 240;
    screenBitmap2.bitsPerPixelShift = 4;
    screenBitmap2.bitsPerPixel = 16;
    
    val=COLOR_TSP | (COLOR_TSP<<8) | (COLOR_TSP<<16) | (COLOR_TSP<<24);
    j=(240*320*2)/4;
    for(i=0;i<j;i++)                
        ptr[i] = val;//0x6c706c93;
            
    osdSetComponentSizeA(OSD_VIDEO1, 320*2, 240);
    osdSetComponentPositionA(OSD_VIDEO1, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_VIDEO1, (int)bg_img);
    osdSetComponentSourceWidthA(OSD_VIDEO1, 0x28);
#ifdef INCLUDE_IMG    
    osdSetComponentConfigA(OSD_VIDEO1, OSD_COMPONENT_ENABLE);
#endif
    osdSetComponentSizeA(OSD_BITMAP1, 320*2, 240);
    osdSetComponentPositionA(OSD_BITMAP1, 0x14, 0x13);
    osdSetComponentOffsetA(OSD_BITMAP1, 0x03a00000);
    osdSetComponentSourceWidthA(OSD_BITMAP1, 0x14);
    osdSetComponentConfigA(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT
                                     | OSD_BITMAP_0TRANS
                                     );
    
    /*osdSetComponentSizeA(OSD_VIDEO2, 320*2, 240);
    osdSetComponentPositionA(OSD_VIDEO2, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_VIDEO2, 0x03a00000);
    osdSetComponentSourceWidthA(OSD_VIDEO2, 0x28);
    osdSetComponentConfigA(OSD_VIDEO2, OSD_COMPONENT_ENABLE);*/
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
            graphicsStringA(&screenBitmap2, 290, STATUS_Y, &sprite6_9, std6x9_, 6, 0,"USB");
        }
        else
        {
            //graphicsBoxfA(&screenBitmap2, 290, 230, 20, 10, 0x466c4696);
            graphicsBoxfA(&screenBitmap2, 290, STATUS_Y, 20, 10, COLOR_TSP);
        }
    }
}

int processDefault(int key,int nbCfg)
{
    int pos;
    if(chkdefault)
    {
        if(!(key & BUTTONS_AV300_ANY) && cnt < delayCnt)
            //graphicsBoxfA(&screenBitmap2, (320*(cnt++))/delayCnt, 210, 1, 5, 0x466c4696);
            graphicsBoxfA(&screenBitmap2, BAR_X+(BAR_W*(cnt++))/delayCnt, BAR_Y, 1, BAR_H, COLOR_WAIT);

        if(cnt==delayCnt)
        {    
            pos=0;
            while(pos<nbCfg+1 && strcmp(cfg[pos].label,cfgG.defBin))
                pos++;
            if(pos<nbCfg+1)
            {
                if(loadFile(cfg[pos].image,(char*)0x03000000))
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
            //graphicsBoxfA(&screenBitmap2, 0, 210, 320, 5, 0x6c706c93);
            graphicsBoxfA(&screenBitmap2, BAR_X, BAR_Y, BAR_W, BAR_H, COLOR_TSP);
            chkdefault=0;
            cnt=0;
        }
    }
    return 0;
}

void drawProgress(int offset,int length,int mode)
{
	if(mode)
            //graphicsBoxfA(&screenBitmap2, 10+(300*offset)/length, 210, 1 , 5, 0xffffffff);
            graphicsBoxfA(&screenBitmap2, BAR_X+(BAR_W*offset)/length, BAR_Y, 1 , BAR_H, COLOR_LOAD);
        else
            //graphicsBoxfA(&screenBitmap2, 10, 210, (300*offset)/length+1 , 5, 0xffffffff);
            graphicsBoxfA(&screenBitmap2, BAR_X, BAR_Y, (BAR_W*offset)/length+1 , BAR_H, COLOR_LOAD);
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
                //graphicsBoxfA(&screenBitmap2, 0, 210, 320, 10, 0x6c706c93);
                graphicsBoxfA(&screenBitmap2, 0, 210, 320, 10, COLOR_TSP);
            }
            else
            {
                //graphicsBoxfA(&screenBitmap2, 0, 210, 320, 10, 0x6c706c93);
                graphicsBoxfA(&screenBitmap2, 0, 210, 320, 10, COLOR_TSP);
                pal32[1]=0xc476c491;
                pal32[0] = 0x6c706c93;//0x466c4696;
                pal16[0] = COLOR_TSP;
                pal16[1] = COLOR_TXT;
                graphicsStringA(&screenBitmap2, 80, 210, &sprite6_9, std6x9_, 6, 0,"default image can't be found");
            }
            stateNoDefault=!stateNoDefault;
            cntNoDefault=0;
        }
        
        if(key & BUTTONS_AV300_ANY)
        {
            //graphicsBoxfA(&screenBitmap2, 0, 210, 320, 10, 0x6c706c93);
            graphicsBoxfA(&screenBitmap2, 0, 210, 320, 10, COLOR_TSP);
            errNoDefault=0;
        }
    }
}

void drawMenu(int nbCfg)
{
    int pos;
    
    // clean AVLO txt */
    graphicsBoxfA(&screenBitmap2, 0, 0, 40, 20, COLOR_TSP);
    
    if(cleanUSBMsg)
    {
        //graphicsBoxfA(&screenBitmap2, 60, 100, 200, 40, 0x6c706c93);
        graphicsBoxfA(&screenBitmap2, 60, 100, 200, 40, COLOR_TSP);
        cleanUSBMsg=0;
    }
    //graphicsBoxfA(&screenBitmap2, 110, 52, 100, 100, 0x466c4696);
    graphicsBoxfA(&screenBitmap2, ENTRY_X, ENTRY_Y, 100-7, 100-7, COLOR_TSP);
    for(pos=0;pos<nbCfg+1;pos++)
    {
        pal32[1] = 0xff80ff80;
        pal16[1] = COLOR_TXT;
        if (pos==cursorPos)
        {
            pal32[0] = 0x00800080;
            pal16[0] = COLOR_SEL;
        }
        else
        {
            pal32[0] = 0x466c4696;
            pal16[0] = COLOR_TSP;
        }
            
        graphicsStringA(&screenBitmap2, ENTRY_X, ENTRY_Y + pos*9, &sprite6_9, std6x9_, 6, 0,cfg[pos].label);
        
        if(pos==cursorPos)
        {
            pal32[1]=0xc476c491;
            pal32[0] = 0x466c4696;
            pal16[0] = COLOR_TSP;
            pal16[1] = COLOR_TXT;
            //graphicsBoxfA(&screenBitmap2, 10, 230, 280, 10, 0x466c4696);
            graphicsBoxfA(&screenBitmap2, STATUS_X, STATUS_Y, 200, 10, COLOR_TSP);
            snprintf(tmp_txt,100,"%s%s",BTM_TXT,cfg[pos].image);
            graphicsStringA(&screenBitmap2, STATUS_X, STATUS_Y, &sprite6_9, std6x9_, 6, 0,tmp_txt);
        }
    }
}
