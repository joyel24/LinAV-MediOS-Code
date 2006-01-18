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

#include <i2c.h>
#include <tsc2003.h>

#include <avlo.h>
#include <avlo_cfg.h>

#define MAX_OFF_PRESS    500
#define MAX_REPEAT       1000
#define MAX_DELAY        25000

#define CUSTOM_COLOR_START  230

#define COLOR_TSP      COLOR_BLACK
#define COLOR_TXT      ptr_cfg->color_txt_index
#define COLOR_BOX      ptr_cfg->color_box_index
#define COLOR_SEL      ptr_cfg->color_sel_index
#define COLOR_LOAD     ptr_cfg->color_load_index
#define COLOR_WAIT     ptr_cfg->color_wait_index
#define COLOR_MSG_BOX_0 ptr_cfg->color_msg_box_0_index
#define COLOR_MSG_BOX_1 ptr_cfg->color_msg_box_1_index
#define COLOR_PWR_CHARGE  ptr_cfg->color_pwr_charge_index
#define COLOR_PWR_L0      ptr_cfg->color_pwr_l0_index
#define COLOR_PWR_L1      ptr_cfg->color_pwr_l1_index
#define COLOR_PWR_L2      ptr_cfg->color_pwr_l2_index
#define COLOR_PWR_L3      ptr_cfg->color_pwr_l3_index
#define COLOR_PWR_FRAME_DC_ON   ptr_cfg->color_pwr_frame_dc_on_index
#define COLOR_PWR_FRAME_DC_OFF  ptr_cfg->color_pwr_frame_dc_off_index

// #define ENTRY_X      117
// #define ENTRY_Y      54
// #define STATUS_X     2
// #define STATUS_Y     228
// #define BAR_X        10
// #define BAR_Y        200
// #define BAR_W        100
// #define BAR_H        5
// #define USB_X        290
// #define USB_Y        228
// #define BAT_X        293
// #define BAT_Y        2

#define ENTRY_X      ptr_cfg->menu_x
#define ENTRY_Y      ptr_cfg->menu_y
#define STATUS_X     ptr_cfg->status_x
#define STATUS_Y     ptr_cfg->status_y
#define BAR_X        ptr_cfg->bar_x
#define BAR_Y        ptr_cfg->bar_y
#define BAR_W        ptr_cfg->bar_w
#define BAR_H        ptr_cfg->bar_h
#define USB_X        ptr_cfg->usb_x
#define USB_Y        ptr_cfg->usb_y
#define BAT_X        ptr_cfg->bat_x
#define BAT_Y        ptr_cfg->bat_y

#define BTM_TXT      "V4.1 | image: "

#define NO_TIME_OUT    0
#define WITH_TIME_OUT  1

#define BAT_LOOP_SIZE 10000

#define GET_BAT_LEVEL (tsc2003getVal(CMD_BAT0|INTERNAL_ON))

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
extern struct graphicsBuffer * buffers;

struct avlo_cfg * ptr_cfg;



void start_avlo(void)
{
    int ret,nbCfg,key,redraw;
    int i;
    int x,y,h,w;
    int bat_loop=0;
    int pass_key_release=0;
           
    printf("In main AVLO\n");
    init_cpld();
    
#ifdef HAVE_IMG
    ini_graphics((unsigned int)bg_img);
#else
    ini_graphics(0);
#endif
    ini_font();
    setPlane(BMAP1);
    setState(BMAP1,OSD_BITMAP_RAMCLUT | OSD_BITMAP_ZX1 |OSD_BITMAP_8BIT|OSD_BITMAP_0TRANS);

#ifdef HAVE_IMG   
    showPlane(VID1);
#endif
    showPlane(BMAP1);

loopErr:
    ptr_cfg = &default_cfg;
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
    ptr_cfg = &default_cfg;
    getStringS("Reading config",&w,&h);                        
    drawBox(w,2*h,&x,&y);
    putS(COLOR_TXT,COLOR_BOX,x,y+h/2,"Reading config");
    
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

    printf("Gal opt:\n-default=%s (=>%s),\n-repeat=%d,\n-time out=%d\n-bg img=%s\n",cfgG.defBin,
        chkdefault?"has default":"no default",maxRepeat,delayCnt,cfgG.bg_img[0]!=0?cfgG.bg_img:"NO IMG");
#ifndef HAVE_IMG
    if(cfgG.bg_img[0] != 0)
    {
        int fd_img = fopen(cfgG.bg_img,O_RDONLY);
        int size = 320*240*4+sizeof(struct avlo_cfg);
        if(fd_img<0)
            printf("[WARNING] File not found: %s\n",cfgG.bg_img);
        else
        {
            int nb_read = fread(fd_img,(void*)buffers[VID1].offset,size);
            if(nb_read==size)
            {
                showPlane(VID1);
                /* changing cfg ptr */
                ptr_cfg = (struct avlo_cfg*)(buffers[VID1].offset+320*240*4);
                init_colors();
            }
            else
                printf("[WARNING] only read %d/%d from %s\n",nb_read,size,cfgG.bg_img);
            fclose(fd_img);
        }
    }
    else
    {
        printf("[WARNING] No img defined\n");
    }
#endif
    printf("AVLO\n");
    setFont(STD8X13);
    putS(COLOR_TXT,COLOR_TSP,0,0,"AVLO");

    usbstate=-1;
    
    redraw=1;

    bat_loop=BAT_LOOP_SIZE ;
    
    while(1)
    {
        if(redraw)
        {
            drawMenu(nbCfg);
            redraw=0;
        }

        affUSB();

        /* read bat */
        bat_loop++;
        if(bat_loop>BAT_LOOP_SIZE)
        {
            drawBat();
            bat_loop = 0;
        }
        
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
                if(toLower(ext[0])=='a' && toLower(ext[1])=='j' && toLower(ext[2])=='z' && ext[3]=='\0'
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
                        pass_key_release = 1;                    
                    }
                }
            }
            if(!pass_key_release)
                waitKeyReleased(WITH_TIME_OUT);
            else
                pass_key_release=0;
        }
    }
}



