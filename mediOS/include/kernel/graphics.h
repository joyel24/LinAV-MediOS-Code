/*
*   include/kernel/graphics.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <sys_def/graphics.h>

#define getOffset(x,y,buffer,type) ((type*)(x*((buffer->bitsPerPixel)>>3)+y*buffer->width*((buffer->bitsPerPixel)>>3)+buffer->offset))

struct graphicsBuffer {
    unsigned int  offset;                 // ->The data
    int           enable;
    int           component;
    int           width;
    int           real_width;
    int           height;
    int           x;
    int           y;
    int           bitsPerPixel;           // eg 32 = YCbCr color
    int           state;
    
    struct graphics_operations * gops;
};

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

#include <sys_def/graphics.h>

/* general functions */
void  gfx_init                 (void);
void  gfx_initComponent        (int vplane,struct graphicsBuffer * buff,unsigned int offset);
void  gfx_restoreComponent     (int vplane,struct graphicsBuffer * buff);
void  gfx_restoreAllComponents (void);
void  gfx_openGraphics         (void);
void  gfx_closeGraphics        (void);
void  gfx_setPlane             (int vplane);
int   gfx_getPlane             (void);
void  gfx_planeHide            (int vplane);
void  gfx_planeShow            (int vplane);
int   gfx_planeIsShown         (int vplane);
void  gfx_planeSetState        (int vplane,int state);
int   gfx_planeGetState        (int vplane);
void  gfx_planeSetSize         (int vplane,int width,int height,int bitsPerPixel);
void  gfx_planeGetSize         (int vplane,int * width,int * height,int * bitsPerPixel);
void  gfx_planeSetPos          (int vplane,int x,int y);
void  gfx_planeGetPos          (int vplane,int * x,int * y);
void* gfx_planeGetBufferOffset (int vplane);
void  gfx_planeSetBufferOffset (int vplane, void * offset);
void  gfx_clearScreen          (unsigned int color);
void  gfx_drawPixel            (unsigned int color,int x, int y);
unsigned int gfx_readPixel     (int x, int y);
void  gfx_drawRect             (unsigned int color, int x, int y, int width, int height);
void  gfx_fillRect             (unsigned int color, int x, int y, int width, int height);
void  gfx_drawLine             (unsigned int color, int x1, int y1, int x2, int y2);
void  gfx_putS                 (unsigned int color, unsigned int bg_color, int x, int y, unsigned char *s);
void  gfx_getStringSize        (unsigned char *str, int *w, int *h);
void  gfx_putC                 (unsigned int color, unsigned int bg_color, int x, int y, unsigned char s);
void  gfx_drawSprite           (unsigned int * palette, SPRITE * sprite, int x, int y);
void  gfx_drawBitmap           (BITMAP * bitmap, int x, int y);
void  gfx_scrollWindowVert     (unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP);
void  gfx_scrollWindowHoriz    (unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT);
void  gfx_setPalletteRGB       (int r, int g, int b, int index);
void  gfx_fontSet              (int font_nb);
int   gfx_fontGet              (void);

/* Error scr */
#ifdef HAVE_DEBUG_ON_SCREEN
void gfx_dbgscrClear(int color);
void gfx_dbgscrPutS(FONT_ID font,unsigned int color, unsigned int bg_color, int x, int y, unsigned char *s);
void gfx_dbgscrPutC(FONT_ID font,unsigned int color, unsigned int bg_color, int x, int y, unsigned char c);
void gfx_dbgscrScroll(unsigned int bg_color,int x,int y,int height,int UP);
void gfx_dbgscrSwitch(void);
#endif

#endif
