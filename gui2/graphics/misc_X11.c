/*
* misc_X11.c
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

#include "misc.h"
#include "events.h"

#include "time.h"

#if 0
// Linux tm struct
struct tm {
   int tm_hour;   /* hour (0 - 23) */
   int tm_isdst;  /* daylight saving time enabled/disabled */
   int tm_mday;   /* day of month (1 - 31) */
   int tm_min;    /* minutes (0 - 59) */
   int tm_mon;    /* month (0 - 11 : 0 = January) */
   int tm_sec;    /* seconds (0 - 59) */
   int tm_wday;   /* Day of week (0 - 6 : 0 = Sunday) */
   int tm_yday;   /* Day of year (0 - 365) */
   int tm_year;   /* Year less 1900 */
}
#endif


/* Global variables */
extern int stateUSB;
extern int statePWR;
extern int stateBAT;

int getTimeS(char * timeSt)
{
    struct av_tm date_time={0,0,0,0,0,0,0,0};

    if(getTime(&date_time))
    {
    	sprintf(timeSt, "%02d:%02d:%02d %02d/%02d/%04d", date_time.tm_hour,date_time.tm_min,date_time.tm_sec,
                                                         date_time.tm_mday,date_time.tm_mon,date_time.tm_year);
        return 1;
    }
    else
    {
        sprintf(timeSt, "%02d:%02d:%02d %02d/%02d/%04d", 0,0,0,0,0,0);
    	return 0;
    }    
}

int getTime(struct av_tm * date_time)
{
    time_t rawtime;
    struct tm * timeinfo;
    
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    
    date_time->tm_hour  = timeinfo->tm_hour;
    date_time->tm_min   = timeinfo->tm_min;
    date_time->tm_sec   = timeinfo->tm_sec;
    date_time->tm_mday  = timeinfo->tm_mday;
    date_time->tm_mon   = timeinfo->tm_mon+1;
    date_time->tm_year  = timeinfo->tm_year+1900;
    date_time->tm_ms    = 0;
    date_time->tm_wday  = timeinfo->tm_wday;
    if(date_time->tm_wday==0)
        date_time->tm_wday=7;
    date_time->tm_wday--;
    
    //printf("today: %d/%d/%d wday:%d\n",date_time->tm_mday,date_time->tm_mon,date_time->tm_year,date_time->tm_wday);
   
    return 1;
}

int set_mouseParam(int freq, int repeat)
{
    return 1;
}

int get_mouseFreq()
{
    return 1;
}

int get_mouseRepeat()
{
    return 1;
}

int getBat(void)
{
        return stateBAT;
}

int getPwr(void)
{
    return statePWR;
}

int getUSB(void)
{   
    return stateUSB;
}

int setUSB(int state)
{
    return 0;
}

int startTick =-1;

int getTick(void)
{
    /* don't want to access the jiffies value */
    /* we will use the number of sec since the last round hour*/
    time_t rawtime;
    if(startTick==-1)
    {
        time ( &rawtime );
        startTick=(((int)rawtime)/3600)*3600;
    }
    time ( &rawtime );
    
    return ((int)(rawtime)-startTick);
}

