#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "graphics.h"
#include "events.h"
#include "avwm.h"
#include "colordef.h"
#include "parse_cfg.h"
#include "menu.h"

#define DO_DEBUG 1;
#define NB_ITEM 10;

struct client_operations cops={
    drawPixel          : drawPixel,
    readPixel          : readPixel,
    drawRect           : drawRect,
    fillRect           : fillRect,
    drawLine           : drawLine,
    putS               : wmPutS,
    putC               : wmPutC,
    getStringS         : wmgetStringS,
    drawSprite         : drawSprite,
    drawBITMAP         : drawBITMAP,
    scrollWindowVert   : scrollWindowVert,
    scrollWindowHoriz  : scrollWindowHoriz,
    clearEventQueue    : clearEventQueue,
    nxtEvent           : nxtEvent,
    processEvent       : wmNxtEvent,
    addEventHandler    : addEventHandler,
    pack               : pack,
    drawImage          : drawImage,
    openScreen         : ini_graphics,
    closeScreen        : close_graphics,
    setFont            : wmSetFont,
    stopEvt            : stopEvt,
    startEvt           : startEvt,
    stopTimout         : stopTimer,
    startTimout        : startTimer,
    showTime           : showTime,
    hideTime           : hideTime,
    showBat            : showBat,
    hideBat            : hideBat
};

void (*currentHandler)(int evt)=NULL;
void (*tmpHandler)(int evt)=NULL;

#include "font.hx"

int showTimeS=1,showBatS=1;

int set_mouseParam(int freq, int repeat)
{
   int fd = 0;
    struct mouseParam param;
    param.freq = freq;
    param.repeated_press = repeat;

   fd=open("/dev/mouse",O_RDONLY | O_NONBLOCK);
   if (fd < 0)
    {
      printf("Can't open /dev/mouse\n");
        return fd;
   }

    if(ioctl(fd,AV_SET_MOUSE_PARAM,&param)<0)
    {
      printf("Error setting mouse params\n");
        return fd;
   }
   close(fd);

    return fd;
}

void draw_batt_status()
{
   int fd;
   int power = 0;
   int color = 0;

   if(showBatS)
   {
        fd=open("/dev/avtsc",O_RDONLY | O_NONBLOCK);
        if (fd < 0)
            {
            printf("Can't open /dev/avtsc\n");
        }
        
            if(ioctl(fd,AV_LEVEL_BAT0_IOC,&power)<0)
            {
            printf("Error getting power value\n");
        }
        close(fd);
    
        if(power < 1320)
        color = COLOR_DARK_RED;
        else if(power < 1380)
        color = COLOR_RED;
        else if(power < 1440)
        color = COLOR_ORANGE2;
        else if(power < 1500)
        color = COLOR_LIGHT_YELLOW;
    	else
        color = COLOR_GREEN;
    
        fillRect(COLOR_BLACK,289,2,22,11);
        fillRect(COLOR_BLACK,311,4,3,7);
        fillRect(color,290,3,20,9);
    }
}


int plugin_pid=-1;

int loadPlugin(char * path, char * param)
{
    int status;
    fprintf(stderr,"load plug: %s\n",path);
    if(currentHandler)
    {
        currentHandler(EVT_RESUME);
        currentHandler(EVT_QUIT);
        currentHandler=NULL;
        waitpid(plugin_pid, &status, 0);
        plugin_pid=-1;
    }
    drawGui();
    return launchPlugin(path,param);
}

int launchPlugin(char * path,char * param)
{
    int pid,err;
    char cops_a[15];
    
    sprintf(cops_a, "%d",(int)&cops);
    pid = vfork();
    if (pid == 0)
    { // child
        if(param!=NULL)
            err = execl(path, path,param,cops_a,(char *)0);
        else
            err = execl(path, path,cops_a,(char *)0);
        fprintf(stderr, "exec failed!%s %d %ld\n", path, err, errno);
        _exit(1);
    }
    else {
        if (pid < 0) {
            fprintf(stderr, "vfork failed %d\n", pid);
            return -1;
        }
        plugin_pid=pid;
    }
    return 0;

}

int timerOn=0;
int evtOn=0;
int stopWM;
int batteryRefresh = 0;

char * path;
char menuFile[]="menu.cfg";

int main(int argc,char * * argv)
{
    int i;
    char * tmpC;
    int pos,size;
    
    fprintf(stderr,"Starting AvWm\n");
    
    tmpC=strrchr(argv[0],'/');
    pos=tmpC-argv[0];
    fprintf(stderr,"Pos: %d\n",pos);
    path=(char*)malloc(sizeof(char)*(pos+1));
    strncpy(path,argv[0],pos);
    path[pos]='\0';
    
    fprintf(stderr,"Path: %s\n",path);
    
    ini_graphics();
    setFont(std6x12);
    plugin_font=std6x12;
    iniFont();
    
    set_mouseParam(6, 3);

    fillRect(COLOR_BLACK,0 , 0, 320, 240);

    putS(COLOR_WHITE,COLOR_BLACK,5,110,"Reading menu file ....");
    
    tmpC=(char*)malloc(sizeof(char)*(pos+1+strlen(menuFile)));
    sprintf(tmpC,"%s/%s",path,menuFile);
    
    if(loadMenu(tmpC)<0)
    {
        putS(COLOR_RED,COLOR_BLACK,5,120,"Error reading menu => stoping");
        for(i=0;i<10000;i++) /* nothing */
        close_graphics();
        return -1;
    }

    fillRect(COLOR_BLACK,5 , 110, 315, 10);
#ifdef DO_DEBUG
    printMenu();
#endif
    drawGui();
    setTimerFreq(10);
    startTimer();
    timerOn=1;
    stopWM=0;
    evtOn=1;
    drawMenu();
    eventLoop();
    close_graphics();
    free(path);
    free(tmpC);
    return 0;
}

