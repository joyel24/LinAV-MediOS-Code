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
#include <assert.h>

#include "pz.h"
#include "ipod.h"
#include "piezo.h"
#include "av3xx_colordef.h"

extern void new_contrast_window(void);
extern void new_browser_window(void);
extern void toggle_backlight(void);
extern void set_wheeldebounce(void);
extern void set_buttondebounce(void);
extern void set_volume(void);
extern void toggle_mp3_repeat(void);
extern void new_record_window(void);
extern void new_oth_window(void);
extern void new_bluecube_window(void);
extern void new_calendar_window(void);
extern void set_keyrepeat(void);
extern void set_keyfreq(void);
//extern void new_clock_window(void);

static GR_WINDOW_ID menu_wid;
static GR_GC_ID menu_gc;
static GR_SCREEN_INFO screen_info;

extern void quit_podzilla(void);
extern void reboot_ipod(void);

extern int yPosDebug;

#define MAX_MENU_ITEMS 10

struct menu_item {
	char *text;
	int type;
	void *ptr;
	int settingtype; // for checkmarks
};


#define DISPLAY_MENU	0
#define SUB_MENU_HEADER	1
#define ACTION_MENU	2
#define VALUE_MENU	3
#define SUB_MENU_PREV   4
#define ACTION_MENU_CHECK 5

typedef void (*menu_action_t) (void);

#if 0
static struct menu_item browse_menu[] = {
	{"Artists", DISPLAY_MENU, 0, -1},
	{"Albums", DISPLAY_MENU, 0, -1},
	{"Songs", DISPLAY_MENU, 0, -1},
	{"Genres", DISPLAY_MENU, 0, -1},
	{"Composers", DISPLAY_MENU, 0, -1},
	{0, 0, 0, -1}
};
#endif

static struct menu_item games_menu[] = {
	{"Othello", ACTION_MENU, new_oth_window, -1},
	{0, 0, 0, -1}
};

static struct menu_item extras_menu[] = {
#if 0
	{"Voice Record", ACTION_MENU, new_record_window, -1},
	{"Contacts", SUB_MENU_HEADER, 0, -1},
	{"Notes", SUB_MENU_HEADER, 0, -1},
#endif
//	{"Clock", SUB_MENU_HEADER, new_clock_window, -1},
	{"Calendar", ACTION_MENU, new_calendar_window, -1},
	{"Games", SUB_MENU_HEADER, games_menu, -1},
	{0, 0, 0, -1}
};

static struct menu_item reset_menu[] = {
	{"Cancel", SUB_MENU_PREV, 0, -1},
	{"Absolutely", ACTION_MENU, ipod_reset_settings, -1},
	{0, 0, 0, -1}
};

static struct menu_item settings_menu[] = {
#if 0
	{"About", SUB_MENU_HEADER, 0, -1},
	{"Shuffle", VALUE_MENU, 0, -1},
	{"EQ -Off", VALUE_MENU, 0, -1},
#endif
#if 0
	{"Backlight Timer", ACTION_MENU, set_backlight_timer, -1},
	{"Wheel Sensitivity", ACTION_MENU, set_wheeldebounce, -1},
#endif
//	{"Backlight", ACTION_MENU_CHECK, toggle_backlight, -1},
	{"Volume", ACTION_MENU, set_volume, -1},
	{"Repeat", ACTION_MENU_CHECK, toggle_mp3_repeat, REPEAT},
	{"Contrast", ACTION_MENU, set_contrast, -1},
//	{"Button Debounce", ACTION_MENU, set_buttondebounce, -1},
	{"Key Freq", ACTION_MENU, set_keyfreq, -1},
	{"Key Repeat", ACTION_MENU, set_keyrepeat, -1},
#if 0
	{"Alarms", SUB_MENU_HEADER, 0, -1},
	{"Contacts", SUB_MENU_HEADER, 0, -1},
#endif
//	{"Clicker", ACTION_MENU, toggle_piezo, -1},  // gibt wahrscheinlich Töne aus bei Tastendrücken on/off
#if 0
	{"Language", SUB_MENU_HEADER, 0, -1},
	{"Legal", SUB_MENU_HEADER, 0, -1},
#endif
	{"Reset All Settings", SUB_MENU_HEADER, reset_menu, -1},
	{"Save Settings", ACTION_MENU, ipod_save_settings, -1},
	{"Load Settings", ACTION_MENU, ipod_load_settings, -1},
	{0, 0, 0, -1}
};

