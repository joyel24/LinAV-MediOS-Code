/*
* calendar.c
* by Schoki
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
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <asm/ioctl.h>

#include "graphics.h"
#include "events.h"
#include "cops.h"
#include "avevents.h"
#include "colordef.h"

#define DaySpace  23
#define WeekSpace 15
#define MonthHeaderHeight 15
#define XCALENDARPOS 5
#define YCALENDARPOS 30

struct client_operations * cops;

const int wochenLaenge = 7;     /* Eine Woche hat sieben Tage.*/
const int startTagJahrNull = 1;      /* Starttag ist der 01.01.0000.*/

int g_init        = 0;
int g_CurrentHour = 7;

struct today {
    int     mday;        /* day of the month */
    int     mon;         /* month */
    int     year;        /* year since 1900 */
    int     wday;        /* day of the week */
};

struct shown {
    int     mday;        /* day of the month */
    int     mon;         /* month */
    int     year;        /* year since 1900 */
    int     wday;        /* day of the week */
    int     firstday;    /* first (w)day of month */
    int     lastday;     /* last (w)day of month */
};

struct today today;
static int leap_year;
struct shown shown;

void ClearCalendar();
void printNr(char* buffer, int nr, int withSign);
void printTime(char* buffer, int nr);


int montg[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};


static int days_in_month[2][13] = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

/* leap year -- account for gregorian reformation in 1752 */
static int is_leap_year(int yr);
/* searches the weekday of the first day in month,
 * relative to the given values */
static int calc_weekday( struct shown *shown );
static void calendar_init();
static void draw_headers(void);
static void calendar_draw();

static int start = 0;

static void next_month(struct shown *shown, int step);
static void prev_month(struct shown *shown, int step);
static void next_day(struct shown *shown, int step);
static void prev_day(struct shown *shown, int step);

int eventHandler(int evt)
{
	switch (evt)
	{

	case BTN_LEFT:
			prev_month(&shown, 0);
			break;

   case BTN_RIGHT:
	      next_month(&shown, 0);
			break;

   case BTN_F1:
   		// Jump to actual day
	   	calendar_init(&today, &shown);
		   calendar_draw();
			break;

	case BTN_OFF:
	case EVT_QUIT:
		RELEASE(cops)
		break;

	case BTN_DOWN:
		next_day(&shown, 1);
		break;

	case BTN_UP:

		prev_day(&shown, 1);
		break;

	case EVT_REDRAW:
      ClearCalendar();
	   calendar_draw();
		break;
	}
}


/* leap year -- account for gregorian reformation in 1752 */
static int is_leap_year(int yr)
{
    return ((yr) <= 1752 ? !((yr) % 4) : \
    (!((yr) % 4) && ((yr) % 100)) || !((yr) % 400))  ? 1:0 ;
}

/* searches the weekday of the first day in month,
 * relative to the given values */
static int calc_weekday( struct shown *shown )
{
    return ( shown->wday + 36 - shown->mday ) % 7 ;
}


static void calendar_init()
{
   int w,h;
   struct av_tm tm;
   int fd;
	char tmp[100];

   /*fd=open("/dev/avrtc",O_RDONLY | O_NONBLOCK);
   if (fd < 0)
	{
      printf("Can't open /dev/avrtc\n");
   }

	if(ioctl(fd,AV_RTC_GET_TIME_IOC,&tm)<0)
	{
      printf("Error getting time and date\n");
   }
   close(fd);*/
   
   cops->getTime(&tm);

/*	sprintf(tmp, "%02d",tm.tm_wday);
   today.wday = atoi(tmp);
	sprintf(tmp, "%04d",tm.tm_year);
   today.year = atoi(tmp);//%100;
	sprintf(tmp, "%02d",tm.tm_mon);
   today.mon = atoi(tmp);
	sprintf(tmp, "%02d",tm.tm_mday);
   today.mday = atoi(tmp);
*/


   shown.mday = today.mday = tm.tm_mday;
   shown.mon  = today.mon  = tm.tm_mon;
   shown.year = today.year = tm.tm_year;
   shown.wday = today.wday = tm.tm_wday;

   shown.firstday = calc_weekday(&shown);
   leap_year = is_leap_year(shown.year);
}

static void draw_headers(void)
{
    int i;
    char *Dayname[8] = {"Mo","Tu","We","Th","Fr","Sa","Su",""};
    int ws = XCALENDARPOS;

    for (i = 0; i < 8;)
    {
        cops->putS(COLOR_BLACK, COLOR_WHITE, ws+2, YCALENDARPOS+MonthHeaderHeight, Dayname[i++]);
        ws += DaySpace;
    }
}

