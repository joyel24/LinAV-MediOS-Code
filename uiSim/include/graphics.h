/*
* graphics.h
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

#ifndef __GRAPHICS_H
#define __GRAPHICS_H

// general constants

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240


/* general functions */

int   ini_graphics   (void);
void  close_graphics (void);


/* drawings */

void drawPixel        (int color, int x, int y);
int  readPixel        (int x, int y);
void drawRect         (int color, int x, int y, int width, int height);
void fillRect         (int color, int x, int y, int width, int height);
void drawLine         (int color, int x1, int y1, int x2, int y2);
void putS             (int color, int bg_color,int x, int y, char *s);
void putC             (int color, int bg_color,int x, int y, char s);
int  getStringS       (const unsigned char *str, int *w, int *h);
void clearScreen      (int color);


#endif
