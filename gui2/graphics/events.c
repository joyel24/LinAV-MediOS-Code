#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "av3xx_common.h"
#include "events.h"


int fdEv=-1;
struct mouseParam settings; // freq - repeated_press

int nxtEvent(void)
{
	int evt;
	if(fdEv<0)
		return -1;
	if(ioctl(fdEv,AV_GET_EVENT,&evt)<0)
		return -1;
	
	return evt+1;	
}

int iniEvent(void)
{
	if(fdEv<0);
	{
		fdEv=open("/dev/mouse",O_RDONLY);
    		if (fdEv < 0) {
        		printf("Can't open /dev/mouse\n");
        		return -1;
    		}
		
		if(ioctl(fdEv,AV_CLEAR_EVENTS,NULL)<0)
			return -1;
	}
	return 0;
}

int clearEventQueue(void)
{
	if(fdEv<0)
		return -1;
	if(ioctl(fdEv,AV_CLEAR_EVENTS,NULL)<0)
		return -1;
	
	return 0;	
}

int setRepeate(int val)
{
	if(getSettings(&settings)<0)
		return -1;
	settings.repeated_press=val;
	if(setSettings(&settings)<0)
		return -1;
	return 0;
}

int getRepeate(void)
{
	if(getSettings(&settings)<0)
		return -1;
	return settings.repeated_press;
}

int setFreq(int val)
{
	if(getSettings(&settings)<0)
		return -1;
	settings.freq=val;
	if(setSettings(&settings)<0)
		return -1;
	return 0;
}

int getFreq(void)
{
	if(getSettings(&settings)<0)
		return -1;
	return settings.freq;
}

int setSettings(struct mouseParam * param)
{
	if(fdEv<0)
		return -1;
	if(ioctl(fdEv,AV_SET_MOUSE_PARAM,param)<0)
		return -1;
	return 0;
}

int getSettings(struct mouseParam * param)
{
	if(fdEv<0)
		return -1;
	if(ioctl(fdEv,AV_GET_MOUSE_PARAM,param)<0)
		return -1;
	return 0;
}