int chargeProgress = 0;

void drawBat(void)
{
    int power=0;
    int color=0;
    int level=0;
    int frame_color = 0;
    
    if(!POWER_CONNECTED)
    {
        power = GET_BAT_LEVEL;
        if(power < 1320)
            color = COLOR_PWR_L0;
        else if(power < 1400)
            color = COLOR_PWR_L1;
        else if(power < 1480)
            color = COLOR_PWR_L2;
        else
            color = COLOR_PWR_L3;

        if(power > 1200)
            level = (int)(power - 1300) / 15;
            
        if(level > 20)
            level = 20;
        frame_color = COLOR_PWR_FRAME_DC_OFF;
    }
    else
    {
        if(chargeProgress == 0)
            level = 0;
        else if(chargeProgress == 1)
            level = 7;
        else if(chargeProgress == 2)
            level = 14;
        else
            level = 20;

        if(chargeProgress < 3 && chargeProgress >=0)
            chargeProgress++;
        else
            chargeProgress = 0;

        color = COLOR_PWR_CHARGE;
        frame_color = COLOR_PWR_FRAME_DC_ON;
    }

    drawRect(frame_color,BAT_X,BAT_Y,22,10);
    fillRect(frame_color,BAT_X+22,BAT_Y+2,3,6);
    fillRect(COLOR_TSP,BAT_X+1,BAT_Y+1,20,8);
    fillRect(color,BAT_X+1,BAT_Y+1,level,8);
    
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
    {
        return 0;
    }
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

void waitKey(void)
{
    int key;
    int pressed=0;
    while(1)
    {
        if(!pressed)
        {
            key=read_btn();
            if(key&BTMASK_ANY)
                pressed = 1;
        }
        else
        {
            if(key != read_btn())
                return;
        }
    }
}

void waitKeyReleased(int has_time_out)
{
    int key;
    int nbPressed=0;
    key=read_btn();
    //printf("W:%x\n",key);
    while((key&BTMASK_ANY) && nbPressed < maxRepeat)
    {
        chkOFF(key);
        key=read_btn();
        if(has_time_out)
            nbPressed++;
        //printf("W:%x\n",key);
    }
    for(key=0;key<100;key++) /* nothing */;
}

void affUSB()
{
    if(usbstate != usbIsConnected())
    {
        usbstate=usbIsConnected();
        if(usbstate)
        {
            setFont(STD6X9);
            putS(COLOR_TXT,0x0,USB_X,USB_Y,"USB");
        }
        else
        {
            fillRect(COLOR_TSP,USB_X,USB_Y, 20, 10);
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

#define TEST_COLOR(VAR,NAME,NAME2,INDEX) {  \
    if(VAR->NAME.r!=-1)               \
    {                                 \
        setPaletteRGB(VAR->NAME.r,VAR->NAME.g,VAR->NAME.b,INDEX); \
        VAR->NAME2=INDEX;             \
        printf("new color (%d,%d,%d) at %d\n",VAR->NAME.r,VAR->NAME.g,VAR->NAME.b,INDEX); \
    }                                 \
}

void init_colors(void)
{
    TEST_COLOR(ptr_cfg,color_txt,color_txt_index,CUSTOM_COLOR_START)
    TEST_COLOR(ptr_cfg,color_box,color_box_index,CUSTOM_COLOR_START+1)
    TEST_COLOR(ptr_cfg,color_sel,color_sel_index,CUSTOM_COLOR_START+2)
    TEST_COLOR(ptr_cfg,color_load,color_load_index,CUSTOM_COLOR_START+3)
    TEST_COLOR(ptr_cfg,color_wait,color_wait_index,CUSTOM_COLOR_START+4)
    TEST_COLOR(ptr_cfg,color_msg_box_0,color_msg_box_0_index,CUSTOM_COLOR_START+5)
    TEST_COLOR(ptr_cfg,color_msg_box_1,color_msg_box_1_index,CUSTOM_COLOR_START+6)
    TEST_COLOR(ptr_cfg,color_pwr_charge,color_pwr_charge_index,CUSTOM_COLOR_START+7)
    TEST_COLOR(ptr_cfg,color_pwr_l0,color_pwr_l0_index,CUSTOM_COLOR_START+8)
    TEST_COLOR(ptr_cfg,color_pwr_l1,color_pwr_l1_index,CUSTOM_COLOR_START+9)
    TEST_COLOR(ptr_cfg,color_pwr_l2,color_pwr_l2_index,CUSTOM_COLOR_START+10)
    TEST_COLOR(ptr_cfg,color_pwr_l3,color_pwr_l3_index,CUSTOM_COLOR_START+11)
    TEST_COLOR(ptr_cfg,color_pwr_frame_dc_on,color_pwr_frame_dc_on_index,CUSTOM_COLOR_START+12)
    TEST_COLOR(ptr_cfg,color_pwr_frame_dc_off,color_pwr_frame_dc_off_index,CUSTOM_COLOR_START+13)
}