static struct menu_item reboot_menu[] = {
	{"Cancel", SUB_MENU_PREV, 0, -1},
	{"Absolutely", ACTION_MENU, reboot_ipod, -1},
	{0, 0, 0, -1}
};

static struct menu_item main_menu[] = {
#if 0
	{"Playlists", SUB_MENU_HEADER, 0, -1},
	{"Browse", SUB_MENU_HEADER, browse_menu, -1},
#endif
	{"Extras", SUB_MENU_HEADER, extras_menu, -1},
	{"Settings", SUB_MENU_HEADER, settings_menu, -1},
	{"File Browser", ACTION_MENU, new_browser_window, -1},
   {"Quit", ACTION_MENU, quit_podzilla, -1},
//        {"Reboot LinWin", SUB_MENU_HEADER, reboot_menu, -1},
	{0, 0, 0, -1}
};

static int current_menu_item = 0;
static int top_menu_item = 0;
static int in_contrast = 0;

static struct menu_item *menu = main_menu;
static struct menu_item *menu_stack[5];
static int menu_item_stack[5];
static int top_menu_item_stack[5];
static int menu_stack_pos = 0;

static void draw_menu()
{
   char tmp[10];
	int i;
	int value = -1;
	GR_SIZE width, height, base;
	struct menu_item *m = &menu[top_menu_item];

	GrGetGCTextSize(menu_gc, "M", -1, GR_TFASCII, &width, &height, &base);
	height += 5;

	i = 0;
	while (i <= MAX_MENU_ITEMS)
	{
		GrSetGCForegroundPixelVal(menu_gc, AV3XX_COLOR_WHITE);
		GrFillRect(menu_wid, menu_gc, 0,
				1 + i * height,
				13, height);

		if(m->type == ACTION_MENU_CHECK) // draw checkmark
		{
        	GrSetGCForegroundPixelVal(menu_gc, AV3XX_COLOR_BLACK);
			GrRect(menu_wid, menu_gc, 3,
				   1 + i * height+4,
				   10, height-8);

			if(m->settingtype == REPEAT)
			   value = ipod_get_setting(REPEAT);
         // ToDo: Read settings for other checkmark settings

			if(value == 0)
			{
			   // do nothing
			}
			else if(value == 1)
			{
	         // draw checkmark
          	GrSetGCForegroundPixelVal(menu_gc, AV3XX_COLOR_RED);
				GrFillRect(menu_wid, menu_gc, 4,
						1 + i * height+5,
						8, height-10);
          	GrSetGCForegroundPixelVal(menu_gc, AV3XX_COLOR_BLACK);
			}

		}

		if (i + top_menu_item == current_menu_item) // marked row
		{
        	GrSetGCForegroundPixelVal(menu_gc, AV3XX_COLOR_BLACK);
			GrFillRect(menu_wid, menu_gc, 14,
				   1 + i * height,
				   screen_info.cols, height);
       	GrSetGCForegroundPixelVal(menu_gc, AV3XX_COLOR_WHITE);
			GrSetGCUseBackground(menu_gc, GR_FALSE);
		}
		else // all other rows
		{
			GrSetGCUseBackground(menu_gc, GR_TRUE);
			GrSetGCMode(menu_gc, GR_MODE_SET);
       	GrSetGCForegroundPixelVal(menu_gc, AV3XX_COLOR_WHITE);
			GrFillRect(menu_wid, menu_gc, 14,
				   1 + i * height,
				   screen_info.cols, height);
       	GrSetGCForegroundPixelVal(menu_gc, AV3XX_COLOR_BLACK);
		}

		if (m->text != 0)
		{
			GrText(menu_wid, menu_gc, 15/*8*/, 1 + (i + 1) * height - 4, m->text, -1, GR_TFASCII);
			m++;
		}

		i++;

		if (i == MAX_MENU_ITEMS)
			break;
	}

	GrSetGCMode(menu_gc, GR_MODE_SET);
}

static void menu_do_draw()
{
   char tmp[20];

	pz_draw_header("LinWin V0.17");
/*
	GrSetGCForegroundPixelVal(menu_gc, AV3XX_COLOR_BLACK);
   sprintf(tmp,"freq: %d repeat: %d", ipod_get_mouseParam_freq(), ipod_get_mouseParam_repeat());
   GrText(menu_wid, menu_gc, 8, 220, tmp, -1, GR_TFASCII);
	GrSetGCForegroundPixelVal(menu_gc, AV3XX_COLOR_WHITE);
*/
	draw_menu();
}

