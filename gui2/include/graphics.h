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

#define getOffset(x,y,buffer,type) ((type*)(x*((buffer->bitsPerPixel)>>3)+y*buffer->width*((buffer->bitsPerPixel)>>3)+buffer->offset))

#define OUTB(val,addr)           (*(volatile char *)(addr) = (val))
#define OUTW(val,addr)           (*(volatile unsigned short *)(addr) = (val))
#define OUTL(val,addr)           (*(volatile unsigned int *)(addr) = (val))
#define INB(addr)                (*(volatile char *)(addr))
#define INW(addr)                (*(volatile unsigned short *)(addr))
#define INL(addr)                (*(volatile unsigned int *)(addr))

// vplane constant
#define BMAP1   0
#define BMAP2   1
#define VID1    2
#define VID2    3
#define CUR1    4
#define CUR2    5

// general constants
#define USE_GC    -1
#define GC_FONT   NULL

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// sprite buffer type
#define S1bit      0
#define S2bit      1
#define S4bit      2
#define S8bit      3
#define S16bit     4
#define S24bit     5
#define S32bit     6

// some data types
#define newPalette(name)         unsigned int name[]
#define newSpriteData(name)      char name[]
#define PALETTE                  unsigned int

#ifndef AV_SCREEN
#define FORCE_REDRAW    1
#define UPDATE_ONLY     0
#endif

struct spriteBuffer {
	unsigned int  data;
	int           width;
    	int           height;
	int           type;
	int           bpline;
};

typedef struct spriteBuffer       SPRITE;
typedef struct spriteBuffer       BITMAP;

struct graphicsFont {
	char **  table;
	int      dx;
	int      dy;
	int      width;
	int      height;
	int      bpline;
};

typedef struct graphicsFont *     FONT_ID;

struct graphicsBuffer {
    unsigned int  offset;                 // ->The data
    int           component;
    int           width;
    int           height;
    int           x;
    int           y;
    int           bitsPerPixel;           // eg 32 = YCbCr color
    int           state;
};

struct graphics_operations {
void          (*drawPixel)        (unsigned int color, int x, int y, struct graphicsBuffer * buff);
unsigned int  (*readPixel)        (int x, int y, struct graphicsBuffer * buff);
void          (*drawRect)         (unsigned int color, int x, int y, int width, int height, struct graphicsBuffer * buff);
void          (*fillRect)         (unsigned int color, int x, int y, int width, int height, struct graphicsBuffer * buff);
void          (*drawChar)         (struct graphicsFont * font, unsigned int color,unsigned int bg_color, int x, int y, unsigned char c, struct graphicsBuffer * buff);
void          (*drawSprite)       (PALETTE * palette, SPRITE * sprite, unsigned int trsp, int x, int y, struct graphicsBuffer * buff);
void          (*drawBITMAP)       (BITMAP * bitmap, unsigned int trsp, int x, int y, struct graphicsBuffer * buff);
void          (*drawString)       (struct graphicsFont * font, unsigned int color,unsigned int bg_color, int x, int y, unsigned char * s, struct graphicsBuffer * buff);
void          (*scrollWindowVert) (unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP, struct graphicsBuffer * buff);
void          (*scrollWindowHoriz)(unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT, struct graphicsBuffer * buff);
void          (*drawHLine)        (unsigned int color, int x, int y, int width, struct graphicsBuffer * buff);
void          (*drawVLine)        (unsigned int color, int x, int y, int height, struct graphicsBuffer * buff);
};

struct graphics_context {
	struct graphicsBuffer *      buffer;
	struct graphics_operations * gops;
	struct graphicsFont *        font;
	unsigned int                 fg;
	unsigned int                 bg;
	unsigned int                 transparent;            // Color index that shall be transparent    
} ;

typedef struct graphics_context * GC_ID;
 


/* general functions */

int   ini_graphics   (void);
void  close_graphics (void);
#ifdef AV_SCREEN
void  iniComponent   (int vplane,struct graphicsBuffer * buff,unsigned int offset);
#endif
GC_ID createGC       (int vplane);
void  destroyGC      (GC_ID gc);

void setPalette      (int palette[256][3],int size);

#ifndef AV_SCREEN
void lcd_update(int type, int x_ini, int y_ini, int w, int h);
#endif

/* drawings */

void         drawPixel        (unsigned int color, int x, int y);
unsigned int readPixel        (int x, int y);
void         drawRect         (unsigned int color, int x, int y, int width, int height);
void         fillRect         (unsigned int color, int x, int y, int width, int height);
void         drawLine         (unsigned int color, int x1, int y1, int x2, int y2);
void         putS             (unsigned int color, unsigned int bg_color,int x, int y, unsigned char *s);
void         putC             (unsigned int color, unsigned int bg_color,int x, int y, unsigned char s);
void         getStringS       (unsigned char *str, int *w, int *h);
void         drawSprite       (PALETTE * palette, SPRITE * sprite, int x, int y);
void         drawBITMAP       (BITMAP * bitmap, int x, int y);
void         scrollWindowVert (unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP);
void         scrollWindowHoriz(unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT);
void         clearScreen      (unsigned int color);

/* images */
void drawImage        (char * filename);

/* Font */
#define needFont(name)	 FONT_ID name;
void setFont(FONT_ID font);
FONT_ID getFont(void);

/* Planes */
void  setPlane        (int vplane);
void  hidePlane       (int vplane);
void  showPlane       (int vplane);
void  setSate         (int vplane,int state);
int   getSate         (int vplane);
void  setSize         (int vplane,int width,int height,int bitsPerPixel);
void  getSize         (int vplane,int * width,int * height,int * bitsPerPixel);
void  setPos          (int vplane,int x,int y);
void  getPos          (int vplane,int * x,int * y);



#endif
