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

#define getOffset(x,y,buffer,type) ((type*)(x*((buffer->bitsPerPixel)>>3)+y*buffer->width+buffer->offset))

#define OUTB(val,addr)           (*(volatile char *)(addr) = (val))
#define OUTW(val,addr)           (*(volatile unsigned short *)(addr) = (val))
#define OUTL(val,addr)           (*(volatile unsigned int *)(addr) = (val))
#define INB(addr)                (*(volatile char *)(addr))
#define INW(addr)                (*(volatile unsigned short *)(addr))
#define INL(addr)                (*(volatile unsigned int *)(addr))

// vplane constant
#define BMAP1   1
#define BMAP2   2

// general constants
#define USE_GC    -1
#define GC_FONT   NULL

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
    int           bytesPerLine;
    int           width;
    int           height;
    int           x;
    int           y;
    int           bitsPerPixel;           // eg 32 = YCbCr color
    int           bitsPerPixelShift;      // eg  5 = YCbCr color
};

struct graphics_operations {
void (*drawPixel)    (int color, int x, int y, struct graphicsBuffer * buff);
int  (*readPixel)    (int x, int y, struct graphicsBuffer * buff);
void (*drawRect)     (int color, int x, int y, int width, int height, struct graphicsBuffer * buff);
void (*fillRect)     (int color, int x, int y, int width, int height, struct graphicsBuffer * buff);
void (*drawChar)     (struct graphicsFont * font, int color,int bg_color, int x, int y, char c, struct graphicsBuffer * buff);
void (*drawSprite)   (SPRITE * sprite, PALETTE * palette, unsigned int trsp, int x, int y, struct graphicsBuffer * buff);
void (*drawBITMAP)   (BITMAP * bitmap, unsigned int trsp, int x, int y, struct graphicsBuffer * buff);
};

struct graphics_context {
	struct graphicsBuffer *      buffer;
	struct graphics_operations * gops;
	struct graphicsFont *        font;
	int                          fg;
	int                          bg;
	int                          transparent;            // Color index that shall be transparent    
} ;

typedef struct graphics_context * GC_ID;
 


/* general functions */

int   ini_graphics   (void);
void  close_graphics (void);
void  iniComponent   (struct graphicsBuffer * buff);
GC_ID createGC       (int vplane);
void  destroyGC       (GC_ID gc);


/* drawings */

void drawPixel       (int color, int x, int y);
int  readPixel       (int x, int y);
void drawRect        (int color, int x, int y, int width, int height);
void fillRect        (int color, int x, int y, int width, int height);
void putS            (int color, int bg_color,int x, int y, char *s);
void putC            (int color, int bg_color,int x, int y, char s);
void drawSprite      (PALETTE * palette, SPRITE * sprite, int x, int y);
void drawBITMAP      (BITMAP * bitmap, int x, int y);

/* Font */
#define needFont(name)	 FONT_ID name;
void setFont(FONT_ID font);

/* Planes */
void  setPlane        (int vplane);
void  hidePlane       (int vplane);
void  showPlane       (int vplane);



#endif
