/*
* graphics.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <graphics.h>
#include "events.h"
#include "graphics_8.h"

#define STRING_MAXSIZE 200

#define tstXY(x,y)  {if(x>SCREEN_WIDTH) return; if(x<0) return; if(y>SCREEN_HEIGHT) return; if(y<0) return;}
#define tstWH(x,y,w,h)  {if(x+w>SCREEN_WIDTH)return; if(x+w<0) return; if(y+h>SCREEN_HEIGHT) return; if(y+h<0) return;}

char screen_BMAP1[SCREEN_WIDTH*SCREEN_HEIGHT+40];
char screen_BMAP2[SCREEN_WIDTH*SCREEN_HEIGHT+40];
char screen_VID1[SCREEN_WIDTH*SCREEN_HEIGHT*4+40];
char screen_VID2[SCREEN_WIDTH*SCREEN_HEIGHT*4+40];

/*global variables */

  Display* display;				/* display */	
  Window window;				/* Create a window */
  GC gc; 

struct graphicsBuffer BITMAP_1 = {
    offset             : 0,
    /*component          : AV3XX_OSD_BITMAP1,*/
    bytesPerLine       : SCREEN_WIDTH*2,
    width              : SCREEN_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 8,
    bitsPerPixelShift  : 3,
    SWidth             : 0xa,
    /*enable             : AV3XX_OSD_BITMAP_RAMCLUT | AV3XX_OSD_BITMAP_ZX1 |
                    AV3XX_OSD_BITMAP_8BIT | AV3XX_OSD_COMPONENT_ENABLE*/
};

struct graphicsBuffer BITMAP_2 = {
    offset             : 0,
    /*component          : AV3XX_OSD_BITMAP2,*/
    bytesPerLine       : SCREEN_WIDTH*2,
    width              : SCREEN_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 8,
    bitsPerPixelShift  : 3,
    SWidth             : 0xa,
    /*enable             : AV3XX_OSD_BITMAP_RAMCLUT | AV3XX_OSD_BITMAP_ZX1 |
                    AV3XX_OSD_BITMAP_8BIT | AV3XX_OSD_COMPONENT_ENABLE*/
};        

struct graphicsBuffer VIDEO_1 = {
    offset             : 0,
    /*component          : AV3XX_OSD_VIDEO1,*/
    bytesPerLine       : SCREEN_WIDTH*2,
    width              : SCREEN_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 32,
    bitsPerPixelShift  : 5,
    SWidth             : 0x28,
    /*enable             : AV3XX_OSD_COMPONENT_ENABLE*/
};

struct graphicsBuffer VIDEO_2 = {
    offset             : 0,
    /*component          : AV3XX_OSD_VIDEO2,*/
    bytesPerLine       : SCREEN_WIDTH*2,
    width              : SCREEN_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 32,
    bitsPerPixelShift  : 5,
    SWidth             : 0x28,
    /*enable             : AV3XX_OSD_COMPONENT_ENABLE*/
};

GC_ID   default_gc=NULL;
FONT_ID default_font=GC_FONT;

GC_ID   gc_bmap1,gc_bmap2,gc_vid1,gc_vid2;

extern struct graphics_operations g8ops;

