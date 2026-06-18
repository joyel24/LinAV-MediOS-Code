/*
 * Copyright (c) 2003 Century Software, Inc.   All Rights Reserved.     
 *                                                                       
 * This file is part of the PIXIL Operating Environment                 
 *                                                                       
 * The use, copying and distribution of this file is governed by one    
 * of two licenses, the PIXIL Commercial License, or the GNU General    
 * Public License, version 2.                                           
 *                                                                       
 * Licensees holding a valid PIXIL Commercial License may use this file 
 * in accordance with the PIXIL Commercial License Agreement provided   
 * with the Software. Others are governed under the terms of the GNU   
 * General Public License version 2.                                    
 *                                                                       
 * This file may be distributed and/or modified under the terms of the  
 * GNU General Public License version 2 as published by the Free        
 * Software Foundation and appearing in the file LICENSE.GPL included   
 * in the packaging of this file.                                      
 *                                                                       
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING  
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A            
 * PARTICULAR PURPOSE.                                                  
 *                                                                       
 * RESTRICTED RIGHTS LEGEND                                             
 *                                                                     
 * Use, duplication, or disclosure by the government is subject to      
 * restriction as set forth in paragraph (b)(3)(b) of the Rights in     
 * Technical Data and Computer Software clause in DAR 7-104.9(a).       
 *                                                                      
 * See http://www.pixil.org/gpl/ for GPL licensing       
 * information.                                                         
 *                                                                      
 * See http://www.pixil.org/license.html or              
 * email cetsales@centurysoftware.com for information about the PIXIL   
 * Commercial License Agreement, or if any conditions of this licensing 
 * are not clear to you.                                                
 */



#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
//#include "vertex.h"
#include "nxclock.h"
#define MWINCLULDECOLORS
#include <nano-X.h>
//#include <wm/nxlib.h>
#include "av3xx_colordef.h"
#include "pz.h"


#define DEF_STYLE	(GR_WM_PROPS_APPWINDOW |\
			 GR_WM_PROPS_NOAUTORESIZE |\
			 GR_WM_PROPS_NOAUTOMOVE)
#define DEF_GEOMETRY	"100x100-0+0"
#define DEF_TITLE	"Clock"

/*
// application initialization data
static nxARGS args[] = {
    nxTITLE(DEF_TITLE),
    nxGEOMETRY(DEF_GEOMETRY),
    nxBACKGROUND(GR_COLOR_WINDOW),
    nxSTYLE(DEF_STYLE),
    nxEND
};
*/
GR_SIZE width;
GR_SIZE height;

static GR_WINDOW_ID clock_wid;
static GR_GC_ID clock_gc;
static GR_SCREEN_INFO screen_info;

static struct clock_time newtime = { 0, 0, 0 };

const float hourhand[4][2] = { {-0.5f, 0}, {0, 1.5f}, {0.5f, 0}, {0, -7.0f} };
const float minhand[4][2] = { {-0.5f, 0}, {0, 1.5f}, {0.5f, 0}, {0, -11.5f} };
const float sechand[4][2] = { {-0.1f, 0}, {0, 2.0f}, {0.1f, 0}, {0, -11.5f} };

static void
drawhand(double ang, const float v[][2], GR_COLOR fill,
	 GR_COLOR line, GR_WINDOW_ID pmap, GR_GC_ID gc)
{
    int i;

    push_matrix();
    rotate(ang);

    GrSetGCForeground(clock_gc, fill);
    begin_polygon();
    for (i = 0; i < 4; i++)
	vertex(v[i][0], v[i][1]);
    end_polygon(pmap, clock_gc);
    GrSetGCForeground(clock_gc, line);
    begin_loop();
    for (i = 0; i < 4; i++)
	vertex(v[i][0], v[i][1]);
    end_loop(pmap, clock_gc);
    pop_matrix();
}

static uchar
type()
{
    return ROUND_CLOCK;
}

void
draw_clock_hands(GR_COLOR fill, GR_COLOR line, GR_WINDOW_ID pmap, GR_GC_ID gc)
{

    drawhand(-360 * (newtime.hour_ + newtime.minute_ / 60.0) / 12, hourhand,
	     fill, line, pmap, clock_gc);
    drawhand(-360 * (newtime.minute_ + newtime.second_ / 60.0) / 60, minhand,
	     fill, line, pmap, clock_gc);
    drawhand(-360 * (newtime.second_ / 60.0), sechand, fill, line, pmap, clock_gc);
}


