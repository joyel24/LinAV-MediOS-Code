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

/* Global variables */
extern int stateUSB;
extern int statePWR;
extern int stateBAT;

int getTimeS(char * timeSt)
{    
    sprintf(timeSt, "%02d:%02d:%02d %02d/%02d/%04d", 0,0,0,0,0,0);
    return 1;
}

int getTime(struct tm * date_time)
{
       return 1;
}

int set_mouseParam(int freq, int repeat)
{
    return 1;
}
int getBat(void)
{
        printf("%d\n", stateBAT);
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

int getTick(void)
{
    return 1;
}