void stopEvt()     {evtOn=0;  }
void startEvt()    {evtOn=1;  }
void stopTimout()  {timerOn=0; stopTimer(); }
void startTimout() {timerOn=1; startTimer();}

void drawGui(void)
{
    int w = 0;
    int h = 0;

    getStringS("M", &w, &h);

    fillRect(COLOR_WHITE,0 , 0, 320, 240);
    fillRect(COLOR_LIGHT_BLUE,0,0,320,h+6);
    fillRect(COLOR_BLACK,0,h+6,320,MENU_SHADOW);
    putS(COLOR_DARK_GREY,COLOR_LIGHT_BLUE,2,2,"AvWm");

    drawTime();
   draw_batt_status();
}

void drawTime()
{
    char timeSt[50];
    if(showTimeS)
    {
        getTime(timeSt);
        fillRect(COLOR_LIGHT_BLUE,150,2,130,11);
        putS(COLOR_DARK_GREY,COLOR_LIGHT_BLUE,150,2,timeSt);
    }
}

void getTime(char * timeSt)
{
    int fd;
    struct tm date_time;

    fd=open("/dev/avrtc",O_RDONLY | O_NONBLOCK);
    if (fd < 0)
    {
        printf("Can't open /dev/avrtc\n");
        return;
    }

    if(ioctl(fd,AV_RTC_GET_TIME_IOC,&date_time)<0)
    {
        printf("Error getting time and date\n");
        return;
    }

    close(fd);

    sprintf(timeSt, "%02d:%02d:%02d %02d/%02d/%04d", date_time.tm_hour,date_time.tm_min,date_time.tm_sec,
                                                         date_time.tm_mday,date_time.tm_mon,date_time.tm_year);
}

void drawMenu(void)
{
    int evt,status;
    int w = 0;
    int h = 0;

   getStringS("M", &w, &h);

    if(currentHandler)
        currentHandler(EVT_SUSPEND);
    fillRect(COLOR_WHITE,5 , h+6+MENU_SHADOW, 315, 240-h-6-MENU_SHADOW);
    doDraw();
}

void wmNxtEvent(int evt)
{
    //int evt=nxtEvent();
    if(evt==BTN_F3)
    {
        clearEventQueue();
        drawMenu();
    }
    else
    {
        if(currentHandler)
            currentHandler(evt);
    }
}

void addEventHandler(void (*evtHandle))
{
    tmpHandler=evtHandle;
}

void pack(void)
{
    currentHandler=tmpHandler;
}

void eventLoop()
{
    int ret;
    int evt;

    while(!stopWM)
    {
        //while((evt=nxtEvent())==NO_EVENT) /* wait */;
        evt=waitEvent();


        if(timerOn && evt==EVT_TIMER)
        {
            processTimeOut();
        }
        
        if(evtOn)
        {
            if(evt==BTN_F3)
            {
                clearEventQueue();
                drawGui();
                drawMenu();
            }
            else
            {
                if(currentHandler)
                    currentHandler(evt);
            }
        }
    }
}

void processTimeOut()
{
    drawTime();

    if(batteryRefresh >= 20)
    {
      draw_batt_status();
        batteryRefresh = 0;
    }
    else
      batteryRefresh++;
}

void wmgetStringS(const unsigned char *str, int *w, int *h)
{
    FONT_ID font=getFont();
    if(font != plugin_font)
    {
        setFont(plugin_font);
        getStringS(str,w,h);
        setFont(font);
    }
    else
        getStringS(str,w,h);
}

void wmPutS(int color, int bg_color,int x, int y, char *s)
{
    FONT_ID font=getFont();
    if(font != plugin_font)
    {
        setFont(plugin_font);
        putS(color,bg_color,x,y,s);
        setFont(font);
    }
    else
        putS(color,bg_color,x,y,s);
}

void wmPutC(int color, int bg_color,int x, int y, char s)
{
    FONT_ID font=getFont();
    if(font != plugin_font)
    {
        setFont(plugin_font);
        putC(color,bg_color,x,y,s);
        setFont(font);
    }
    else
        putC(color,bg_color,x,y,s);
}

void wmSetFont(int font)
{
    plugin_font=font_table[font];
}

void showTime(void) { showTimeS=1; drawTime(); }
void hideTime(void) { showTimeS=0; }
void showBat(void)  { showBatS=1; draw_batt_status(); }
void hideBat(void)  { showBatS=0; }

static char debugmembuf[200];

void debug(char *fmt, ...)
{
#ifdef DO_DEBUG

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(debugmembuf, sizeof(debugmembuf), fmt, ap);
    putS(COLOR_BLACK,COLOR_WHITE,0,230,debugmembuf);
    va_end(ap);

#endif
}
