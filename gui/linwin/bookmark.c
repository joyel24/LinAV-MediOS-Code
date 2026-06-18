/*
 * Copyright (C) 2004 Goetz Minuth
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


static GR_WINDOW_ID book_wid;
static GR_GC_ID book_gc;
static GR_SCREEN_INFO screen_info;
static g_booktype = -1;

GR_SIZE g_width, g_height, g_base;

static draw_bookmarks()
{
   char tmp[30];
   if(g_booktype == BOOKMARK_DIR)
   	GrText(book_wid, book_gc, 5, 50, "Bookmarks Dir", -1, GR_TFASCII);
   else if(g_booktype == BOOKMARK_MP3)
   	GrText(book_wid, book_gc, 5, 50, "Bookmarks Mp3", -1, GR_TFASCII);
   else if(g_booktype == BOOKMARK_PIC)
   	GrText(book_wid, book_gc, 5, 50, "Bookmarks Pic", -1, GR_TFASCII);
   else if(g_booktype == BOOKMARK_DOC)
   	GrText(book_wid, book_gc, 5, 50, "Bookmarks Doc", -1, GR_TFASCII);
   else
	{
	   sprintf(tmp,"Undefined Bookmark from type %d", g_booktype);
   	GrText(book_wid, book_gc, 5, 50, tmp, -1, GR_TFASCII);
	}

  	GrText(book_wid, book_gc, 5, 65, "Bookmarks are not implemeted yet !", -1, GR_TFASCII);
}
static void bookmark_do_draw(GR_EVENT * event)
{
	pz_draw_header("Bookmarks");

   draw_bookmarks();
}

static int bookmark_do_keystroke(GR_EVENT * event)
{
	int ret = 0;

	switch (event->keystroke.ch)
	{

	case '1':

		ret = 1;
		break;

	case '2':

		ret = 1;
		break;

	case '3':

      break;

   case 'r':

		ret = 1;
		break;

	case 'l':
	case 'f':
		pz_close_window(book_wid);
		ret = 1;
		break;

	case 'd':

		break;

	case 'u':

		break;
	}

	return ret;
}

void new_bookmark_window(int type)
{
	GrGetScreenInfo(&screen_info);

	g_booktype = type;

	book_gc = GrNewGC();
	GrSetGCUseBackground(book_gc, GR_TRUE);
	GrSetGCForegroundPixelVal(book_gc, AV3XX_COLOR_BLACK);

	GrGetGCTextSize(book_gc, "M", -1, GR_TFASCII, &g_width, &g_height, &g_base);

	book_wid = pz_new_window(0, HEADER_TOPLINE + 1, screen_info.cols,
                                    screen_info.rows - (HEADER_TOPLINE + 1),
                                    bookmark_do_draw, bookmark_do_keystroke);

	GrSelectEvents(book_wid, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN);

	GrMapWindow(book_wid);

   draw_bookmarks();
}
