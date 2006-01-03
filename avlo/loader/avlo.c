/*
*   loader/avlo.c
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <string.h>
#include <ata.h>
#include <fat.h>
#include <fs_io.h>
#include <disk.h>
#include <irq.h>
#include <kernel.h>

#include <graphics.h>
#include <buttons.h>
#include <font.h>
#include <osd.h>

#include <colordef.h>

#include <parse_cfg.h>

#include <avlo.h>

#define MAX_OFF_PRESS    500
#define MAX_REPEAT       1000
#define MAX_DELAY        25000

#define COLOR_TSP      COLOR_BLACK
#define COLOR_TXT      COLOR_WHITE
#define COLOR_BOX      COLOR_BLUE
#define COLOR_SEL      COLOR_RED
#define COLOR_LOAD     COLOR_RED3
#define COLOR_WAIT     COLOR_ORANGE
#define COLOR_BOX_BAR  COLOR_DARK_GRAY

#define COLOR_MSG_BOX_0 COLOR_DARK_GRAY
#define COLOR_MSG_BOX_1 COLOR_GRAY

#define ENTRY_X      117
#define ENTRY_Y      54
#define STATUS_X     75
#define STATUS_Y     227
#define BAR_X        60
#define BAR_Y        190
#define BAR_W        (320-2*BAR_X)
#define BAR_H        5

#define BTM_TXT      "V3.1 | image: "

#define NO_TIME_OUT    0
#define WITH_TIME_OUT  1

void (*binCaller)(char * param)=(void (*)(char *))0x03000000;

struct config_image cfg[MAX_CFG];
struct config_gene cfgG;

char * errorMsg[]={
"Error initializing HD",                  /*err(0)*/
"Error: should not come back from OS",    /*err(1)*/
"Error opening file avlo.cfg",            /*err(2)*/
"Bad config file (avlo.cfg)",             /*err(3)*/
};

int usbstate,usbenable=0,cleanUSBMsg=0;
int chkdefault,cnt=0,cursorPos=0,delayCnt;
int errNoDefault=0,cntNoDefault=0,stateNoDefault=0;
int nbOff=0;
int * wdt = (int*)0x30a1a;
int maxRepeat;
char tmp_txt[100];

extern char bg_img[320*4*240];

