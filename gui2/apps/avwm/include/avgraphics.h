/*
* avgraphics.h
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

#ifndef __AVGRAPHICS_H
#define __AVGRAPHICS_H

#include "graphics.h"

void          wmDrawPixel         (unsigned int color, int x, int y);
unsigned int  wmReadPixel         (int x, int y);
void          wmDrawRect          (unsigned int color, int x, int y, int width, int height);
void          wmFillRect          (unsigned int color, int x, int y, int width, int height);
void          wmDrawLine          (unsigned int color, int x1, int y1, int x2, int y2);
void          wmPutS              (unsigned int color, unsigned int bg_color,int x, int y, unsigned char *s);
void          wmPutC              (unsigned int color, unsigned int bg_color,int x, int y, unsigned char s);
void          wmDrawSprite        (PALETTE * palette, SPRITE * sprite, int x, int y);
void          wmDrawBITMAP        (BITMAP * bitmap, int x, int y);
void          wmScrollWindowVert  (unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP);
void          wmScrollWindowHoriz (unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT);
void          wmClearScreen       (unsigned int color);

void          wmSetPlane          (int vplane);
int           wmGetPlane          (void);
void          defaultPlane        (void);

#endif
