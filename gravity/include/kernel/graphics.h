/* 
*   include/graphics.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <kernel/graphics_struct.h>

/* general functions */

void  open_graphics       (void);
void  close_graphics      (void);

/* drawings */

void         clearScreen      (unsigned int color);
void         drawPixel        (unsigned int color, int x, int y);
unsigned int readPixel        (int x, int y);
void         drawRect         (unsigned int color, int x, int y, int width, int height);
void         fillRect         (unsigned int color, int x, int y, int width, int height);
void         drawLine         (unsigned int color, int x1, int y1, int x2, int y2);
void         putS             (unsigned int color, unsigned int bg_color,int x, int y, unsigned char *s);
void         putC             (unsigned int color, unsigned int bg_color,int x, int y, unsigned char s);
void         getStringS       (unsigned char *str, int *w, int *h);
void         drawSprite       (unsigned int * palette, SPRITE * sprite, int x, int y);
void         drawBITMAP       (BITMAP * bitmap, int x, int y);
void         scrollWindowVert (unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP);
void         scrollWindowHoriz(unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT);

/* Planes */
void  setPlane        (int vplane);
int   getPlane        (void);
void  hidePlane       (int vplane);
void  showPlane       (int vplane);
int   isShown         (int vplane);
void  setState        (int vplane,int state);
int   getState        (int vplane);
void  setSize         (int vplane,int width,int height,int bitsPerPixel);
void  getSize         (int vplane,int * width,int * height,int * bitsPerPixel);
void  setPos          (int vplane,int x,int y);
void  getPos          (int vplane,int * x,int * y);

/* font */
void setFont          (int font_nb);
int  getFont          (void);

#endif
