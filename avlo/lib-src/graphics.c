/*
*   lib/graphics.c
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stdarg.h>

#include <stddef.h>

#include <io.h>

#include <kernel.h>

#include <graphics.h>
#include <osd.h>

#include <gui_pal.h>
#include <colordef.h>
#include <font.h>

#include <types.h>

#define STRING_MAXSIZE 200

#define tstXY(x,y)  {if(x>SCREEN_WIDTH) return 0; if(x<0) return 0; if(y>SCREEN_HEIGHT) return 0; if(y<0) return 0;}
#define tstWH(x,y,w,h)  {if(x+w>SCREEN_WIDTH)return 0; if(x+w<0) return 0; if(y+h>SCREEN_HEIGHT) return 0; if(y+h<0) return 0;}

char screen_BMAP1[SCREEN_WIDTH*SCREEN_HEIGHT+40];

//char screen_VID1[SCREEN_WIDTH*SCREEN_HEIGHT*4+40];


extern struct graphics_operations g8ops;
extern struct graphics_operations g32ops;

struct graphicsBuffer BITMAP_1 = {
    offset             : 0,
    state              : OSD_BITMAP_RAMCLUT | OSD_BITMAP_ZX1 |
                    OSD_BITMAP_8BIT | COLOR_TRSP << OSD_BITMAP_A_SHIFT,
    enable             : 0,
    width              : SCREEN_WIDTH,
    real_width         : SCREEN_REAL_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : SCREEN_ORIGIN_X,
    y                  : SCREEN_ORIGIN_Y,
    bitsPerPixel       : 8,
};

struct graphicsBuffer VIDEO_1 = {
    offset             : 0,
    state              : 0,
    enable             : 0,
    width              : SCREEN_WIDTH,
    real_width         : SCREEN_REAL_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : SCREEN_ORIGIN_X,
    y                  : SCREEN_ORIGIN_Y,
    bitsPerPixel       : 32,
};


#define NB_BUFFER 2

struct graphicsBuffer * buffers[NB_BUFFER];

int buffers_comp[NB_BUFFER] = {
    OSD_BITMAP1,
    OSD_VIDEO1,
};

extern FONT_ID font_table[NBFONT];

int     current_font=0;
int     current_plane=0;


#define restoreComp(COMP,BUFF)   {  osdSetComponentOffset      (COMP, BUFF.offset); \
                                    osdSetComponentSize        (COMP, 2*BUFF.width, BUFF.height); \
                                    osdSetComponentPosition    (COMP, BUFF.x, BUFF.y); \
                                    osdSetComponentSourceWidth (COMP, ((BUFF.width*BUFF.bitsPerPixel)/32)/8); \
                                    if(BUFF.enable) \
                                        osdSetComponentConfig  (COMP, BUFF.state|OSD_COMPONENT_ENABLE); \
                                 }


#define restoreComponent(VPLANE,BUFF) osdRestorePlane(buffers_comp[VPLANE],BUFF->offset, \
                BUFF->x, BUFF->y,                                                        \
                BUFF->width,BUFF->height,                                                \
                BUFF->bitsPerPixel, BUFF->state, BUFF->enable)                           \



void restoreAllComponent(void)
{
    int i;
    for(i=0;i<NB_BUFFER;i++)
        restoreComponent(i,buffers[i]);
}

extern unsigned int _end_kernel;

void ini_graphics(unsigned int vid1_address)
{
    buffers[0]=&BITMAP_1;
    buffers[1]=&VIDEO_1;

    osdInit();

    /* reset everything */
    osdSetComponentConfig(OSD_VIDEO1,  0);
    osdSetComponentConfig(OSD_VIDEO2,  0);
    osdSetComponentConfig(OSD_BITMAP1, 0);
    osdSetComponentConfig(OSD_BITMAP2, 0);
    osdSetComponentConfig(OSD_CURSOR1, 0);
    osdSetComponentConfig(OSD_CURSOR2, 0);

    setPalette(gui_pal,256);

    ini_font();
    iniComponent(BMAP1,&BITMAP_1,(unsigned int)&screen_BMAP1);
    if(vid1_address!=0)
        iniComponent(VID1,&VIDEO_1,vid1_address);
    else
    {
        iniComponent(VID1,&VIDEO_1,(unsigned int)&_end_kernel);
    }
    current_plane=BMAP1;
    current_font=0;
}

void iniComponent(int vplane,struct graphicsBuffer * buff,unsigned int offset)
{
    int diff=offset % 32;
    if(diff)
        offset+=(32-diff);
    buff->offset=offset;
    osdSetComponentOffset(buffers_comp[vplane],offset);
    osdSetComponentSize(buffers_comp[vplane], 2*buff->width, buff->height);
    osdSetComponentPosition(buffers_comp[vplane],buff->x, buff->y);
    osdSetComponentSourceWidth(buffers_comp[vplane], ((buff->width*buff->bitsPerPixel)/32)/8);
    if(buff->bitsPerPixel==8)
        buff->gops=&g8ops;
    if(buff->bitsPerPixel==32)
        buff->gops=&g32ops;
}