static void calendar_draw()
{
    int ws,row,pos,days_per_month,j;
    char buffer[9];
    char tmp[100];
	 int color;

    char *Monthname[] = {
                          "Jan",
                          "Feb",
                          "Mar",
                          "Apr",
                          "May",
                          "Jun",
                          "Jul",
                          "Aug",
                          "Sep",
                          "Oct",
                          "Nov",
                          "Dec"
                        };
    ClearCalendar();

	 cops->fillRect(COLOR_WHITE,XCALENDARPOS, YCALENDARPOS, 170, 2*WeekSpace);

    snprintf(buffer,9,"%s %04d",Monthname[shown.mon-1],shown.year);
	 cops->putS(COLOR_BLACK, COLOR_WHITE, XCALENDARPOS+2, YCALENDARPOS+1,buffer);

    draw_headers();

	 if (shown.firstday > 6)
        shown.firstday -= 7;

    row = 1;
    pos = shown.firstday;
    days_per_month = days_in_month[leap_year][shown.mon];
    ws = XCALENDARPOS + (pos * DaySpace);

    for (j = 1; j <= days_per_month;j++)
    {
        snprintf(buffer,4,"%02d", j);

		  if( (pos == 5) || (pos == 6) )
		  {
		      // Wochenende --> Tage in rot schreiben
				color = COLOR_RED;
		  }
		  else
		  {
		      color = COLOR_BLACK;
		  }

		  if(j == shown.mday)
		  {
		     // Den selektierten Tag besonders farblich markieren --> höchste Prio der Farben
			  color = COLOR_GREY;
		  }

		  if((j == today.mday) && (shown.mon == today.mon) && (shown.year == today.year))
		  {
		     color = COLOR_BLUE;
		  }

        cops->putS(color, COLOR_WHITE, ws+2, YCALENDARPOS + MonthHeaderHeight + 2 + row * WeekSpace,buffer);

        if (shown.mday == j)
        {
            shown.wday = pos;
        }

        ws += DaySpace;
        pos++;
        if (pos >= 7)
        {
            row++;
            pos = 0;
            ws = XCALENDARPOS;
        }
    }

    shown.lastday = pos;
}

static void next_month(struct shown *shown, int step)
{
    shown->mon++;
    if (shown->mon > 12)
    {
        shown->mon=1;
        shown->year++;
        leap_year = is_leap_year(shown->year);
    }
    else if (step > 0)
        shown->mday = shown->mday - days_in_month[leap_year][shown->mon-1];
    else if (shown->mday > days_in_month[leap_year][shown->mon])
        shown->mday = days_in_month[leap_year][shown->mon];
    shown->firstday = shown->lastday;
    calendar_draw();
}

static void prev_month(struct shown *shown, int step)
{
    shown->mon--;
    if (shown->mon < 1)
    {
        shown->mon = 12;
        shown->year--;
        leap_year = is_leap_year(shown->year);
    }
    if (step > 0)
        shown->mday = shown->mday + days_in_month[leap_year][shown->mon];
    else if (shown->mday > days_in_month[leap_year][shown->mon])
        shown->mday = days_in_month[leap_year][shown->mon];
    shown->firstday += 7 - (days_in_month[leap_year][shown->mon] % 7);
    calendar_draw();
}

static void next_day(struct shown *shown, int step)
{
    shown->mday += step;
    if (shown->mday > days_in_month[leap_year][shown->mon])
	 {
        next_month(shown, step);
	 }
    else
	 {
        calendar_draw();
	 }
}

static void prev_day(struct shown *shown, int step)
{
    shown->mday -= step;
    if (shown->mday < 1)
        prev_month(shown, step);
    else
        calendar_draw();
}

void printNr(char* buffer, int nr, int withSign)
{
   int begin = 0;

   if(withSign == 1)
   {
      if(nr < 0)
      {
         nr=nr*(-1);
         buffer[begin++] = '-';
      }
      else
      {
         buffer[begin++] = '+';
      }
   }

   buffer[begin++] = '0' + (nr / 10000) % 10;
   buffer[begin++] = '0' + (nr / 1000) % 10;
   buffer[begin++] = '0' + (nr / 100) % 10;
   buffer[begin++] = '0' + (nr / 10) % 10;
   buffer[begin++] = '0' + (nr / 1) % 10;
   buffer[begin++] = 0;
}

void printTime(char* buffer, int nr)
{
   buffer[0] = '0' + (nr / 10) % 10;
   buffer[1] = '0' + (nr / 1) % 10;
   buffer[2] = 0;
}

void ClearCalendar()
{
	 cops->fillRect(COLOR_WHITE,XCALENDARPOS, YCALENDARPOS-12, 170, 132);
}

int main(int argc,char * * argv)
{
    REGISTER(cops,eventHandler,0);
    calendar_init();
    calendar_draw();  
    PACK(cops,NULL)
    return 0;
}



