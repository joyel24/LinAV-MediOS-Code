/*
* events.c
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "events.h"
#include "misc.h"

#define doMouseIoctl(ioctl_cmd,param,error,on_error)  {if(gen_ioctl("/dev/mouse",ioctl_cmd,param)<0){printf(error);return on_error;}}

int fdEv=-1;
struct mouseParam settings; // freq - repeated_press

int nxtEvent(void)
{
    int evt;
    doMouseIoctl(AV_GET_EVENT,&evt,"Error getting nxtEvent",0);
    /*if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_GET_EVENT,&evt)<0)
        return -1;
    */
    return evt+1;
}

int waitEvent(void)
{
    int evt;
    doMouseIoctl(AV_WAIT_EVENT,&evt,"Error in waitEvent ioctl",0);
    /*if(fdEv<0)
    {
        fprintf(stderr,"[waitEvent] no fd connection\n");
        return NO_EVENT;
    }
    if(ioctl(fdEv,AV_WAIT_EVENT,&evt)<0)
    {
        fprintf(stderr,"[waitEvent] ioctl err fd=%d\n",fdEv);
        return NO_EVENT;
    }*/
    
    return evt+1;
}

int iniEvent(void)
{
    /*if(fdEv<0);
    {
        fdEv=open("/dev/mouse",O_RDONLY);
        if (fdEv < 0)
        {
            printf("Can't open /dev/mouse\n");
            return -1;
        }
        
        if(ioctl(fdEv,AV_CLEAR_EVENTS,NULL)<0)
            return -1;
    }*/
    doMouseIoctl(AV_CLEAR_EVENTS,NULL,"Error clearing evt queue",-1);
    return 0;
}

int clearEventQueue(void)
{
    /*if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_CLEAR_EVENTS,NULL)<0)
        return -1;*/
    doMouseIoctl(AV_CLEAR_EVENTS,NULL,"Error clearing evt queue",-1);
    return 0;    
}

int wakeUP(void)
{    
    /*if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_DO_WAKEUP,NULL)<0)
        return -1;    */
    doMouseIoctl(AV_DO_WAKEUP,NULL,"Error waking up paused app",-1);
    return 0;    
}

int pause_app(void)
{    
    /*if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_PAUSE_APP,NULL)<0)
        return -1; */
    doMouseIoctl(AV_PAUSE_APP,NULL,"Error pausing app",-1);   
    return 0;    
}

int release_app(void)
{    
    /*if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_RELEASE_APP,NULL)<0)
        return -1;*/
    doMouseIoctl(AV_RELEASE_APP,NULL,"Error releasing app",-1);     
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
    /*if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_SET_MOUSE_PARAM,param)<0)
        return -1;*/
    doMouseIoctl(AV_SET_MOUSE_PARAM,param,"Error setting btn param",-1);
    return 0;
}

int getSettings(struct mouseParam * param)
{
    /*if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_GET_MOUSE_PARAM,param)<0)
        return -1;*/
    doMouseIoctl(AV_GET_MOUSE_PARAM,param,"Error getting btn param",-1);
    return 0;
}

int setTimerFreq(int val)
{
    /*if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_SET_TIMER_FREQ,&val)<0)
        return -1;*/
    doMouseIoctl(AV_SET_TIMER_FREQ,&val,"Error setting timer freq",-1);
    return 0;
}

int startTimer()
{
    /*if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_START_TIMER)<0)
        return -1;*/
    doMouseIoctl(AV_START_TIMER,NULL,"Error starting timer",-1);
    return 0;
}

int stopTimer()
{
    /*if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_STOP_TIMER)<0)
        return -1;*/
    doMouseIoctl(AV_STOP_TIMER,NULL,"Error stopping timer",-1);
    return 0;
}

int timerState()
{
    int ret;
    /*if(fdEv<0)
        return -1;
    if(ioctl(fdEv,AV_TIMER_STATE,&ret)<0)
        return -1;*/
    doMouseIoctl(AV_TIMER_STATE,&ret,"Error getting timer state",-1);
    return 0;
}