void doDrawLine(unsigned int color, int x1, int y1, int x2, int y2,struct graphicsBuffer * buff)
{
    int numpixels;
    int i;
    int deltax, deltay;
    int d, dinc1, dinc2;
    int x, xinc1, xinc2;
    int y, yinc1, yinc2;

    if(x1==x2)
    {
        if(y1>y2)
        {
            i=y1;
            y1=y2;
            y2=i;
        }
        buff->gops->drawVLine(color,x1,y1,y2-y1+1,buff);
        return;
    }

    if(y1==y2)
    {
        if(x1>x2)
        {
            i=x1;
            x1=x2;
            x2=i;
        }
        buff->gops->drawHLine(color,x1,y1,x2-x1+1,buff);
        return;
    }

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
        buff->gops->drawPixel(color, x, y,buff);

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

void setPlane(int vplane)
{
    current_plane=vplane;
}

int getPlane(void)
{
    return current_plane;
}

void hidePlane(int vplane)
{
    buffers[vplane]->enable=0;
        osdSetComponentConfig(buffers_comp[vplane],0);
}

void showPlane(int vplane)
{
    buffers[vplane]->enable=1;
        osdSetComponentConfig(buffers_comp[vplane],buffers[vplane]->state|OSD_COMPONENT_ENABLE);
}

int isShown(int vplane)
{
    return buffers[vplane]->enable;
}

void setState(int vplane,int state)
{
    buffers[vplane]->state=state;
}

int getState(int vplane)
{
    return buffers[vplane]->state;
}

void setSize(int vplane,int width,int height,int bitsPerPixel)
{
    buffers[vplane]->real_width=width;
        if(width%32)
          buffers[vplane]->width=width+(32-(width%32));
        else
          buffers[vplane]->width=width;
        buffers[vplane]->height=height;
        buffers[vplane]->bitsPerPixel=bitsPerPixel;
        osdSetComponentSize(buffers_comp[vplane], 2*buffers[vplane]->real_width, height);
        osdSetComponentSourceWidth(buffers_comp[vplane], ((buffers[vplane]->width*bitsPerPixel)/32)/8);
}

void getSize(int vplane,int * width,int * height,int * bitsPerPixel)
{
    *width=buffers[vplane]->real_width;
        *height=buffers[vplane]->height;
        *bitsPerPixel=buffers[vplane]->bitsPerPixel;
}

void setPos(int vplane,int x,int y)
{
        buffers[vplane]->x=x;
        buffers[vplane]->y=y;
        osdSetComponentPosition(buffers_comp[vplane],x,y);
}

void getPos(int vplane,int * x,int * y)
{
        *x=buffers[vplane]->x;
        *y=buffers[vplane]->y;
}

void clearScreen(unsigned int color)
{
        buffers[current_plane]->gops->fillRect(color,0,0,
                                buffers[current_plane]->width,
                                buffers[current_plane]->height,
                                buffers[current_plane]);
}

void drawPixel(unsigned int color,int x, int y)
{
    buffers[current_plane]->gops->drawPixel(color,x,y,buffers[current_plane]);
}

unsigned int readPixel(int x, int y)
{
    return buffers[current_plane]->gops->readPixel(x,y,buffers[current_plane]);
}

void drawRect(unsigned int color, int x, int y, int width, int height)
{
    buffers[current_plane]->gops->drawRect(color,
                                x,y,
                                width,height,
                                buffers[current_plane]);
}

void fillRect(unsigned int color, int x, int y, int width, int height)
{
    buffers[current_plane]->gops->fillRect(color,
                                x,y,
                                width,height,
                                buffers[current_plane]);
}

void drawLine(unsigned int color, int x1, int y1, int x2, int y2)
{
    doDrawLine(color,
                            x1,y1,
                            x2,y2,
                            buffers[current_plane]);
}

void putS(unsigned int color, unsigned int bg_color, int x, int y, unsigned char *s)
{
        buffers[current_plane]->gops->drawString(font_table[current_font],
                            color,bg_color,
                            x,y,
                            s,
                            buffers[current_plane]);
}

void getStringS(unsigned char *str, int *w, int *h)
{
    *w=0;
        while(*str++)
          *w += font_table[current_font]->width;
        *h=font_table[current_font]->height;
}

void putC(unsigned int color, unsigned int bg_color, int x, int y, unsigned char s)
{
    buffers[current_plane]->gops->drawChar(font_table[current_font],
                            color,bg_color,
                            x,y,
                            s,
                            buffers[current_plane]);
}

void drawSprite(unsigned int * palette, SPRITE * sprite, int x, int y)
{
    buffers[current_plane]->gops->drawSprite(palette,
                                sprite,
                                -1, // no trsp atm
                                x,y,
                                buffers[current_plane]);
}

void drawBITMAP(BITMAP * bitmap, int x, int y)
{
    buffers[current_plane]->gops->drawBITMAP(bitmap,
                                -1, // no trsp atm
                                x,y,
                                buffers[current_plane]);
}

void scrollWindowVert(unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP)
{
    buffers[current_plane]->gops->scrollWindowVert(bgColor,
                                x,y,
                                width,height,
                                scroll,
                                UP,
                                buffers[current_plane]);
}

void scrollWindowHoriz(unsigned int bgColor, int x, int y,int width, int height, int scroll, int RIGHT)
{
    buffers[current_plane]->gops->scrollWindowHoriz(bgColor,
                                x,y,
                                width,height,
                                scroll,
                                RIGHT,
                                buffers[current_plane]);
}

void setFont(int font_nb)
{
    current_font=font_nb;
}

int getFont(void)
{
    return current_font;
}

