/*
*   kernel/gfx/graphics.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stdarg.h>

#include <sys_def/stddef.h>

#include <kernel/io.h>
#include <kernel/malloc.h>
#include <kernel/kernel.h>
#include <kernel/console.h>

#include <kernel/target/arch/lcd.h>

#include <kernel/graphics.h>
#include <kernel/osd.h>

#include <kernel/gui_pal.h>
#include <sys_def/colordef.h>
#include <kernel/kfont.h>

#include <kernel/swi.h>

#define STRING_MAXSIZE 200

#define tstXY(x,y)  {if(x>SCREEN_WIDTH) return 0; if(x<0) return 0; if(y>SCREEN_HEIGHT) return 0; if(y<0) return 0;}
#define tstWH(x,y,w,h)  {if(x+w>SCREEN_WIDTH)return 0; if(x+w<0) return 0; if(y+h>SCREEN_HEIGHT) return 0; if(y+h<0) return 0;}

//#define abs(val)    (val<0?-val:val)

char screen_BMAP1[SCREEN_WIDTH*SCREEN_HEIGHT+40];
char screen_BMAP2[SCREEN_WIDTH*SCREEN_HEIGHT+40];
char screen_VID1[SCREEN_WIDTH*SCREEN_HEIGHT*4+40];
char screen_VID2[SCREEN_WIDTH*SCREEN_HEIGHT*4+40];

extern struct graphics_operations g8ops;
extern struct graphics_operations g32ops;

struct graphicsBuffer BITMAP_1;
struct graphicsBuffer BITMAP_2;
struct graphicsBuffer VIDEO_1;
struct graphicsBuffer VIDEO_2;
struct graphicsBuffer CURSOR_1;
struct graphicsBuffer CURSOR_2;

struct graphicsBuffer BITMAP_1_ini = {
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

struct graphicsBuffer BITMAP_2_ini = {
    offset             : 0,
    state              : OSD_BITMAP_RAMCLUT | OSD_BITMAP_ZX1 |
                    OSD_BITMAP_8BIT | OSD_BITMAP_0TRANS | COLOR_TRSP << OSD_BITMAP_A_SHIFT,
    enable             : 0,
    width              : SCREEN_WIDTH,
    real_width         : SCREEN_REAL_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : SCREEN_ORIGIN_X,
    y                  : SCREEN_ORIGIN_Y,
    bitsPerPixel       : 8,
};

struct graphicsBuffer VIDEO_1_ini = {
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

struct graphicsBuffer VIDEO_2_ini = {
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

struct graphicsBuffer CURSOR_1_ini = {
    offset             : 0,
    state              : 0 ,
    width              : 0,
    real_width         : 0,
    height             : 0,
    x                  : 0,
    y                  : 0,
    bitsPerPixel       : 0,
};

struct graphicsBuffer CURSOR_2_ini = {
    offset             : 0,
    state              : 0 ,
    width              : 0,
    real_width         : 0,
    height             : 0,
    x                  : 0,
    y                  : 0,
    bitsPerPixel       : 0,
};

#define NB_BUFFER 6

struct graphicsBuffer * buffers[NB_BUFFER];

int buffers_comp[NB_BUFFER] = {
    OSD_BITMAP1,
    OSD_BITMAP2,
    OSD_VIDEO1,
    OSD_VIDEO2,
    OSD_CURSOR1,
    OSD_CURSOR2
};

extern FONT_ID font_table[NBFONT];

int     current_font=0;
int     current_plane=0;


void init_buffer_data(void)
{
    memcpy(&BITMAP_1,&BITMAP_1_ini,sizeof(struct graphicsBuffer));
    memcpy(&BITMAP_2,&BITMAP_2_ini,sizeof(struct graphicsBuffer));
    memcpy(&VIDEO_1,&VIDEO_1_ini,sizeof(struct graphicsBuffer));
    memcpy(&VIDEO_2,&VIDEO_2_ini,sizeof(struct graphicsBuffer));
    memcpy(&CURSOR_1,&CURSOR_1_ini,sizeof(struct graphicsBuffer));
    memcpy(&CURSOR_2,&CURSOR_2_ini,sizeof(struct graphicsBuffer));
}

void gfx_init(void)
{
    buffers[0]=&BITMAP_1;
    buffers[1]=&BITMAP_2;
    buffers[2]=&VIDEO_1;
    buffers[3]=&VIDEO_2;
    buffers[4]=&CURSOR_1;
    buffers[5]=&CURSOR_2;
    
    init_buffer_data();
    
    osd_init();

    /* reset everything */
    osd_setComponentConfig(OSD_VIDEO1,  0);
    osd_setComponentConfig(OSD_VIDEO2,  0);
    osd_setComponentConfig(OSD_BITMAP1, 0);
    osd_setComponentConfig(OSD_BITMAP2, 0);
    osd_setComponentConfig(OSD_CURSOR1, 0);
    osd_setComponentConfig(OSD_CURSOR2, 0);

    osd_setEntirePalette(gui_pal,256);

    fnt_init();
}

