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
	pack               : pack
};

void (*currentHandler)(int evt)=NULL;
void (*tmpHandler)(int evt)=NULL;

int plugin_pid=-1;

int launchPlugin(char * name)
{
	int pid,err;
	char tmp[15];
	
	printf("in parent: %x\n",(int)&cops);
	
	sprintf(tmp, "%d",(int)&cops);
	pid = vfork();
	if (pid == 0)
	{ // child       
		err = execl(name, name,tmp,"/");
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

int main(int argc,char * * argv)
{
	int i;
	ini_graphics();	
	setFont(std6x9);
	fillRect(1,0 , 0, 320, 240);
	drawGui();
	/*launchPlugin("/mnt/ls-gui");	
	while(1);*/
	eventLoop();
	close_graphics();
	return 0;
}

void drawGui(void)
{
	char timeSt[]="--:--:--";
	
	fillRect(COLOR_WHITE,0 , 0, 320, 240);
	fillRect(COLOR_LIGHT_BLUE,0,0,320,13);
	fillRect(COLOR_BLACK,0,13,320,2);
	putS(COLOR_DARK_GREY,COLOR_LIGHT_BLUE,2,2,"AvWm");
	getTime(timeSt);
	putS(COLOR_DARK_GREY,COLOR_LIGHT_BLUE,200,2,timeSt);
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

	sprintf(timeSt, "%02x:%02x:%02x", date_time.tm_hour,date_time.tm_min,date_time.tm_sec);
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
				launchPlugin("/mnt/ls-gui");
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
				launchPlugin("/mnt/snow");
				return;
			case BTN_LEFT:
				if(currentHandler)
				{
					currentHandler(EVT_RESUME);
					currentHandler(EVT_REDRAW);
				}
				return;
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
	while(1)
	{
		//while((evt=nxtEvent())==NO_EVENT) /* wait */;
		evt=waitEvent();
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
