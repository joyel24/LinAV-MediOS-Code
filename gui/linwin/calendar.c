/*
   acalendar.c, a simple calendar for AV3xx

   Copyright 2004, Goetz Minuth

   This File is free software; I give unlimited permission to copy
   and/or distribute it, with or without modifications, as long as this
   notice is preserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "av3xx_common.h"
#include "av3xx_colordef.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <asm/ioctl.h>
#include "pz.h"
#include "piezo.h"

#define LCD_WIDTH  320
#define LCD_HEIGHT 240

#define DaySpace  23
#define WeekSpace 15
#define MonthHeaderHeight 15
#define XCALENDARPOS 5
#define YCALENDARPOS 20

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

void ClearScreen();
void ClearCalendar();
void printNr(char* buffer, int nr, int withSign);
void printTime(char* buffer, int nr);


int montg[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};


static int days_in_month[2][13] = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

static GR_WINDOW_ID calendar_wid;
static GR_GC_ID calendar_gc;
static GR_SCREEN_INFO screen_info;

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

static int calendar_do_keystroke(GR_EVENT * event)
{
   GR_EVENT nextevent;
	int ret = 0;

	// Filter double Events
	delay(15000);
   GrPeekEvent(&nextevent);

	do
	{
		if(nextevent.type == GR_EVENT_TYPE_KEY_DOWN)
		{
			// remove event
			GrCheckNextEvent(&nextevent);
		}

		GrPeekEvent(&nextevent);
	}
	while(nextevent.type == GR_EVENT_TYPE_KEY_DOWN);

	switch (event->keystroke.ch) {

	case 'l':
			prev_month(&shown, 0);
			ret = 1;
			break;

   case 'r':
	      next_month(&shown, 0);
         ret = 1;
			break;

   case '1':
   		// Jump to actual day
	   	calendar_init(&today, &shown);
		   calendar_draw();
         ret = 1;
			break;

	case 'f':
		pz_close_window(calendar_wid);
		ret = 1;
		break;

	case 'd':
		next_day(&shown, 1);
		ret = 1;
		break;

	case 'u':

		prev_day(&shown, 1);
		ret = 1;
		break;
	}

	// Filter double Events
	delay(15000);
	GrPeekEvent(&nextevent);

	do
	{
		if(nextevent.type == GR_EVENT_TYPE_KEY_DOWN)
		{
			// remove event
			GrCheckNextEvent(&nextevent);
		}

		GrPeekEvent(&nextevent);
	}
	while(nextevent.type == GR_EVENT_TYPE_KEY_DOWN);

	return ret;
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
   struct tm tm;
   int fd;
	char tmp[100];

   fd=open("/dev/avrtc",O_RDONLY | O_NONBLOCK);
   if (fd < 0)
	{
      printf("Can't open /dev/avrtc\n");
   }

	if(ioctl(fd,AV_RTC_GET_TIME_IOC,&tm)<0)
	{
      printf("Error getting time and date\n");
   }
   close(fd);
/*
	GrSetGCForegroundPixelVal(calendar_gc, AV3XX_COLOR_BLACK);
	sprintf(tmp, "%d %d %d %d %d %d %d",tm.tm_wday,tm.tm_year,tm.tm_mon,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
	GrText(calendar_wid, calendar_gc, 10, 200,tmp, -1, GR_TFASCII);
*/



   today.mon = tm.tm_mon;
   today.year = 2000+tm.tm_year%100;
/*
   char timeSt[] = "xx";
	stringPutHexA(timeSt,   tm.tm_mon,  2); // xxx
*/

   today.mday = tm.tm_mday;
   today.wday = tm.tm_wday;

   shown.mday = today.mday;
   shown.mon = today.mon;
   shown.year = today.year;
   shown.wday = today.wday;

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
        GrSetGCForegroundPixelVal(calendar_gc, AV3XX_COLOR_BLACK);
		  GrText(calendar_wid, calendar_gc, ws+2, YCALENDARPOS+MonthHeaderHeight,Dayname[i++], -1, GR_TFASCII);
        ws += DaySpace;
    }
}

