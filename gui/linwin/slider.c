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

#include "pz.h"
#include "ipod.h"
#include "av3xx_colordef.h"

static GR_WINDOW_ID slider_wid;
static GR_GC_ID slider_gc;
static GR_SCREEN_INFO screen_info;

struct slider_widget {
	int setting;
	int value;
	int min;
	int max;
	char *title;
};

static struct slider_widget slider;

static void slider_do_draw(void)
{
	int filler;
	char tmp[10];

	filler = ((int)((slider.value * 128) / slider.max));

	GrSetGCForegroundPixelVal(slider_gc, AV3XX_COLOR_BLACK);
	sprintf(tmp,"%d", slider.min);
	GrText(slider_wid, slider_gc, 15, 38, tmp, -1, GR_TFASCII);
	sprintf(tmp,"%d", slider.max);
	GrText(slider_wid, slider_gc, 130, 38, tmp, -1, GR_TFASCII);

	GrSetGCForegroundPixelVal(slider_gc, AV3XX_COLOR_RED);
	GrRect(slider_wid, slider_gc, 15, 40, 132, 10);

	GrSetGCForegroundPixelVal(slider_gc, AV3XX_COLOR_BLUE);
	GrFillRect(slider_wid, slider_gc, 16, 41, 130, 8);

	GrSetGCForegroundPixelVal(slider_gc, AV3XX_COLOR_BLACK);
	GrFillRect(slider_wid, slider_gc, 16 , 41, filler, 8);

	GrSetGCForegroundPixelVal(slider_gc, AV3XX_COLOR_WHITE);
	GrFillRect(slider_wid, slider_gc, 10 , 51, 145, 11);

	GrSetGCForegroundPixelVal(slider_gc, AV3XX_COLOR_BLACK);
	sprintf(tmp,"%d", slider.value);
	GrText(slider_wid, slider_gc, filler+5, 62, tmp, -1, GR_TFASCII);

	GrSetGCForegroundPixelVal(slider_gc, AV3XX_COLOR_WHITE);
}

static int slider_do_keystroke(GR_EVENT * event)
{
	int ret = 0;
	int step = 0;

	switch (event->keystroke.ch) {
   case 'l':
		pz_close_window(slider_wid);
		ret = 1;
		break;

	case 'd':

      step = (slider.max-slider.min)/20;
		if(step == 0) step = 1;

		if (slider.value-step >= slider.min)
		{
			slider.value-=step;
//			slider.value--;
      }
		else
		{
		   slider.value = slider.min;
		}

		ipod_set_setting(slider.setting, slider.value);
		slider_do_draw();
		ret = 1;

		break;

	case 'u':

      step = (slider.max-slider.min)/20;
		if(step == 0) step = 1;
		
		if (slider.value < slider.max - step)
		{
			slider.value+=step;
		}
		else
		{
		   slider.value = slider.max;
		}

		ipod_set_setting(slider.setting, slider.value);
		slider_do_draw();
		ret = 1;
		break;

	}
	return ret;
}

void new_slider_widget(int SETTING, char *title, int slider_min, int slider_max)
{
	char tmp[10];

	slider.setting = SETTING;
	slider.value = ipod_get_setting(slider.setting);
	slider.min = slider_min;
	slider.max = slider_max;
	slider.title = title;

	GrGetScreenInfo(&screen_info);

	slider_gc = GrNewGC();
	GrSetGCUseBackground(slider_gc, GR_TRUE);
	GrSetGCForegroundPixelVal(slider_gc, AV3XX_COLOR_WHITE);

	slider_wid = pz_new_window(0, HEADER_TOPLINE + 1, screen_info.cols, screen_info.rows - (HEADER_TOPLINE + 1),
		slider_do_draw, slider_do_keystroke);

	GrSelectEvents(slider_wid, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN);

	pz_draw_header(slider.title);

	GrMapWindow(slider_wid);
}
