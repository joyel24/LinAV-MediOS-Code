/*
 * Copyright (C) 2004 Bernard Leach
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
//#include <assert.h>
#include <sys/time.h>
#include "pz.h"
#include "browser.h"
#include "calendar.h"
#include "ipod.h"
#include "av3xx_colordef.h"
#include "av3xx_common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <asm/ioctl.h>

static GR_WINDOW_ID root_wid;
static GR_GC_ID root_gc;
static GR_SCREEN_INFO screen_info;

struct pz_window {
	GR_WINDOW_ID wid;
	void (*draw)(void);
	int (*keystroke)(GR_EVENT * event);
};

static struct pz_window windows[10];
static int n_opened = 0;
/* int BACKLIGHT_TIMER = 10; // in seconds */

static unsigned long int last_button_event = 0;
static unsigned long int last_keypress_event = 0;
static unsigned long int wheel_evt_count = 0;

#define WHEEL_EVT_MOD   3

char key_pressed = '\0';

/*
+----+
|    ++
| ||  |
| ||  |
|    ++
+----+
*/

static GR_POINT batt_outline[] = {
	{298, 6},
	{316, 6},
	{316, 8},
	{317, 8},
	{317, 12},
	{316, 12},
	{316, 15},
	{298, 15},
	{298, 6}
};

#define BATT_POLY_POINTS 9

void reboot_ipod(void)
{
	GrClose();
	execl("/bin/reboot", "reboot", NULL);
	exit(1);
}

void quit_podzilla(void)
{
//   ipod_save_settings();
	GrClose();
	exit(1);
}                

void set_wheeldebounce(void)
{
	new_slider_widget(WHEEL_DEBOUNCE, "Wheel Sensitivity", 1, 20);
}
                                                                                
void set_buttondebounce(void)
{
	new_slider_widget(ACTION_DEBOUNCE, "Action Debounce", 100, 500);
}

void set_keyrepeat(void)
{
	new_slider_widget(KEY_REPEAT, "Key Repeat", 0, 5);
}

void set_keyfreq(void)
{
	new_slider_widget(KEY_FREQ, "Key Freqt", 0, 100);
}

static void event_handler(GR_EVENT *event)
{
	int i;
	unsigned long int curtime;
	struct timeval cur_st;
	GR_WINDOW_ID wid = GR_ROOT_WINDOW_ID;
/*
   int fd;
   struct tm date_time;

   fd=open("/dev/avrtc",O_RDONLY | O_NONBLOCK);
   if (fd < 0)
	{
      printf("Can't open /dev/avrtc\n");
   }

	if(ioctl(fd,AV_RTS_GET_TIME_IOC,&date_time)<0)
	{
      printf("Error getting time and date\n");
   }
   close(fd);

   curtime = (date_time.tm_sec % 1000) * 1000;// + date_time.tm_ms;
*/

	gettimeofday( &cur_st, NULL);
	curtime = (cur_st.tv_sec % 1000) * 1000 + cur_st.tv_usec / 1000;

	switch (event->type) {
	case GR_EVENT_TYPE_TIMEOUT:
		// Test to see if backlight timer has expired, if so, turn it off.
		if (ipod_get_setting(BACKLIGHT_TIMER) > 0) {
			if ((curtime - last_keypress_event) > ((ipod_get_setting(BACKLIGHT_TIMER) + 1) * 1000)) {
				ipod_set_setting(BACKLIGHT, 0);
			}
		}
		break;

	case GR_EVENT_TYPE_EXPOSURE:
		wid = ((GR_EVENT_EXPOSURE *)event)->wid;
		for (i = 0; i < n_opened; i++) {
			if (windows[i].wid == wid && wid != GR_ROOT_WINDOW_ID) {
				windows[i].draw();
			}
		}
		break;

	case GR_EVENT_TYPE_KEY_DOWN:
		wid = ((GR_EVENT_KEYSTROKE *)event)->wid;

/*

		// If backlight timer isn't off and backlight isn't on turn it on.

		last_keypress_event = curtime;

		if (ipod_get_setting(BACKLIGHT_TIMER) > 0) {
			ipod_set_setting(BACKLIGHT, 1);
		}

		switch (event->keystroke.ch) {
		case '1':
		case '2':
		case '3':
		case 'f':
		case 'l':
   	case 'r':
			if (curtime - last_button_event > ipod_get_setting(ACTION_DEBOUNCE)) {
				last_button_event = curtime;
			}
			else {
				wid = GR_ROOT_WINDOW_ID;
			}
			break;

		case 'u':
		case 'd':
			wheel_evt_count++;
			if (wheel_evt_count % ipod_get_setting(WHEEL_DEBOUNCE) != 0) {
				wid = GR_ROOT_WINDOW_ID;
			}
			break;
		}
*/

		key_pressed = event->keystroke.ch;
		for (i = 0; i < n_opened; i++) {
			if (windows[i].wid == wid && wid != GR_ROOT_WINDOW_ID) {
				if(windows[i].keystroke(event) == 1)
					beep();
			}
		}
		break;

	case GR_EVENT_TYPE_KEY_UP:
		wid = ((GR_EVENT_KEYSTROKE *)event)->wid;
		event->keystroke.ch = key_pressed;
		break;

	case GR_EVENT_TYPE_TIMER:
		windows[n_opened-1].keystroke(event);
		break;

	default:
		printf("AN UNKNOWN EVENT OCCURED!!\n");
	}
}

