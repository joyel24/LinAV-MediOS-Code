/* events.c file */

#include <stdio.h>
#include <stdlib.h>
#include "events.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>


extern Display* display;
extern Window window;	/*variable Event */

int nxtEvent(void)
 {
    XEvent event;
    
    XSelectInput(display, window, ExposureMask);
    /* next event */
    XNextEvent(display, &event);
    
    switch (event.type) 
    {
      case Expose : 
	printf("Événement Expose :)\n");
      break;
    }
    return 0; 
  }  

/* 

  int waitEvent(void)
{
    int evt;
    if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_WAIT_EVENT,&evt)<0)
        return -1;
    return evt+1;
}

int iniEvent(void)
{
    if(fdEv<0);
    {
        fdEv=open("/dev/mouse",O_RDONLY);
        if (fdEv < 0)
        {
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

int wakeUP(void)
{    
    if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_DO_WAKEUP,NULL)<0)
        return -1;    
    return 0;    
}

int halt_device(void)
{    
    if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_HALT_DEVICE,NULL)<0)
        return -1;    
    return 0;    
}

int pause_app(void)
{    
    if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_PAUSE_APP,NULL)<0)
        return -1;    
    return 0;    
}

int release_app(void)
{    
    if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_RELEASE_APP,NULL)<0)
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

int setTimerFreq(int val)
{
    if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_SET_TIMER_FREQ,&val)<0)
        return -1;
    return 0;
}

int startTimer()
{
    if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_START_TIMER)<0)
        return -1;
    return 0;
}

int stopTimer()
{
    if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_STOP_TIMER)<0)
        return -1;
    return 0;
}

int timerState()
{
    int ret;
    if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_TIMER_STATE,&ret)<0)
        return -1;
    return 0;
}

*/