int ini_graphics()
{      
    gc_bmap1=createGC(BMAP1);
    gc_bmap2=createGC(BMAP2);
    gc_vid1=createGC(VID1); 
    gc_vid2=createGC(VID2);
    
    BITMAP_1.offset=(unsigned int)&screen_BMAP1;
    BITMAP_2.offset=(unsigned int)&screen_BMAP2;
    VIDEO_1.offset=(unsigned int)&screen_VID1;
    VIDEO_2.offset=(unsigned int)&screen_VID2;
    
    setPlane(BMAP1);
    showPlane(BMAP1);
    
    int screen;
 
    /*connect to X server */ 
    display = XOpenDisplay(0);  
    if(!display) 
    {
            printf("Error while connecting to X server");
            exit(1);
    }
                
    screen = DefaultScreen(display);
    gc = DefaultGC(display, screen);
    
    /*window Creation*/
    
    window = XCreateSimpleWindow(
            display,                               /* Display */
            DefaultRootWindow(display),            /* Main Window */
            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,     /* Geometry */
            10,                                    /* Width border */
            BlackPixel(display, screen),	
            WhitePixel(display, screen)
            );            
    if(!window) 
    {
            printf("Can't create the window");
            exit(1);
    }
    
    /* Adding Title */    
    XStoreName(display, window, "LinAV project");
    
    /* Mapping window to display */    
    XMapWindow(display, window);
         
  return 0;
}
    
void drawPixBuffer(char color, int x, int y)
{
    int gui_pal[256][3];
    int r,g,b,stockcolor;
    int screen;
    XColor c;
    Colormap pal = DefaultColormap(display,screen);
    screen = DefaultScreen(display);
    gc = DefaultGC(display, screen);
    
    stockcolor = color;
    r = gui_pal[stockcolor][1];
    g = gui_pal[stockcolor][2];
    b = gui_pal[stockcolor][3];
    c.red = r;
    c.green = g;
    c.blue = b;
    
    XAllocColor(display, pal, &c);
    XSetForeground(display, gc, c.pixel);
    XDrawPoint(display, window, gc, x, y);
}
	 

void lcd_update()
{
    int x, y;
    
    for(y=0; y<SCREEN_HEIGHT; y++)
        for(x=0; x<SCREEN_WIDTH; x++)
            drawPixBuffer(screen_BMAP1[y*SCREEN_WIDTH+x], x, y);
}	

void close_graphics()
{
    XCloseDisplay(display);
}

GC_ID createGC(int vplane)
{
    struct graphics_context * gc=(struct graphics_context *) malloc(sizeof(struct graphics_context));
    
    if(!gc)
    {
        fprintf(stderr,"can't allocate GC\n");
        return NULL;
    }
    /* default ini of GC */
    
    gc->transparent=-1;
    
    switch(vplane) {
        case BMAP1:
            gc->gops=&g8ops;
            gc->buffer=&BITMAP_1;
            break;
        case BMAP2:
            gc->gops=&g8ops;
            gc->buffer=&BITMAP_2;
            break;
        case VID1:
            gc->gops=NULL;
            gc->buffer=&VIDEO_1;
            break;
                case VID2:
            gc->gops=NULL;
            gc->buffer=&VIDEO_2;
            break;
        default:
            fprintf(stderr,"wrong plane\n");
            return NULL;
    }
            
    return gc;
}

void destroyGC(GC_ID gc)
{
    if(gc!=NULL)
        free(gc);
}

void setPlane(int vplane)
{
    switch(vplane) {
        case BMAP1:
            default_gc=gc_bmap1;
            break;
        case BMAP2:
            default_gc=gc_bmap2;
            break;
        case VID1:
            default_gc=gc_vid1;
            break;
                case VID2:
            default_gc=gc_vid2;
            break;
        default:
            fprintf(stderr,"wrong plane\n");
    }
}

void hidePlane(int vplane)
{
}

void showPlane(int vplane)
{      
}

/* drawing functions */
void clearScreen(int color)
{
    default_gc->gops->fillRect(color,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,default_gc->buffer);
}

void drawPixel(int color,int x, int y)
{
    tstXY(x,y);
    default_gc->gops->drawPixel(color, x, y,default_gc->buffer);
}

int readPixel(int x, int y)
{
    tstXY(x,y);
    return default_gc->gops->readPixel(x,y,default_gc->buffer);
}

void drawRect(int color, int x, int y, int width, int height)
{
    tstXY(x,y);
    tstWH(x,y,width,height);
    default_gc->gops->drawRect(color,x,y,width,height,default_gc->buffer);
}