static void draw_batt_status()
{
   char tmp[10];
   int fd;
   int power = 0;

   fd=open("/dev/avpower",O_RDONLY | O_NONBLOCK);
   if (fd < 0)
	{
      printf("Can't open /dev/avpower\n");
   }

	if(ioctl(fd,AV_LEVEL_BAT0_IOC,&power)<0)
	{
      printf("Error getting power value\n");
   }
   close(fd);

	GrPoly(root_wid, root_gc, BATT_POLY_POINTS, batt_outline);

	if(power < 1350)
   	GrSetGCForegroundPixelVal(root_gc, AV3XX_COLOR_DARK_RED);
	else if(power < 1500)
   	GrSetGCForegroundPixelVal(root_gc, AV3XX_COLOR_ORANGE2);
   else
   	GrSetGCForegroundPixelVal(root_gc, AV3XX_COLOR_GREEN);

	sprintf(tmp, "%d", power);
	GrText(root_wid, root_gc, 250, HEADER_BASELINE, tmp, -1, GR_TFASCII);

	GrFillRect(root_wid, root_gc, 300, 8, 15, 6);
	GrSetGCForegroundPixelVal(root_gc, AV3XX_COLOR_BLACK); // set color back to black
}

static void draw_date()
{
   int fd;
   struct tm date_time;
   char timeSt[11];

   fd=open("/dev/avrtc",O_RDONLY | O_NONBLOCK);
   if (fd < 0)
	{
      printf("Can't open /dev/avrtc\n");
   }

	if(ioctl(fd,AV_RTC_GET_TIME_IOC,&date_time)<0)
	{
      printf("Error getting time and date\n");
   }
   close(fd);

	sprintf(timeSt, "%02d.%02d.20%02d", date_time.tm_mday,date_time.tm_mon,date_time.tm_year);

	GrSetGCForegroundPixelVal(root_gc, AV3XX_COLOR_BLACK);
	GrText(root_wid, root_gc, 5, HEADER_BASELINE, timeSt, -1, GR_TFASCII);
}

void pz_draw_header(char *header)
{
	GR_SIZE width, height, base;

	GrSetGCForegroundPixelVal(root_gc, AV3XX_COLOR_BLACK);
	GrSetGCBackgroundPixelVal(root_gc, AV3XX_COLOR_WHITE);
	GrClearWindow(root_wid, 0);

	GrGetGCTextSize(root_gc, header, -1, GR_TFASCII, &width, &height,
			&base);

	GrText(root_wid, root_gc, (screen_info.cols - width) / 2, HEADER_BASELINE,
		header, -1, GR_TFASCII);
	GrLine(root_wid, root_gc, 0, HEADER_TOPLINE, screen_info.cols, HEADER_TOPLINE);

	draw_batt_status();
	draw_date();
}

GR_WINDOW_ID pz_new_window(int x, int y, int w, int h, void(*do_draw), int(*do_keystroke)(GR_EVENT * event))
{
	GR_WINDOW_ID new_wid = GrNewWindowEx(GR_WM_PROPS_APPFRAME |
			    GR_WM_PROPS_CAPTION |
			    GR_WM_PROPS_CLOSEBOX,
			    "LinWin",
			    root_wid,
			    x, y, w, h, BLACK);

	/* FIXME: assumes always ok */

	windows[n_opened].wid = new_wid;
	windows[n_opened].draw = do_draw;
	windows[n_opened].keystroke = do_keystroke;
	n_opened++;

	return new_wid;
}

void
pz_close_window(GR_WINDOW_ID wid)
{
	//assert(windows[n_opened-1].wid == wid);

	GrUnmapWindow(wid);
	GrDestroyWindow(wid);

	n_opened--;
}

int
main(int argc, char **argv)
{

	if (GrOpen() < 0) {
		fprintf(stderr, "GrOpen failed");
		exit(1);
	}

	root_gc = GrNewGC();
	GrSetGCUseBackground(root_gc, GR_TRUE);

	GrSetGCForegroundPixelVal(root_gc, AV3XX_COLOR_WHITE);
//	GrSetGCForeground(root_gc, WHITE);
	GrGetScreenInfo(&screen_info);

	root_wid = GrNewWindowEx(GR_WM_PROPS_APPFRAME |
			    GR_WM_PROPS_CAPTION |
			    GR_WM_PROPS_CLOSEBOX,
			    "LinWin",
			    GR_ROOT_WINDOW_ID,
			    0, 0, screen_info.cols, screen_info.rows, BLACK);

	GrSelectEvents(root_wid, GR_EVENT_MASK_EXPOSURE |
		       GR_EVENT_MASK_CLOSE_REQ | GR_EVENT_MASK_KEY_DOWN);

	GrMapWindow(root_wid);

	ipod_load_settings();

	new_menu_window();

	while (1) {
		GR_EVENT event;

		GrGetNextEventTimeout(&event, 1000);
		event_handler(&event);
	}

	return 0;
}
