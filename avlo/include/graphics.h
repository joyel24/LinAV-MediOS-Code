/*
*   include/graphics.h
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <graphics_struct.h>

#define getOffset(x,y,buffer,type) ((type*)(x*((buffer->bitsPerPixel)>>3)+y*buffer->width*((buffer->bitsPerPixel)>>3)+buffer->offset))

struct graphicsFont {
    char **  table;
    int      dx;
    int      dy;
    int      width;
    int      height;
    int      bpline;
    int      num;
};

typedef struct graphicsFont *     FONT_ID;


struct graphics_operations {
void          (*drawPixel)        (unsigned int color, int x, int y, struct graphicsBuffer * buff);
unsigned int  (*readPixel)        (int x, int y, struct graphicsBuffer * buff);
void          (*drawRect)         (unsigned int color, int x, int y, int width, int height, struct graphicsBuffer * buff);
void          (*fillRect)         (unsigned int color, int x, int y, int width, int height, struct graphicsBuffer * buff);
void          (*drawChar)         (struct graphicsFont * font, unsigned int color,unsigned int bg_color, int x, int y, unsigned char c, struct graphicsBuffer * buff);
void          (*drawSprite)       (unsigned int * palette, SPRITE * sprite, unsigned int trsp, int x, int y, struct graphicsBuffer * buff);
void          (*drawBITMAP)       (BITMAP * bitmap, unsigned int trsp, int x, int y, struct graphicsBuffer * buff);
void          (*drawString)       (struct graphicsFont * font, unsigned int color,unsigned int bg_color, int x, int y, unsigned char * s, struct graphicsBuffer * buff);
void          (*scrollWindowVert) (unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP, struct graphicsBuffer * buff);
void          (*scrollWindowHoriz)(unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT, struct graphicsBuffer * buff);
void          (*drawHLine)        (unsigned int color, int x, int y, int width, struct graphicsBuffer * buff);
void          (*drawVLine)        (unsigned int color, int x, int y, int height, struct graphicsBuffer * buff);
};

/* general functions */
void  ini_graphics        (unsigned int vid1_addrees);
void  iniComponent        (int vplane,struct graphicsBuffer * buff,unsigned int offset);

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
