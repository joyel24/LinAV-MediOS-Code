/*
* misc.c
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

#include <fcntl.h>
#include <sys/ioctl.h>

#include "misc.h"

int getTimeS(char * timeSt)
{    
    struct tm date_time;

    if(getTime(&date_time))
    	sprintf(timeSt, "%02d:%02d:%02d %02d/%02d/%04d", date_time.tm_hour,date_time.tm_min,date_time.tm_sec,
                                                         date_time.tm_mday,date_time.tm_mon,date_time.tm_year);
    else
    	return 0;
    return 1;
}

int getTime(struct tm * date_time)
{
	int fd;
        
	fd=open("/dev/avrtc",O_RDONLY | O_NONBLOCK);
        if (fd < 0)
        {
            printf("Can't open /dev/avrtc\n");
            return 0;
        }
                
        if(ioctl(fd,AV_RTC_GET_TIME_IOC,date_time)<0)
        {
            printf("Error getting time and date\n");
            return 0;
        }        
        
        close(fd);
        
        return 1;
}

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
int getBat(void)
{
	int fd,power;
        
 	fd=open("/dev/avtsc",O_RDONLY | O_NONBLOCK);
        
        if (fd < 0)
        {
            printf("Can't open /dev/avtsc\n");
            return -1;
        }
        
        if(ioctl(fd,AV_LEVEL_BAT0_IOC,&power)<0)
        {
            printf("Error getting power value\n");
            return -1;
        }
        
        close(fd);
        
        return power;
}