static void calendar_do_draw(GR_EVENT *event)
{
   if(g_init == 0)
	{
   	pz_draw_header("Calendar");
	   calendar_init();
		g_init = 1;
	}

	calendar_draw();
}

static void calendar_draw()
{
    int ws,row,pos,days_per_month,j;
    char buffer[9];
   	char tmp[100];

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

    GrSetGCForegroundPixelVal(calendar_gc, AV3XX_COLOR_WHITE);
    GrFillRect(calendar_wid, calendar_gc, XCALENDARPOS, YCALENDARPOS, LCD_WIDTH-XCALENDARPOS, 2*WeekSpace);

    GrSetGCForegroundPixelVal(calendar_gc, AV3XX_COLOR_BLACK);
    snprintf(buffer,9,"%s %04d",Monthname[shown.mon-1],shown.year);
    GrText(calendar_wid, calendar_gc, XCALENDARPOS+2, YCALENDARPOS+1,buffer, -1, GR_TFASCII);

    draw_headers();

	 if (shown.firstday > 6)
        shown.firstday -= 7;

    row = 1;
    pos = shown.firstday;
    days_per_month = days_in_month[leap_year][shown.mon];
    ws = XCALENDARPOS + (pos * DaySpace);
/*
	GrSetGCForegroundPixelVal(calendar_gc, AV3XX_COLOR_BLACK);
	sprintf(tmp, "%d %d %d",leap_year, shown.mon, days_per_month);
	GrText(calendar_wid, calendar_gc, 180, 200,tmp, -1, GR_TFASCII);
*/
    for (j = 1; j <= days_per_month;j++)
    {
        snprintf(buffer,4,"%02d", j);

		  if( (pos == 5) || (pos == 6) )
		  {
		      // Wochenende --> Tage in rot schreiben
            GrSetGCForegroundPixelVal(calendar_gc, AV3XX_COLOR_RED);
		  }
		  else
		  {
            GrSetGCForegroundPixelVal(calendar_gc, AV3XX_COLOR_BLACK);
		  }

		  if(j == shown.mday)
		  {
		     // Den selektierten Tag besonders farblich markieren --> höchste Prio der Farben
            GrSetGCForegroundPixelVal(calendar_gc, AV3XX_COLOR_GREY);
		  }

		  if((j == today.mday) && (shown.mon == today.mon) && (shown.year == today.year))
		  {
            GrSetGCForegroundPixelVal(calendar_gc, AV3XX_COLOR_BLUE2);
		  }

        GrText(calendar_wid, calendar_gc, ws+2, YCALENDARPOS + MonthHeaderHeight + 2 + row * WeekSpace,buffer, -1, GR_TFASCII);

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

void ClearScreen()
{
    // Clear full Screen with black color
//xxx   graphicsBoxfA(&screenBitmap, 0, 0, 640, 240, BACKCOLOR_MAIN);
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
    GrSetGCForegroundPixelVal(calendar_gc, AV3XX_COLOR_WHITE);
    GrFillRect(calendar_wid, calendar_gc, XCALENDARPOS, YCALENDARPOS-10, 170, 120);
}


void new_calendar_window(void)
{
	GrGetScreenInfo(&screen_info);

	calendar_gc = GrNewGC();
	GrSetGCUseBackground(calendar_gc, GR_TRUE);
  	GrSetGCForegroundPixelVal(calendar_gc, AV3XX_COLOR_BLACK);

	calendar_wid = pz_new_window(0, HEADER_TOPLINE + 1, screen_info.cols,
                                   screen_info.rows - (HEADER_TOPLINE + 1),
                                   calendar_do_draw, calendar_do_keystroke);

	GrSelectEvents(calendar_wid, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN);

	GrMapWindow(calendar_wid);
}