void gfx_initComponent(int vplane,struct graphicsBuffer * buff,unsigned int offset)
{    
    int diff=offset % 32;    
    if(diff)
        offset+=(32-diff);
    buff->offset=offset;
    osd_setComponentOffset(buffers_comp[vplane],offset);
    osd_setComponentSize(buffers_comp[vplane], 2*buff->width, buff->height);
    osd_setComponentPosition(buffers_comp[vplane],buff->x, buff->y);
    osd_setComponentSourceWidth(buffers_comp[vplane], ((buff->width*buff->bitsPerPixel)/32)/8);
    if(buff->bitsPerPixel==8)
        buff->gops=&g8ops;
    if(buff->bitsPerPixel==32)
        buff->gops=&g32ops;
}

void gfx_restoreComponent(int vplane,struct graphicsBuffer * buff)
{
    osd_restorePlane(buffers_comp[vplane],buff->offset,
        buff->x,buff->y,
        buff->width,buff->height,
        buff->bitsPerPixel, buff->state, buff->enable);

    if(buff->enable)
        osd_setComponentConfig(buffers_comp[vplane],buff->state|OSD_COMPONENT_ENABLE);
}

void gfx_restoreAllComponents(void)
{
    int i;
    for(i=0;i<4;i++)
    {
        gfx_restoreComponent(i,buffers[i]);
    }
}

void gfx_kdrawLine(unsigned int color, int x1, int y1, int x2, int y2,struct graphicsBuffer * buff)
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

void gfx_openGraphics(void)
{
    // hide the console since we're going to use the screen
    if(con_screenIsVisible()) con_screenSwitch();
    printk("INI gfx\n");
    
    /* hidding bmap1 */
    
    init_buffer_data();
    
    osd_setComponentConfig(OSD_VIDEO1,  0);
    osd_setComponentConfig(OSD_VIDEO2,  0);
    osd_setComponentConfig(OSD_BITMAP1, 0);
    osd_setComponentConfig(OSD_BITMAP2, 0);
    osd_setComponentConfig(OSD_CURSOR1, 0);
    osd_setComponentConfig(OSD_CURSOR2, 0);
    
    /*setting up planes */
    gfx_initComponent(BMAP1,&BITMAP_1,(unsigned int)&screen_BMAP1);
    gfx_initComponent(BMAP2,&BITMAP_2,(unsigned int)&screen_BMAP2);
    gfx_initComponent(VID1,&VIDEO_1,(unsigned int)&screen_VID1);
    gfx_initComponent(VID2,&VIDEO_2,(unsigned int)&screen_VID2);
    current_plane=BMAP1;
    current_font=0;
    buffers[BMAP1]->enable=1;
    osd_setComponentConfig(OSD_BITMAP1,buffers[BMAP1]->state|OSD_COMPONENT_ENABLE);
    osd_setEntirePalette(gui_pal,256);
    //printk("BMAP1 @%x\n",buffers[BMAP1]->offset);
}



void gfx_closeGraphics(void)
{
    con_screenSwitch();
}

void gfx_setPlane(int vplane)
{
    current_plane=vplane;
}

int gfx_getPlane(void)
{
    return current_plane;
}

void gfx_planeHide(int vplane)
{
    buffers[vplane]->enable=0;
    if(!con_screenIsVisible())
        osd_setComponentConfig(buffers_comp[vplane],0);
}

void gfx_planeShow(int vplane)
{
    buffers[vplane]->enable=1;
    if(!con_screenIsVisible())
        osd_setComponentConfig(buffers_comp[vplane],buffers[vplane]->state|OSD_COMPONENT_ENABLE);
}

int gfx_planeIsShown(int vplane)
{
    return buffers[vplane]->enable;
}

void gfx_planeSetState(int vplane,int state)
{
    buffers[vplane]->state=state;
}

int gfx_planeGetState(int vplane)
{
    return buffers[vplane]->state;
}

