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

#define DO_DEBUG 1;

needFont(std6x9);

struct client_operations cops={
	drawPixel          : drawPixel,
	readPixel          : readPixel,
	drawRect           : drawRect,
	fillRect           : fillRect,
	drawLine           : drawLine,
	putS               : putS,
	putC               : putC,
	//drawSprite         : drawSprite,
	//drawBITMAP         : drawBITMAP,
	scrollWindowVert   : scrollWindowVert,
	scrollWindowHoriz  : scrollWindowHoriz,
	clearEventQueue    : clearEventQueue,
	nxtEvent           : nxtEvent,
	processEvent       : wmNxtEvent,
	addEventHandler    : addEventHandler,
	pack               : pack,
	drawImage          : drawImage
};

void (*currentHandler)(int evt)=NULL;
void (*tmpHandler)(int evt)=NULL;

int plugin_pid=-1;

int launchPlugin(char * name,char * param)
{
	int pid,err;
	char cops_a[15];
	
	sprintf(cops_a, "%d",(int)&cops);
	pid = vfork();
	if (pid == 0)
	{ // child
		if(param!=NULL)
			err = execl(name, name,param,cops_a,(char *)0);
		else
			err = execl(name, name,cops_a,(char *)0);
		fprintf(stderr, "exec failed!%s %d %ld\n", name, err, errno);
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
int stopWM;

int main(int argc,char * * argv)
{
	int i;
	ini_graphics();	
	setFont(std6x9);
	fillRect(1,0 , 0, 320, 240);
	drawGui();
	/*launchPlugin("/mnt/ls-gui");	
	while(1);*/
	setTimerFreq(10);
	startTimer();
	timerOn=1;
	stopWM=0;
	eventLoop();
	close_graphics();
	return 0;
}

void drawGui(void)
{
	fillRect(COLOR_WHITE,0 , 0, 320, 240);
	fillRect(COLOR_LIGHT_BLUE,0,0,320,13);
	fillRect(COLOR_BLACK,0,13,320,2);
	putS(COLOR_DARK_GREY,COLOR_LIGHT_BLUE,2,2,"AvWm");
	drawTime();
}

void drawTime()
{
	char timeSt[50];
	getTime(timeSt);
	fillRect(COLOR_LIGHT_BLUE,150,2,170,11);	
	putS(COLOR_DARK_GREY,COLOR_LIGHT_BLUE,150,2,timeSt);
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
	if(currentHandler)
		currentHandler(EVT_SUSPEND);
	fillRect(COLOR_WHITE,110 , 70, 100, 100);
	drawRect(COLOR_LIGHT_GREY,110 , 70, 100, 100);
	putS(COLOR_DARK_GREY,COLOR_WHITE,110+32,70+5,"Browse");
	putS(COLOR_DARK_GREY,COLOR_WHITE,110+5,70+45,"Back");
	putS(COLOR_DARK_GREY,COLOR_WHITE,110+70,70+45,"Snow");
	while(1)
	{
		evt=waitEvent();
		switch(evt) {
			case BTN_UP:
				if(currentHandler)
				{
					currentHandler(EVT_RESUME);
					currentHandler(EVT_QUIT);
					currentHandler=NULL;
					waitpid(plugin_pid, &status, 0);
					plugin_pid=-1;
				}
				drawGui();
				launchPlugin("/mnt/ls-gui","/");
				return;
			case BTN_RIGHT:
				if(currentHandler)
				{
					currentHandler(EVT_QUIT);
					currentHandler=NULL;
					waitpid(plugin_pid, &status, 0);
					plugin_pid=-1;
				}
				drawGui();
				launchPlugin("/mnt/snow",NULL);
				return;
			case BTN_LEFT:
				if(currentHandler)
				{
					currentHandler(EVT_RESUME);
					currentHandler(EVT_REDRAW);
				}
				return;
			case BTN_OFF:
				stopWM=1;;
		}
	}
	
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
	return 0;
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
}

void processTimeOut()
{
	drawTime();
}

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
