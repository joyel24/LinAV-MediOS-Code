/*
 * Copyright (C) 2004 David Carne
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
/* Changes 2004-06-27 by matz-josh:
	- fixed crash on dos/windows textfiles containing empty lines
	- the handle on the scrollbar is now at least 2 Pixels high
	  it has been sometimes invisible (hight 0) in large files
	- added support for rewind and forward keys:
	  rewind scrolls up and forward scrolls down one screen.
	- removed flicker, when trying to scroll beyond the last(first) line of text
*/

#include "pz.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "av3xx_colordef.h"

#define LINESPERSCREEN 16

static GR_GC_ID tv_gc;
static GR_WINDOW_ID tv_wid;
static GR_WINDOW_INFO tv_winfo;
static GR_SCREEN_INFO screen_info;
static char * g_filename;

static int yPosDebug = 100;

static unsigned int totalLines = 0;
static unsigned int currentLine = 0;
static char ** lineData;
GR_SIZE g_width, g_height, g_base;

static void printPage(int startLine, int x, int y, int w, int h)
{
	int i;

	for (i = startLine; i < startLine + LINESPERSCREEN && i < totalLines; i++)
	{
		GrText(tv_wid, tv_gc, 3, (i - startLine + 1) * g_height + 4, lineData[i], -1, GR_TFASCII);
	}

}

// This buildlines code is a horrible, horrible hack. It may also (more than likely) be (very) broken.
// I also see many ways to optimize it, but I'm really to lazy to at the moment.
static void buildLineData()
{

	char * starttextptr;
	FILE * fp;
	long unsigned int file_len;
	char * curtextptr;
	char ** localAr;
	int allocedlines;
	char * temp;
   char tmp[1000];

	currentLine = 0;
	fp = fopen(g_filename,"r");
	if (!fp)
	{
		printf("cannot open file %s\n", g_filename);
		return;
	}

	sprintf(tmp, "%s opened !",g_filename);
	GrText(tv_wid, tv_gc, 8, yPosDebug,	tmp, -1, GR_TFASCII);
	yPosDebug+=10;

	fseek(fp, 0, SEEK_END);
	file_len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	starttextptr = (char *) malloc (file_len + 1);
	fread(starttextptr, file_len, 1, fp);
	starttextptr[file_len] = 0;

	curtextptr = starttextptr;
	localAr = (char **)malloc(sizeof(char *) * 20);
	allocedlines = 20;

	// While we haven't reached the end of the file
	while (*curtextptr != 0)
	{
		int charcnt = 0;
		char * sol;

		// See if we need to allocate more lines to the line storage
		if (allocedlines < currentLine + 1)
		{
			//fprintf(stderr,"Realloced Mem\n");
			char ** temp = localAr;
			localAr = (char **)malloc(sizeof(char *) * (allocedlines + 20));
			memcpy(localAr, temp, sizeof(char *) * allocedlines);
			free(temp);
			allocedlines += 20;
		}

		localAr[currentLine] = (char *) malloc(100);
		sol = curtextptr;
		while (true)
		{
			GR_SIZE width, height, base;

			if (*curtextptr == '\r')  { //ignore '\r' in dos/windows textfiles
                                curtextptr++;
			}
							
			if (*curtextptr == '\n')
			{
				curtextptr++;
				break;
			} else if (*curtextptr == 0) {
				break;
			}

			GrGetGCTextSize ( tv_gc , sol , curtextptr - sol + 1 , GR_TFASCII, &width , &height, &base );

			if (width > tv_winfo.width - 15)
			{
				// check if we there is a line break in this line
				if (!strchr(localAr[currentLine], ' ') && !strchr(localAr[currentLine], '\t'))
					// Guess not, leave
					break;

				// now backtrack to the last word
				while (!isspace(localAr[currentLine][charcnt]))
				{
					localAr[currentLine][charcnt] = 0;
					curtextptr --;
					charcnt --;
				}
				// skip over the char we broke the line on.
				curtextptr++;
				charcnt++;
				break;
			}

			localAr[currentLine][charcnt++] = *curtextptr++;
		}

		localAr[currentLine][charcnt] = 0;
		while (charcnt && localAr[currentLine][charcnt-1] == '\t') {
			localAr[currentLine][--charcnt] = 0;
		}

		// trim the line down to size
		temp = localAr[currentLine];
		localAr[currentLine] = malloc(strlen(temp)+1);
		strcpy(localAr[currentLine], temp);
		free(temp);

		// fprintf(stderr,"Built Line %s\n", localAr[currentLine]);

		currentLine++;
	}

	totalLines = currentLine;
	currentLine = 0;
	lineData = localAr;
	free(starttextptr);
}

