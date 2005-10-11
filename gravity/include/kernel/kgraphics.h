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
#include <api.h>
#include <kernel/threads.h>

#define getOffset(x,y,buffer,type) ((type*)(x*((buffer->bitsPerPixel)>>3)+y*buffer->width*((buffer->bitsPerPixel)>>3)+buffer->offset))

#define getCtxOffset(x,y,ctx) ((COLOR*)pCtx->pixels + x + y * (pCtx->delta >> 2))

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

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

#ifdef DPLANE
//int gfx_swi_handler(int cmd,GFX_DATA * gfxD, void * pvData);
#else

/* 32-bit planes graphic ops, to use directly... */
void graphics32_DrawPixel (COLOR color, int x, int y, GFX_CONTEXT* pCtx);
COLOR graphics32_ReadPixel (int x, int y, GFX_CONTEXT* pCtx);
void graphics32_DrawRect (COLOR color, int x, int y, int width, int height, GFX_CONTEXT* pCtx);
void graphics32_FillRect (COLOR color, int x, int y, int width, int height, GFX_CONTEXT* pCtx);
void graphics32_DrawLine (COLOR color, int x1, int y1, int x2, int y2, GFX_CONTEXT* pCtx);
void graphics32_DrawBITMAP(BITMAP * bitmap, GFX_POINT* pt, GFX_CONTEXT* pCtx);

int gfx_swi_handler (TASK_INFO* pOwner, int cmd, GFX_CONTEXT* gfxD, void* pvData);
#endif
/* general functions */
void  ini_graphics        (void);
void  iniComponent        (int vplane,struct graphicsBuffer * buff,unsigned int offset);


/* Error scr */
void clear_error_scr(int color);
void putS_error_scr(FONT_ID font,unsigned int color, unsigned int bg_color, int x, int y, unsigned char *s);
void putC_error_scr(FONT_ID font,unsigned int color, unsigned int bg_color, int x, int y, unsigned char c);
void scroll_error_scr(unsigned int bg_color,int x,int y,int height,int UP);
void error_scr_switch(void);
#endif