void start_avlo(void)
{
    int ret,nbCfg,key,redraw;
    int i;
    int x,y,h,w;
    printf("In main AVLO\n");
    init_cpld();

    

    ini_graphics((unsigned int)bg_img);
    ini_font();
    setPlane(BMAP1);
    setState(BMAP1,OSD_BITMAP_RAMCLUT | OSD_BITMAP_ZX1 |OSD_BITMAP_8BIT|OSD_BITMAP_0TRANS);
    
    showPlane(VID1);
    showPlane(BMAP1);

loopErr:
    usbenable=0;cleanUSBMsg=0;cnt=0;cursorPos=0;errNoDefault=0;cntNoDefault=0;stateNoDefault=0;nbOff=0;
    clearScreen(COLOR_TSP);
    setFont(STD8X13);
    printf("A\n");
    putS(COLOR_TXT,COLOR_TSP,0,0,"A");
    
    getStringS("HD Init",&w,&h);                        
    drawBox(w,2*h,&x,&y);
    putS(COLOR_TXT,COLOR_BOX,x,y+h/2,"HD Init");
    
    init_disk();

    printf("AV\n");
    setFont(STD8X13);
    putS(COLOR_TXT,COLOR_TSP,0,0,"AV");

loop:
    if((ret=file_open("/avlo.cfg"))<0)
    {
        err(2);
        goto loopErr;        
    }

    printf("AVL\n");
    setFont(STD8X13);
    putS(COLOR_TXT,COLOR_TSP,0,0,"AVL");


    if((nbCfg=do_parse(cfg,&cfgG))<0)
    {
        printf("Error getting config info\n");
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

    printf("Gal opt:\n-default=%s (=>%s),\n-repeat=%d,\n-time out=%d\n",cfgG.defBin,
        chkdefault?"has default":"no default",maxRepeat,delayCnt);

    printf("AVLO\n");
    setFont(STD8X13);
    putS(COLOR_TXT,COLOR_TSP,0,0,"AVLO");

    usbstate=usbIsConnected();

    redraw=1;

    while(1)
    {
        if(redraw)
        {
            drawMenu(nbCfg);
            redraw=0;
        }

        affUSB();

        key=read_btn();

        if(processDefault(key,nbCfg)<0)
            goto loopErr;

        printErr(key);

        if(key&BTMASK_ANY)
        {
            if ((key&BTMASK_DOWN) && !usbenable) {
                if(cursorPos<nbCfg)
                {
                    moveCursor(+1);
                }
            }

            if ((key&BTMASK_UP) && !usbenable) {
                if(cursorPos>0)
                {
                    moveCursor(-1);
                }
            }

            if (((key&BTMASK_ON)||(key&BTMASK_RIGHT)) && !usbenable)
            {
                // let's find out the file extension
                char * ext=strrchr(cfg[cursorPos].image,'.')+1;
                int launch=0;
                printf("loading: %s ext:%s\n",cfg[cursorPos].image,ext);
                if(ext[0]=='a' && ext[1]=='j' && ext[2]=='z' && ext[3]=='\0'
                        //&& cfgG.key[0]!=0
                                //&& loadCJBM(cfg[cursorPos].image,cfgG.key))
                                && fastLoadCJBM(cfg[cursorPos].image))
                    launch=1;
                else if (loadFile(cfg[cursorPos].image,(char*)0x03000000,1))
                    launch=1;

                if(launch)
                {
                    printf("File loaded, now launching\n");
                    printf("append=%s\n",cfg[cursorPos].append);
                    if(cfg[cursorPos].append[0])
                    {
                        snprintf(tmp_txt,100,"AV_Pinit=/bin/init_cust myinit=%s",cfg[cursorPos].append);
                        binCaller(tmp_txt);
                    }
                    else
                        binCaller(NULL);
                    while(1);
                }
                else
                    printf("error loading %s\n",cfg[cursorPos].image);
                redraw=1;
            }

            if(key&BTMASK_MENU3)
            {
                if(usbenable)
                {
                    printf("disable usb\n");
                    usbenable=0;
                    usb_disable();
                    redraw=1;
                    cleanUSBMsg=1;
                    waitKeyReleased(NO_TIME_OUT);
                    for(i=0;i<0x10000;i++); /* Nothing */
                    goto loopErr;
                }
                else
                {
                    if(usbstate)
                    {                        
                        getStringS("USB Enable, PRESS F3 to resume",&w,&h);
                        
                        printf("enable usb\n");
                        usbenable=1;
                        //fillRect(COLOR_BOX,60, 100, 200, 40);  
                        drawBox(w,2*h,&x,&y);
                        putS(COLOR_TXT,COLOR_BOX,x,y+h/2,"USB Enable, PRESS F3 to resume");

                        usb_enable();
                        waitKeyReleased(NO_TIME_OUT);                        
                    }
                }
            }
            waitKeyReleased(WITH_TIME_OUT);
        }
    }
}

void drawBox(int txt_width,int txt_height,int * start_x,int * start_y)
{
    *start_x = (320-(txt_width+8))/2;
    *start_y = (240-(txt_height+8))/2;
    drawRect(COLOR_MSG_BOX_0,*start_x,*start_y,txt_width+8,txt_height+8);
    drawRect(COLOR_MSG_BOX_1,*start_x+1,*start_y+1,txt_width+6,txt_height+6);
    fillRect(COLOR_BOX,*start_x+2,*start_y+2,txt_width+4,txt_height+4);
    *start_x += 4;
    *start_y += 4;
}

void (*decode)(char * src,char * dst)=(void (*)(char * src,char * dst))0x03F00470;

int fastLoadCJBM(char * filename)
{
    unsigned char * cptr;
    int x,y,h,w;
    getStringS("File Loaded, decompressing...",&w,&h);

    if (!loadFile(filename,(char*)0x03800000,1))
        return 0;
    printf("File loaded, now decompressing\n");
    //fillRect(COLOR_BOX,60, 100, 230, 40);
    setFont(STD6X9);
    getStringS("File Loaded, decompressing...",&w,&h);
    drawBox(w,2*h,&x,&y);
    putS(COLOR_TXT,COLOR_BOX,x,y+h/2,"File Loaded, decompressing...");
    cptr=(unsigned char *)0x03F00860+0x10;
    *cptr=0;

    decode((char*)0x03800000,(char*)0x03000000);
    printf("decompress done\n");

    return 1;
}

void moveCursor(int direction)
{
    int h,w;
    // unhighlight current
    setFont(STD6X9);
    putS(COLOR_TXT,COLOR_TSP,ENTRY_X, ENTRY_Y + cursorPos*9,cfg[cursorPos].label);
    getStringS(cfg[cursorPos].label,&w,&h);
    w--;
    h--;
    drawLine(COLOR_TSP,ENTRY_X-1,ENTRY_Y+1+ cursorPos*9,ENTRY_X-1,ENTRY_Y+h-1+ cursorPos*9);
    drawLine(COLOR_TSP,ENTRY_X+w+1,ENTRY_Y+1+ cursorPos*9,ENTRY_X+w+1,ENTRY_Y+h-1+ cursorPos*9);
    drawLine(COLOR_TSP,ENTRY_X-2,ENTRY_Y+2+ cursorPos*9,ENTRY_X-2,ENTRY_Y+h-2+ cursorPos*9);
    drawLine(COLOR_TSP,ENTRY_X+w+2,ENTRY_Y+2+ cursorPos*9,ENTRY_X+w+2,ENTRY_Y+h-2+ cursorPos*9);
    // move to nxt
    cursorPos+=direction;
    // highlight nxt
    putS(COLOR_TXT,COLOR_SEL,ENTRY_X, ENTRY_Y + cursorPos*9,cfg[cursorPos].label);
    getStringS(cfg[cursorPos].label,&w,&h);
    w--;
    h--;
    drawLine(COLOR_SEL,ENTRY_X-1,ENTRY_Y+1+ cursorPos*9,ENTRY_X-1,ENTRY_Y+h-1+ cursorPos*9);
    drawLine(COLOR_SEL,ENTRY_X+w+1,ENTRY_Y+1+ cursorPos*9,ENTRY_X+w+1,ENTRY_Y+h-1+ cursorPos*9);
    drawLine(COLOR_SEL,ENTRY_X-2,ENTRY_Y+2+ cursorPos*9,ENTRY_X-2,ENTRY_Y+h-2+ cursorPos*9);
    drawLine(COLOR_SEL,ENTRY_X+w+2,ENTRY_Y+2+ cursorPos*9,ENTRY_X+w+2,ENTRY_Y+h-2+ cursorPos*9);
    // change bottom status
    fillRect(COLOR_TSP,STATUS_X, STATUS_Y, 200, 10);
    snprintf(tmp_txt,100,"%s%s",BTM_TXT,cfg[cursorPos].image);
    putS(COLOR_TXT,COLOR_TSP,STATUS_X, STATUS_Y,tmp_txt);
}

void err(int i)
{
    int key=0;
    int x,y,h,w;
    
    printf("error, let's loop\n");

    //fillRect(COLOR_BOX,60, 100, 230, 40);
    setFont(STD6X9);    
    getStringS("USB is enable, you can access the HD",&w,&h);
    drawBox(w,3*h,&x,&y);
    putS(COLOR_TXT,COLOR_BOX,x,y,errorMsg[i]);
    putS(COLOR_TXT,COLOR_BOX,x,y+h,"USB is enable, you can access the HD");
    putS(COLOR_TXT,COLOR_BOX,x,y+2*h,"Press a key to retry");
    usb_enable();
    usbenable=1;
    while(1)
    {
        key=read_btn();
        if(key&BTMASK_ANY)
        {
            if(key&BTMASK_OFF)
                chkOFF(key);
            else
                break;
        }
    }
}

void chkOFF(int key)
{
    int i;
    int x,y,h,w;
    if (key&BTMASK_OFF)
    {
        nbOff++;
        if(nbOff>MAX_OFF_PRESS)
        {
            //fillRect(COLOR_BOX,60, 100, 200, 40);
            setFont(STD6X9);    
            getStringS("Shutting down NOW !!",&w,&h);
            drawBox(w,2*h,&x,&y);
            putS(COLOR_TXT,COLOR_BOX,x,y+h/2,"Shutting down NOW !!");
            printf("Shutting down NOW !!\n");
            if(usbenable)
            {
                usb_disable();
                for(i=0;i<0x14000;i++); /* Nothing */
            }
           ata_stop_HD();
            __clf();
            while(1) *wdt=0;
        }
    }
    else
        nbOff=0;
}

void waitKeyReleased(int has_time_out)
{
    int key;
    int nbPressed=0;;
    key=read_btn();
    while((key&BTMASK_ANY) && nbPressed < maxRepeat)
    {
        chkOFF(key);
        key=read_btn();
        if(has_time_out)
            nbPressed++;
    }
    for(key=0;key<100;key++) /* nothing */;
}

void affUSB(void)
{
    if(usbstate != usbIsConnected())
    {
        usbstate=usbIsConnected();
        if(usbstate)
        {
            setFont(STD6X9);
            putS(COLOR_TXT,0x0,290,STATUS_Y,"USB");
        }
        else
        {
            fillRect(COLOR_TSP,290, STATUS_Y, 20, 10);
        }
    }
}

int processDefault(int key,int nbCfg)
{
    int pos;
    if(chkdefault)
    {
        if(!(key&BTMASK_ANY) && cnt < delayCnt)
            fillRect(COLOR_WAIT,BAR_X+(BAR_W*(cnt++))/delayCnt, BAR_Y, 1, BAR_H);
        if(cnt==delayCnt)
        {
            pos=0;
            while(pos<nbCfg+1 && strcmp(cfg[pos].label,cfgG.defBin))
                pos++;
            if(pos<nbCfg+1)
            {            
                char * ext=strrchr(cfg[pos].image,'.')+1;
                int launch=0;
                printf("loading: %s ext:%s\n",cfg[pos].image,ext);
                if(ext[0]=='a' && ext[1]=='j' && ext[2]=='z' && ext[3]=='\0'                        
                                && fastLoadCJBM(cfg[pos].image))
                    launch=1;
                else if (loadFile(cfg[pos].image,(char*)0x03000000,1))
                    launch=1;

                if(launch)
                {
                    printf("File loaded, now launching\n");
                    printf("append=%s\n",cfg[pos].append);
                    if(cfg[pos].append[0])
                    {
                        snprintf(tmp_txt,100,"AV_Pinit=/bin/init_cust myinit=%s",cfg[pos].append);
                        binCaller(tmp_txt);
                    }
                    else
                        binCaller(NULL);
                    while(1);
                }
                else
                    printf("error loading %s\n",cfg[pos].image);
/*            
                if(loadFile(cfg[pos].image,(char*)0x03000000,1))
                {
                    printf("append=%s\n",cfg[pos].append);
                    if(cfg[pos].append[0])
                    {
                        snprintf(tmp_txt,100,"AV_Pinit=/bin/init_cust myinit=%s",cfg[pos].append);
                        binCaller(tmp_txt);
                    }
                    else
                        binCaller(NULL);
                    err(1);
                    return -1;
                }
                else
                    printf("error loading %s\n",cfg[cursorPos].image);
*/
            }

            errNoDefault=1;
            chkdefault=0;
            cnt=0;

        }

        if(key&BTMASK_ANY)
        {
            fillRect(COLOR_TSP,BAR_X, BAR_Y, BAR_W, BAR_H);
            chkdefault=0;
            cnt=0;
        }
    }
    return 0;
}

void drawProgress(int offset,int length,int mode)
{
    if(mode)
        fillRect(COLOR_LOAD,BAR_X+(BAR_W*offset)/length, BAR_Y, 1 , BAR_H);
    else
        fillRect(COLOR_LOAD,BAR_X,BAR_Y, (BAR_W*offset)/length+1 , BAR_H);
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
                fillRect(COLOR_TSP,0, 210, 320, 10);
            }
            else
            {
                fillRect(COLOR_TSP,0, 210, 320, 10);
                putS(COLOR_TXT,COLOR_TSP,80,210,"default image can't be found");
            }
            stateNoDefault=!stateNoDefault;
            cntNoDefault=0;
        }

        if(key & BTMASK_ANY)
        {
            fillRect(COLOR_TSP,0, 210, 320, 10);
            errNoDefault=0;
        }
    }
}