static void
rect(double x, double y, double w, double h, GR_WINDOW_ID pmap, GR_GC_ID gc)
{
    double r = x + w;
    double t = y + h;
    begin_polygon();
    vertex(x, y);
    vertex(r, y);
    vertex(r, t);
    vertex(x, t);
    end_polygon(pmap, clock_gc);
}

void
reset_values(int h, int m, int s)
{
    if (h != newtime.hour_ || m != newtime.minute_ || s != newtime.second_) {
	newtime.hour_ = h;
	newtime.minute_ = m;
	newtime.second_ = s;
    }
}

static void
tick()
{
    struct timeval t;
    struct tm *timeofday;

    gettimeofday(&t, 0);
    timeofday = localtime((const time_t *) &t.tv_sec);
    reset_values(timeofday->tm_hour, timeofday->tm_min, timeofday->tm_sec);
}

void
draw_clock(int x, int y, int w, int h, GR_WINDOW_ID pmap, GR_GC_ID gc,
	   GR_WINDOW_ID window)
{
    int i;

    GrSetGCForeground(clock_gc, GrGetSysColor(GR_COLOR_WINDOW));
    GrFillRect(pmap, clock_gc, 0, 0, w, h);

    tick();
    push_matrix();
    translate(x + w / 2.0 - .5, y + h / 2.0 - .5);
    scale_xy((w - 1) / 28.0, (h - 1) / 28.0);
    if (type() == ROUND_CLOCK) {
	GrSetGCForeground(clock_gc, BLACK);
	begin_polygon();
	circle(0, 0, 14, pmap, clock_gc, w, h);
	end_polygon(pmap, clock_gc);
	GrSetGCForeground(clock_gc, BLACK);
	begin_loop();
	circle(0, 0, 14, pmap, clock_gc, w, h);
	end_loop(pmap, clock_gc);
    }
    //draw the shadows
    push_matrix();
    translate(0.60, 0.60);
    draw_clock_hands(LTGRAY, LTGRAY, pmap, clock_gc);
    pop_matrix();
    //draw the tick marks
    push_matrix();
    GrSetGCForeground(clock_gc, BLACK);
    for (i = 0; i < 12; i++) {
	if (6 == i)
	    rect(-0.5, 9, 1, 2, pmap, clock_gc);
	else if (3 == i || 0 == i || 9 == i)
	    rect(-0.5, 9.5, 1, 1, pmap, clock_gc);
	else
	    rect(-0.25, 9.5, .5, 1, pmap, clock_gc);
	rotate(-30);
    }
    pop_matrix();
    //draw the hands
    draw_clock_hands(GRAY, BLACK, pmap, clock_gc);
    pop_matrix();
    GrCopyArea(window, clock_gc, 0, 0, w, h, pmap, 0, 0, MWROP_SRCCOPY);
}

GR_WINDOW_ID
resize(GR_SIZE w, GR_SIZE h, GR_WINDOW_ID pmap)
{
    width = w;
    height = h;
    if (pmap)
	GrDestroyWindow(pmap);
    return GrNewPixmap(w, h, NULL);
}

static int clock_do_keystroke(GR_EVENT * event)
{
	int ret = 0;

	switch (event->keystroke.ch)
	{
		case 'f':
			pz_close_window(clock_wid);
			ret = 1;
			break;
	}

	return ret;
}

static void clock_do_draw()
{
    draw_clock(0, 0, width, height, 0/*pmap*/, clock_gc, clock_wid);
}

void new_clock_window()
{
	GrGetScreenInfo(&screen_info);

	clock_gc = GrNewGC();
	GrSetGCUseBackground(clock_gc, GR_TRUE);
  	GrSetGCForegroundPixelVal(clock_gc, AV3XX_COLOR_WHITE);

	clock_wid = pz_new_window(0, HEADER_TOPLINE + 1, screen_info.cols, screen_info.rows - (HEADER_TOPLINE + 1), clock_do_draw, clock_do_keystroke);

	GrSelectEvents(clock_wid, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN);

	GrMapWindow(clock_wid);
}