void fillRect(int color, int x, int y, int width, int height)
{
    tstXY(x,y);
    tstWH(x,y,width,height);
    default_gc->gops->fillRect(color,x,y,width,height,default_gc->buffer);
}

void drawLine(int color, int x1, int y1, int x2, int y2)
{
    int numpixels;
    int i;
    int deltax, deltay;
    int d, dinc1, dinc2;
    int x, xinc1, xinc2;
    int y, yinc1, yinc2;

    tstXY(x1,y1);
    tstXY(x2,y2);

    deltax = abs(x2 - x1);
    deltay = abs(y2 - y1);

    if(deltax >= deltay)
    {
        numpixels = deltax;
        d = 2 * deltay - deltax;
        dinc1 = deltay * 2;
        dinc2 = (deltay - deltax) * 2;
        xinc1 = 1;
        xinc2 = 1;
        yinc1 = 0;
        yinc2 = 1;
    }
    else
    {
        numpixels = deltay;
        d = 2 * deltax - deltay;
        dinc1 = deltax * 2;
        dinc2 = (deltax - deltay) * 2;
        xinc1 = 0;
        xinc2 = 1;
        yinc1 = 1;
        yinc2 = 1;
    }
    numpixels++; /* include endpoints */

    if(x1 > x2)
    {
        xinc1 = -xinc1;
        xinc2 = -xinc2;
    }

    if(y1 > y2)
    {
        yinc1 = -yinc1;
        yinc2 = -yinc2;
    }

    x = x1;
    y = y1;

    for(i=0; i<numpixels; i++)
    {
        default_gc->gops->drawPixel(color, x, y,default_gc->buffer);

        if(d < 0)
        {
            d += dinc1;
            x += xinc1;
            y += yinc1;
        }
        else
        {
            d += dinc2;
            x += xinc2;
            y += yinc2;
        }
    }
}

void putS(int color, int bg_color, int x, int y, char *s)
{
    FONT_ID font=default_font;
    int len=strlen(s);
    char c;

    tstXY(x,y);

    if(font->width*len>SCREEN_WIDTH)
    {
        s[SCREEN_WIDTH/font->width]=0;
        c = s[SCREEN_WIDTH/font->width];
    }

    default_gc->gops->drawString(font,color,bg_color,x,y,s,default_gc->buffer);

    if(font->width*len>SCREEN_WIDTH)
    {
        s[SCREEN_WIDTH/font->width]=c;
    }
}

int getStringS(const unsigned char *str, int *w, int *h)
{
    FONT_ID font=default_font;

    return default_gc->gops->getStringSize(font,str,w,h);
}

void putC(int color, int bg_color, int x, int y, char s)
{
    FONT_ID font=default_font;

    tstXY(x,y);

    default_gc->gops->drawChar(font,color,bg_color,x,y,s,default_gc->buffer);
}

void drawSprite(unsigned int * palette, SPRITE * sprite, int x, int y)
{
    tstXY(x,y);
    default_gc->gops->drawSprite(palette,sprite,default_gc->transparent,x,y,default_gc->buffer);
}

void drawBITMAP(BITMAP * bitmap, int x, int y)
{
    tstXY(x,y);
    default_gc->gops->drawBITMAP(bitmap,default_gc->transparent,x,y,default_gc->buffer);
}

void scrollWindowVert(int bgColor, int x, int y, int width, int height, int scroll, int UP)
{
    default_gc->gops->scrollWindowVert(bgColor,x,y,width,height,scroll,UP,default_gc->buffer);
}

void scrollWindowHoriz(int bgColor, int x, int y, int width, int height, int scroll, int RIGHT)
{
    default_gc->gops->scrollWindowHoriz(bgColor,x,y,width,height,scroll,RIGHT,default_gc->buffer);
}

/* font */
void setFont(FONT_ID font)
{
    default_font=font;
}

FONT_ID getFont(void)
{
    return default_font;
}