void drawMenu(int nbCfg)
{
    int pos;
    int bg_color;
    int w,h;

    // clean AVLO txt */
    clearScreen(COLOR_TSP);

    if(cleanUSBMsg)
    {
        fillRect(COLOR_TSP,60, 100, 200, 40);
        cleanUSBMsg=0;
    }

    fillRect(COLOR_TSP,ENTRY_X, ENTRY_Y, 100-7, 100-7);

    drawRect(COLOR_MSG_BOX_0,BAR_X-2,BAR_Y-2,BAR_W+4,BAR_H+4);
    drawRect(COLOR_MSG_BOX_1,BAR_X-1,BAR_Y-1,BAR_W+2,BAR_H+2);
    fillRect(COLOR_TSP,BAR_X, BAR_Y, BAR_W, BAR_H);

    for(pos=0;pos<nbCfg+1;pos++)
    {
        if (pos==cursorPos)
        {
            bg_color = COLOR_SEL;            
        }
        else
        {
            bg_color = COLOR_TSP;
        }

        setFont(STD6X9);
        putS(COLOR_TXT,bg_color,ENTRY_X, ENTRY_Y + pos*9,cfg[pos].label);

        if(pos==cursorPos)
        {
            getStringS(cfg[cursorPos].label,&w,&h);
            w--;
            h--;
            drawLine(COLOR_SEL,ENTRY_X-1,ENTRY_Y+1+ cursorPos*9,ENTRY_X-1,ENTRY_Y+h-1+ cursorPos*9);
            drawLine(COLOR_SEL,ENTRY_X+w+1,ENTRY_Y+1+ cursorPos*9,ENTRY_X+w+1,ENTRY_Y+h-1+ cursorPos*9);
            drawLine(COLOR_SEL,ENTRY_X-2,ENTRY_Y+2+ cursorPos*9,ENTRY_X-2,ENTRY_Y+h-2+ cursorPos*9);
            drawLine(COLOR_SEL,ENTRY_X+w+2,ENTRY_Y+2+ cursorPos*9,ENTRY_X+w+2,ENTRY_Y+h-2+ cursorPos*9);
            fillRect(COLOR_TSP,STATUS_X, STATUS_Y, 200, 10);
            snprintf(tmp_txt,100,"%s%s",BTM_TXT,cfg[pos].image);
            putS(COLOR_TXT,COLOR_TSP,STATUS_X, STATUS_Y,tmp_txt);
        }
    }
}

int loadFile(char * fileN,char* buffer,int prog)
{
    int curFile;
    int offset=0;
    int size;
    int clustSize;

    curFile=fopen(fileN,O_RDONLY);
    printf("openFile(): %d\n",curFile);


    if(curFile>=0)
    {
        size=filesize(curFile);
        printf("file size:%d\n",size);
        clustSize=getClusterSize(0); /* volume 0 */

        while((fread(curFile,&buffer[offset],clustSize))>0)
        {
            if(prog)
                drawProgress(offset,size,0);
            offset+=clustSize;
        }
        fclose(curFile);
        return 1;
    }
    else
    {
        printf("Error loading file\n");
        return 0;
    }
}