static void viewer_scrollup()
{
   int yBase;
   int yHeight;

	// calculate scrollbar sizes
	float pixPerLine = (tv_winfo.height - 16 - 2) / (float) totalLines;
	int block_start_pix = 9 + pixPerLine * currentLine;
	int block_height = 9 + pixPerLine * (currentLine + LINESPERSCREEN) - block_start_pix;

	GrSetGCForegroundPixelVal(tv_gc, AV3XX_COLOR_BLACK);

	if (block_height < 2)		//for long texts the minimum height of the handle is set to 2
		block_height = 2;

	// Dont get expose events for the moment
	GrSelectEvents(tv_wid, GR_EVENT_MASK_KEY_DOWN);

	GrSetGCForegroundPixelVal(tv_gc, AV3XX_COLOR_WHITE);
	GrFillRect(tv_wid, tv_gc, tv_winfo.width - 8, 7, 8, tv_winfo.height - 8); // empty old scroll bar
	GrSetGCForegroundPixelVal(tv_gc, AV3XX_COLOR_BLACK);

	yBase   = 7 + g_height;
   yHeight = g_height * (LINESPERSCREEN-1);

	// Move the last entries to the bottom
   GrCopyArea(tv_wid,
	           tv_gc,
              0, yBase, tv_winfo.width, yHeight,
	           tv_wid,
				  0, 7,
				  MWROP_SRCCOPY);

	// Add a line at the beginning
	GrSetGCForegroundPixelVal(tv_gc, AV3XX_COLOR_WHITE);
	GrFillRect(tv_wid, tv_gc, 0, 7, tv_winfo.width, g_height); // empty line
	GrSetGCForegroundPixelVal(tv_gc, AV3XX_COLOR_BLACK);
	GrSetGCUseBackground(tv_gc, GR_FALSE);

	GrText(tv_wid, tv_gc, 3, g_height + 4, lineData[currentLine], -1, GR_TFASCII);

	// Draw the ScrollBar track
	GrRect (tv_wid , tv_gc , tv_winfo.width - 6, 8, 2, tv_winfo.height - 16 );

	// Draw the Handle on the ScrollBar
	GrRect (tv_wid , tv_gc , tv_winfo.width - 7, block_start_pix, 4, block_height);
	GrSetGCForegroundPixelVal(tv_gc, AV3XX_COLOR_BLACK);
	GrRect (tv_wid , tv_gc , tv_winfo.width - 6, block_start_pix + 1, 2, block_height - 2);

	GrSelectEvents(tv_wid, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN);
}

static void viewer_scrolldown()
{
   int yBase;
   int yHeight;

	// calculate scrollbar sizes
	float pixPerLine = (tv_winfo.height - 16 - 2) / (float) totalLines;
	int block_start_pix = 9 + pixPerLine * currentLine;
	int block_height = 9 + pixPerLine * (currentLine + LINESPERSCREEN) - block_start_pix;

	GrSetGCForegroundPixelVal(tv_gc, AV3XX_COLOR_BLACK);

	if (block_height < 2)		//for long texts the minimum height of the handle is set to 2
		block_height = 2;

	// Dont get expose events for the moment
	GrSelectEvents(tv_wid, GR_EVENT_MASK_KEY_DOWN);

	GrSetGCForegroundPixelVal(tv_gc, AV3XX_COLOR_WHITE);
	GrFillRect(tv_wid, tv_gc, tv_winfo.width - 8, 7, 8, tv_winfo.height - 8); // empty old scroll bar
	GrSetGCForegroundPixelVal(tv_gc, AV3XX_COLOR_BLACK);

	yBase   = 7 + g_height;
   yHeight = g_height * (LINESPERSCREEN-1);

	// Move the last entries to the bottom
   GrCopyArea(tv_wid,
	           tv_gc,
              0, 7, tv_winfo.width, yHeight,
	           tv_wid,
				  0, yBase,
				  MWROP_SRCCOPY);

	// Add a line at the end
	GrSetGCForegroundPixelVal(tv_gc, AV3XX_COLOR_WHITE);
	GrFillRect(tv_wid, tv_gc, 0, 7+yHeight, tv_winfo.width, g_height); // empty line
	GrSetGCForegroundPixelVal(tv_gc, AV3XX_COLOR_BLACK);
	GrSetGCUseBackground(tv_gc, GR_FALSE);

	GrText(tv_wid, tv_gc, 3, yHeight+g_height+4, lineData[currentLine+LINESPERSCREEN-1], -1, GR_TFASCII);

	// Draw the ScrollBar track
	GrRect (tv_wid , tv_gc , tv_winfo.width - 6, 8, 2, tv_winfo.height - 16 );

	// Draw the Handle on the ScrollBar
	GrRect (tv_wid , tv_gc , tv_winfo.width - 7, block_start_pix, 4, block_height);
	GrSetGCForegroundPixelVal(tv_gc, AV3XX_COLOR_BLACK);
	GrRect (tv_wid , tv_gc , tv_winfo.width - 6, block_start_pix + 1, 2, block_height - 2);

	GrSelectEvents(tv_wid, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN);
}