static int menu_do_keystroke(GR_EVENT * event)
{
   GR_EVENT nextevent;
	static int rcount = 0;
	static int lcount = 0;
	int ret = 0;
	int freq = 0;
	int repeat = 0;

	switch (event->keystroke.ch) {
	case '1':
	case 'r':

		ret = 1;
		switch (menu[current_menu_item].type) {
		case SUB_MENU_HEADER:
			if (menu[current_menu_item].ptr != 0)
			{
				menu_stack[menu_stack_pos] = menu;
				menu_item_stack[menu_stack_pos] =
				    current_menu_item;
				top_menu_item_stack[menu_stack_pos++] =
				    top_menu_item;
/*
      sprintf(tmp,"%s Cur:%d StackPos:%d",menu[current_menu_item].text,current_menu_item,menu_stack_pos);
		GrText(menu_wid, menu_gc, 8, yPosDebug, tmp, -1, GR_TFASCII);
		yPosDebug+=10;
*/
				pz_draw_header(menu[current_menu_item].text);
				menu = (struct menu_item *)menu[current_menu_item].ptr;
				current_menu_item = 0;
				top_menu_item = 0;
				draw_menu();
			}
			break;

		case ACTION_MENU:
			if (menu[current_menu_item].ptr != 0) {
				((menu_action_t) menu[current_menu_item].ptr) ();
			}
			break;

		case ACTION_MENU_CHECK:

			if (menu[current_menu_item].ptr != 0)
			{
				((menu_action_t) menu[current_menu_item].ptr) ();
				draw_menu();
			}
			break;

		case SUB_MENU_PREV:
			event->keystroke.ch = 'l';
			menu_do_keystroke(event);
			break;
		}

		break;

   case 'l': // left
/*
	   yPosDebug2 = 110;
	   for(i = 0; i < 5;i++)
		{
			sprintf(tmp,"%s StackPos:%d",menu[i].text,i);
			GrText(menu_wid, menu_gc, 140, yPosDebug2, tmp, -1, GR_TFASCII);
			yPosDebug2+=15;
		}
*/
		ret = 1;
		if (menu_stack_pos > 0) {
			menu = menu_stack[--menu_stack_pos];
			current_menu_item = menu_item_stack[menu_stack_pos];
			top_menu_item = top_menu_item_stack[menu_stack_pos];
/*
      sprintf(tmp,"%s Cur:%d StackPos:%d",menu[current_menu_item].text,current_menu_item,menu_stack_pos);
		GrText(menu_wid, menu_gc, 8, yPosDebug, tmp, -1, GR_TFASCII);
		yPosDebug+=10;
*/
		   if(menu_stack_pos == 0)
			   menu_do_draw();
         else
			{
				pz_draw_header(menu_stack[menu_stack_pos-1][menu_item_stack[menu_stack_pos-1]].text);
				draw_menu();
			}
   		ret = 1;
		}

		break;

	case 'u': // up
/*#ifdef IPOD
		lcount++;
		if (lcount < 1) {
			break;
		}
		lcount = 0;
#endif*/
		if (current_menu_item) {
			if (current_menu_item == top_menu_item) {
				top_menu_item--;
			}
			current_menu_item--;
			draw_menu();
			ret = 1;
		}
		break;

/*#ifndef IPOD
	case 'f': // quit
		GrClose();
		exit(0);
		break;
#endif*/

	case 'd': // down
/*#ifdef IPOD
		rcount++;
		if (rcount < 1) {
			break;
		}
		rcount = 0;
#endif*/
		if (menu[current_menu_item + 1].text != 0) {
			current_menu_item++;
			if (current_menu_item - MAX_MENU_ITEMS == top_menu_item) {
				top_menu_item++;
			}
			draw_menu();
			ret = 1;
		}
		break;
	}

	return ret;
}

void new_menu_window()
{
	GrGetScreenInfo(&screen_info);

	menu_gc = GrNewGC();
	GrSetGCUseBackground(menu_gc, GR_TRUE);
	GrSetGCForegroundPixelVal(menu_gc, AV3XX_COLOR_WHITE);

	menu_wid = pz_new_window(0, HEADER_TOPLINE + 1, screen_info.cols, screen_info.rows - (HEADER_TOPLINE + 1), menu_do_draw, menu_do_keystroke);

	GrSelectEvents(menu_wid, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN);

	GrMapWindow(menu_wid);
}