void gfx_planeSetSize(int vplane,int width,int height,int bitsPerPixel)
{
    buffers[vplane]->real_width=width;
    if(width%32)
        buffers[vplane]->width=width+(32-(width%32));
    else
        buffers[vplane]->width=width;
    buffers[vplane]->height=height;
    buffers[vplane]->bitsPerPixel=bitsPerPixel;
    if(!con_screenIsVisible())
    {
        osd_setComponentSize(buffers_comp[vplane], 2*buffers[vplane]->real_width, height);
        osd_setComponentSourceWidth(buffers_comp[vplane], ((buffers[vplane]->width*bitsPerPixel)/32)/8);
    }
}

void gfx_planeGetSize(int vplane,int * width,int * height,int * bitsPerPixel)
{
    if(width)
        *width=buffers[vplane]->real_width;
    if(height)
        *height=buffers[vplane]->height;
    if(bitsPerPixel)
    *bitsPerPixel=buffers[vplane]->bitsPerPixel;
}

void gfx_planeSetPos(int vplane,int x,int y)
{
    buffers[vplane]->x=x;
    buffers[vplane]->y=y;
    if(!con_screenIsVisible())
        osd_setComponentPosition(buffers_comp[vplane],x,y);
}

void gfx_planeGetPos(int vplane,int * x,int * y)
{
    if(x)
        *x=buffers[vplane]->x;
    if(y)
        *y=buffers[vplane]->y;
}

void* gfx_planeGetBufferOffset(int vplane)
{
    return (void*)buffers[vplane]->offset;
}

void gfx_planeSetBufferOffset(int vplane, void * offset)
{
    buffers[vplane]->offset=(unsigned int)offset;
    osd_setComponentOffset(buffers_comp[vplane],buffers[vplane]->offset);
}

void gfx_clearScreen(unsigned int color)
{
    buffers[current_plane]->gops->fillRect(color,0,0,
                                buffers[current_plane]->width,
                                buffers[current_plane]->height,
                                buffers[current_plane]);
}
void gfx_drawPixel(unsigned int color,int x, int y)
{
    buffers[current_plane]->gops->drawPixel(color,x,y,buffers[current_plane]);
}

unsigned int gfx_readPixel(int x, int y)
{
    return buffers[current_plane]->gops->readPixel(x,y,buffers[current_plane]);
}

void gfx_drawRect(unsigned int color, int x, int y, int width, int height)
{
     buffers[current_plane]->gops->drawRect(color,x,y,width,height,buffers[current_plane]);
}

void gfx_fillRect(unsigned int color, int x, int y, int width, int height)
{
    buffers[current_plane]->gops->fillRect(color,x,y,width,height,buffers[current_plane]);
}

void gfx_drawLine(unsigned int color, int x1, int y1, int x2, int y2)
{
    gfx_kdrawLine(color,x1,y1,x2,y2,buffers[current_plane]);
}

void gfx_putS(unsigned int color, unsigned int bg_color, int x, int y, unsigned char *s)
{
    buffers[current_plane]->gops->drawString(font_table[current_font],color,bg_color,
                            x,y,s,buffers[current_plane]);
}

void gfx_getStringSize(unsigned char *str, int *w, int *h)
{
    if(!str)
    {
        if(w) *w=0;
        if(h) *h=0;
    }
    
    if(w)
    {
        *w=0;
        while(*str++)
            *w += font_table[current_font]->width;
    }
    if(h)
        *h=font_table[current_font]->height;
}

void gfx_putC(unsigned int color, unsigned int bg_color, int x, int y, unsigned char s)
{
    buffers[current_plane]->gops->drawChar(font_table[current_font],color,bg_color,
                            x,y,s,buffers[current_plane]);
}

void gfx_drawSprite(unsigned int * palette, SPRITE * sprite, int x, int y)
{
    buffers[current_plane]->gops->drawSprite(palette,(SPRITE *)sprite,-1, // no trsp atm
                                x,y,buffers[current_plane]);
}

void gfx_drawBitmap(BITMAP * bitmap, int x, int y)
{
    buffers[current_plane]->gops->drawBITMAP(bitmap,-1, // no trsp atm
                                x,y,buffers[current_plane]);
}

void gfx_scrollWindowVert(unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP)
{
    buffers[current_plane]->gops->scrollWindowVert(bgColor,x,y,width,height,
                                scroll,UP,buffers[current_plane]);
}

void gfx_scrollWindowHoriz(unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT)
{
    buffers[current_plane]->gops->scrollWindowHoriz(bgColor,x,y,width,height,
                                scroll,RIGHT,buffers[current_plane]);
}

void gfx_setPalletteRGB(int r, int g, int b, int index)
{
    osd_setPaletteRGB(r,g,b,index);
}

void gfx_fontSet(int font_nb)
{
    current_font=font_nb;
}

int gfx_fontGet(void)
{
    return current_font;
}