static void drawtext(void)
{
	// calculate scrollbar sizes
	float pixPerLine = (tv_winfo.height - 16 - 2) / (float) totalLines;
	int block_start_pix = 9 + pixPerLine * currentLine;
	int block_height = 9 + pixPerLine * (currentLine + LINESPERSCREEN) - block_start_pix;

	GrSetGCForegroundPixelVal(tv_gc, AV3XX_COLOR_BLACK);

	if (block_height < 2)		//for long texts the minimum height of the handle is set to 2
		block_height = 2;

	// Clear the Screen
	GrClearWindow(tv_wid, 0);

	// Draw the text
	printPage(currentLine, 0,0,0,0);

	// Draw the ScrollBar track
	GrRect (tv_wid , tv_gc , tv_winfo.width - 6, 8, 2, tv_winfo.height - 16 );

	// Draw the Handle on the ScrollBar
	GrRect (tv_wid , tv_gc , tv_winfo.width - 7, block_start_pix, 4, block_height);
	GrSetGCForegroundPixelVal(tv_gc, AV3XX_COLOR_BLACK);
	GrRect (tv_wid , tv_gc , tv_winfo.width - 6, block_start_pix + 1, 2, block_height - 2);
}

static void textview_do_draw(GR_EVENT * event)
{
	pz_draw_header(g_filename);
	drawtext();
}

static int textview_do_keystroke(GR_EVENT * event){
	int i;
	int ret = 0;

	switch (event->keystroke.ch) {
      case 'f':
			pz_close_window(tv_wid);
			for (i = 0; i < totalLines; i++)
				free(lineData[i]);
			free (lineData);
			break;

		case 'd':
			if (currentLine + LINESPERSCREEN < totalLines) {
				currentLine ++;
            viewer_scrolldown();
//				drawtext();
			}
			ret = 1;
			break;

		case 'u':

			if (currentLine > 0) {
				currentLine --;
            viewer_scrollup();
//				drawtext();
			}
			ret = 1;
			break;

      case '2':
                        // forward key pressed:  go down one screen
			if (currentLine + LINESPERSCREEN < totalLines)
			{
	         currentLine = currentLine + LINESPERSCREEN;

				if (currentLine + LINESPERSCREEN > totalLines)  // if we went down beyond the end of the text
					currentLine = totalLines - LINESPERSCREEN; // we go to the end of the text

		      drawtext();
			}
			ret = 1;
			break;

      case '1':
			// rewind key pressed:  go up one screen
                        // if it's already the first line, nothing is done
			if (currentLine > 0)
			{
				if (currentLine < LINESPERSCREEN) // if there is no full screen above the current one
					currentLine = 0;	  // go up to the very first line
				else	// go up on screen
					currentLine = currentLine - LINESPERSCREEN;

				drawtext();
			}
			ret = 1;
			break;
	}

	return ret;
}

bool is_text_type(char * extension)
{
   bool retVal = false;

   if(strcmp(extension, ".txt") == 0)
	   retVal = true;
   else if(strcmp(extension, ".cfg") == 0)
	   retVal = true;
   else if(strcmp(extension, ".c") == 0)
	   retVal = true;
   else if(strcmp(extension, ".cpp") == 0)
	   retVal = true;
   else if(strcmp(extension, ".h") == 0)
	   retVal = true;
   else if(strcmp(extension, ".ini") == 0)
	   retVal = true;
   else if(strcmp(extension, ".csv") == 0)
	   retVal = true;

	return retVal;
}

void new_textview_window(char * filename)
{
	g_filename = (char *)strdup(filename);

	GrGetScreenInfo(&screen_info);

	tv_gc = GrNewGC();
	GrSetGCUseBackground(tv_gc, GR_TRUE);
	GrSetGCForegroundPixelVal(tv_gc, AV3XX_COLOR_WHITE);
//	GrSetGCForeground(tv_gc, WHITE);

	tv_wid = pz_new_window(0, HEADER_TOPLINE + 1, screen_info.cols, screen_info.rows - (HEADER_TOPLINE + 1), textview_do_draw, textview_do_keystroke);

	GrSelectEvents(tv_wid, GR_EVENT_MASK_EXPOSURE|GR_EVENT_MASK_KEY_DOWN);

	GrGetGCTextSize(tv_gc, "M", -1, GR_TFASCII, &g_width, &g_height, &g_base);

	GrMapWindow(tv_wid);
	GrGetWindowInfo( tv_wid, &tv_winfo);

	buildLineData();
}
