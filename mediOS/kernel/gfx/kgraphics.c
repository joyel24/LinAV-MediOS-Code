/* 
*   kernel/gfx/graphics.c
*
*   AMOS project
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

#include <kernel/kgraphics.h>
#include <kernel/osd.h>

#include <kernel/gui_pal.h>
#include <sys_def/colordef.h>
#include <kernel/kfont.h>

#include <kernel/swi.h>

#include <types.h>

#define STRING_MAXSIZE 200

#define tstXY(x,y)  {if(x>SCREEN_WIDTH) return 0; if(x<0) return 0; if(y>SCREEN_HEIGHT) return 0; if(y<0) return 0;}
#define tstWH(x,y,w,h)  {if(x+w>SCREEN_WIDTH)return 0; if(x+w<0) return 0; if(y+h>SCREEN_HEIGHT) return 0; if(y+h<0) return 0;}

//#define abs(val)    (val<0?-val:val)

#ifdef USE_DEBUG_ON_SCREEN 
char screen_ERROR[SCREEN_WIDTH*SCREEN_HEIGHT+40];
#endif
char screen_BMAP1[SCREEN_WIDTH*SCREEN_HEIGHT+40];
char screen_BMAP2[SCREEN_WIDTH*SCREEN_HEIGHT+40];
char screen_VID1[SCREEN_WIDTH*SCREEN_HEIGHT*4+40];
char screen_VID2[SCREEN_WIDTH*SCREEN_HEIGHT*4+40];

extern struct graphics_operations g8ops;
extern struct graphics_operations g32ops;

#ifdef USE_DEBUG_ON_SCREEN 
struct graphicsBuffer ERROR_SCR = {
    offset             : 0,
    state              : OSD_BITMAP_RAMCLUT | OSD_BITMAP_ZX1 |
                    OSD_BITMAP_8BIT | COLOR_TRSP << OSD_BITMAP_A_SHIFT,
    enable             : 0,
    width              : SCREEN_WIDTH,
    real_width         : SCREEN_REAL_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 8,
};
#endif

struct graphicsBuffer BITMAP_1 = {
    offset             : 0,
    state              : OSD_BITMAP_RAMCLUT | OSD_BITMAP_ZX1 |
                    OSD_BITMAP_8BIT | COLOR_TRSP << OSD_BITMAP_A_SHIFT,
    enable             : 0,
    width              : SCREEN_WIDTH,
    real_width         : SCREEN_REAL_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 8,   
};

struct graphicsBuffer BITMAP_2 = {
    offset             : 0,
    state              : OSD_BITMAP_RAMCLUT | OSD_BITMAP_ZX1 |
                    OSD_BITMAP_8BIT | OSD_BITMAP_0TRANS | COLOR_TRSP << OSD_BITMAP_A_SHIFT,
    enable             : 0,
    width              : SCREEN_WIDTH,
    real_width         : SCREEN_REAL_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 8,
};        

struct graphicsBuffer VIDEO_1 = {
    offset             : 0,
    state              : 0,
    enable             : 0,
    width              : SCREEN_WIDTH,
    real_width         : SCREEN_REAL_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 32,
};

struct graphicsBuffer VIDEO_2 = {
    offset             : 0,
    state              : 0,
    enable             : 0,
    width              : SCREEN_WIDTH,
    real_width         : SCREEN_REAL_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 32,
};

struct graphicsBuffer CURSOR_1 = {
    offset             : 0,
    state              : 0 ,
    width              : 0,
    real_width         : 0,
    height             : 0,
    x                  : 0,
    y                  : 0,
    bitsPerPixel       : 0,   
};

struct graphicsBuffer CURSOR_2 = {
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


#define restoreComp(COMP,BUFF)   {  osdSetComponentOffset      (COMP, BUFF.offset); \
                                    osdSetComponentSize        (COMP, 2*BUFF.width, BUFF.height); \
                                    osdSetComponentPosition    (COMP, BUFF.x, BUFF.y); \
                                    osdSetComponentSourceWidth (COMP, ((BUFF.width*BUFF.bitsPerPixel)/32)/8); \
                                    if(BUFF.enable) \
                                        osdSetComponentConfig  (COMP, BUFF.state|OSD_COMPONENT_ENABLE); \
                                 }

/********************************************************* Error screen ******/

static int error_scr_state;

#ifdef USE_DEBUG_ON_SCREEN 
void clear_error_scr(int color)
{
    g8ops.fillRect(color,0,0,ERROR_SCR.width,ERROR_SCR.height,&ERROR_SCR);
}

void putS_error_scr(FONT_ID font,unsigned int color, unsigned int bg_color, int x, int y, unsigned char *s)
{
    g8ops.drawString(font,color,bg_color,x,y,s,&ERROR_SCR);
}

void putC_error_scr(FONT_ID font,unsigned int color, unsigned int bg_color, int x, int y, unsigned char c)
{
    g8ops.drawChar(font,color,bg_color,x,y,c,&ERROR_SCR);
}

void scroll_error_scr(unsigned int bg_color,int x,int y,int height,int UP)
{
    g8ops.scrollWindowVert(bg_color, x, y, SCREEN_WIDTH-x, SCREEN_HEIGHT-y, height, UP, &ERROR_SCR);
}
#endif

/*void restoreComponent(int vplane,struct graphicsBuffer * buff)
{
    osdSetComponentOffset(buffers_comp[vplane],buff->offset);
    osdSetComponentSize(buffers_comp[vplane], 2*buff->width, buff->height);
    osdSetComponentPosition(buffers_comp[vplane],buff->x, buff->y);
    osdSetComponentSourceWidth(buffers_comp[vplane], ((buff->width*buff->bitsPerPixel)/32)/8);
    if(buff->enable)
        osdSetComponentConfig(buffers_comp[vplane],buff->state|OSD_COMPONENT_ENABLE);
}*/

#define restoreComponent(VPLANE,BUFF) osdRestorePlane(buffers_comp[VPLANE],BUFF->offset, \
                BUFF->x, BUFF->y,                                                        \
                BUFF->width,BUFF->height,                                                \
                BUFF->bitsPerPixel, BUFF->state, BUFF->enable)                           \



void restoreAllComponent(void)
{
    int i;
    for(i=0;i<4;i++)
        restoreComponent(i,buffers[i]);
}

#ifdef USE_DEBUG_ON_SCREEN 
void error_scr_switch()
{
    if(error_scr_state)
    {
        error_scr_state=0;
        restoreAllComponent();
        printk("Switching to normal screen\n");
    }
    else
    {
        error_scr_state=1;
        osdSetComponentConfig(OSD_VIDEO1,  0);
        osdSetComponentConfig(OSD_VIDEO2,  0);
        osdSetComponentConfig(OSD_BITMAP1, 0);
        osdSetComponentConfig(OSD_BITMAP2, 0);
        osdSetComponentConfig(OSD_CURSOR1, 0);
        osdSetComponentConfig(OSD_CURSOR2, 0);
        restoreComp(OSD_BITMAP1,ERROR_SCR);        
        printk("Switching to debug screen\n");
    }
}
#endif

/****************************************************************************/

void ini_graphics(void)
{
    buffers[0]=&BITMAP_1;
    buffers[1]=&BITMAP_2;
    buffers[2]=&VIDEO_1;
    buffers[3]=&VIDEO_2;
    buffers[4]=&CURSOR_1;
    buffers[5]=&CURSOR_2;
    
    osdInit();
    
    /* reset everything */
    osdSetComponentConfig(OSD_VIDEO1,  0);
    osdSetComponentConfig(OSD_VIDEO2,  0);
    osdSetComponentConfig(OSD_BITMAP1, 0);
    osdSetComponentConfig(OSD_BITMAP2, 0);
    osdSetComponentConfig(OSD_CURSOR1, 0);
    osdSetComponentConfig(OSD_CURSOR2, 0);
    
    setPalette(gui_pal,256);
#ifdef USE_DEBUG_ON_SCREEN 
    error_scr_state=1;
    
    iniComponent(BMAP1,&ERROR_SCR,(unsigned int)&screen_ERROR);
    osdSetComponentConfig(OSD_BITMAP1,ERROR_SCR.state|OSD_COMPONENT_ENABLE);    
    ERROR_SCR.enable=1;
#endif    
    ini_font();
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

void KdrawLine(unsigned int color, int x1, int y1, int x2, int y2,struct graphicsBuffer * buff)
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

void user_printf(const char * fmt, va_list args);

int swi_gfx_handler(unsigned long nCmd,unsigned long cmd,
    unsigned long nParam2,
    unsigned long nParam3)
{
    GFX_DATA * g_data = (GFX_DATA *) nParam2;
    void * pvData = (void*)nParam3;
        
    int * int_data=(int*)nParam3;
    unsigned char * str=(unsigned char*)nParam3;
    PLANE_DATA * p_data=(PLANE_DATA*) nParam3;
    
    if(nCmd == nAPI_PRINTF)
    {
        user_printf((const char *)cmd, (va_list) nParam2);
    }
    else if(nCmd == nAPI_GFX)
    {    
        printk("processing GFX fct: %x\n",cmd);
            switch(cmd) {
                case 0x000:                      /* void open_graphics(void) */
                    error_scr_state=0;    
                    /* hidding bmap1 */
                    osdSetComponentConfig(OSD_BITMAP1, 0);            
                    /*setting up planes */              
                    iniComponent(BMAP1,&BITMAP_1,(unsigned int)&screen_BMAP1); 
                    iniComponent(BMAP2,&BITMAP_2,(unsigned int)&screen_BMAP2); 
                    iniComponent(VID1,&VIDEO_1,(unsigned int)&screen_VID1);
                    iniComponent(VID2,&VIDEO_2,(unsigned int)&screen_VID2);
                    current_plane=BMAP1;
                    current_font=0;
                    buffers[BMAP1]->enable=1;
                    osdSetComponentConfig(OSD_BITMAP1,buffers[BMAP1]->state|OSD_COMPONENT_ENABLE);
                    printk("BMAP1 @%x\n",buffers[BMAP1]->offset);
                    break;
                case 0x001:                      /* void close_graphics(void) */
                    break;
                case 0x100:                      /* void setPlane(int vplane) */
                    current_plane=*int_data;
                    break;
                case 0x101:                      /* int getPlane(void) */
                    *int_data=current_plane;
                    break;
                case 0x102:                      /* void hidePlane(int vplane) */
                    buffers[*int_data]->enable=0;
                    if(!error_scr_state)
                        osdSetComponentConfig(buffers_comp[*int_data],0);
                    break;
                case 0x103:                      /* void showPlane(int vplane) */
                    buffers[*int_data]->enable=1;
                    if(!error_scr_state)
                        osdSetComponentConfig(buffers_comp[*int_data],buffers[*int_data]->state|OSD_COMPONENT_ENABLE);
                    break;
                case 0x104:                      /* int isShown(int vplane) */
                    *int_data=buffers[*int_data]->enable;
                    break;
                case 0x105:                      /* void setState(int vplane,int state) */
                    buffers[p_data->vplane]->state=p_data->state;
                    break;
                case 0x106:                      /* int getState(int vplane) */
                    *int_data=buffers[*int_data]->state;
                    break;
                case 0x107:                      /* void setSize(int vplane,int width,int height,int bitsPerPixel) */
                    buffers[p_data->vplane]->real_width=g_data->w;
                    if(g_data->w%32)
                        buffers[p_data->vplane]->width=g_data->w+(32-(g_data->w%32));
                    else
                        buffers[p_data->vplane]->width=g_data->w;
                    buffers[p_data->vplane]->height=g_data->h;
                    buffers[p_data->vplane]->bitsPerPixel=p_data->bpp;
                    if(!error_scr_state)
                    {
                        osdSetComponentSize(buffers_comp[p_data->vplane], 2*buffers[p_data->vplane]->real_width, g_data->h);
                        osdSetComponentSourceWidth(buffers_comp[p_data->vplane], ((buffers[p_data->vplane]->width*p_data->bpp)/32)/8);
                    } 
                    break;
                case 0x108:                      /* void getSize(int vplane,int * width,int * height,int * bitsPerPixel) */            
                    g_data->w=buffers[p_data->vplane]->real_width;
                    g_data->h=buffers[p_data->vplane]->height;
                    p_data->bpp=buffers[p_data->vplane]->bitsPerPixel;
                    break;
                case 0x109:                      /* void setPos(int vplane,int x,int y) */
                    buffers[p_data->vplane]->x=g_data->x;
                    buffers[p_data->vplane]->y=g_data->y;
                    if(!error_scr_state)
                        osdSetComponentPosition(buffers_comp[p_data->vplane],g_data->x,g_data->y);
                    break;
                case 0x10A:                      /* void getPos(int vplane,int * x,int * y) */
                    g_data->x=buffers[p_data->vplane]->x;
                    g_data->y=buffers[p_data->vplane]->y;
                    break;
                case 0x200:                      /* void clearScreen(unsigned int color) */            
                    buffers[current_plane]->gops->fillRect(*int_data,0,0,
                                buffers[current_plane]->width,
                                buffers[current_plane]->height,
                                buffers[current_plane]);
                    break;
                case 0x201:                      /* void drawPixel(unsigned int color,int x, int y) */
                    buffers[current_plane]->gops->drawPixel(g_data->color,g_data->x,g_data->y,buffers[current_plane]);
                    break;
                case 0x202:                      /* unsigned int readPixel(int x, int y) */
                    g_data->color=buffers[current_plane]->gops->readPixel(g_data->x,g_data->y,buffers[current_plane]);
                    break;
                case 0x203:                      /* void drawRect(unsigned int color, int x, int y, int width, int height) */
                    buffers[current_plane]->gops->drawRect(g_data->color,
                                g_data->x,g_data->y,
                                g_data->w,g_data->h,
                                buffers[current_plane]);
                    break;
                case 0x204:                      /* void fillRect(unsigned int color, int x, int y, int width, int height) */
                    buffers[current_plane]->gops->fillRect(g_data->color,
                                g_data->x,g_data->y,
                                g_data->w,g_data->h,
                                buffers[current_plane]);
                    break;
                case 0x205:                      /* void drawLine(unsigned int color, int x1, int y1, int x2, int y2) */
                    KdrawLine(g_data->color,
                            g_data->x,g_data->y,
                            g_data->w,g_data->h,
                            buffers[current_plane]);
                    break;
                case 0x206:                      /* void putS(unsigned int color, unsigned int bg_color, int x, int y, unsigned char *s) */
                    buffers[current_plane]->gops->drawString(font_table[current_font],
                            g_data->color,g_data->bg_color,
                            g_data->x,g_data->y,
                            str,
                            buffers[current_plane]);
                    break;
                case 0x207:                      /* void getStringS(unsigned char *str, int *w, int *h) */
                    g_data->w=0;
                    while(*str++)
                        g_data->w += font_table[current_font]->width;
                    g_data->h=font_table[current_font]->height;
                    break;
                case 0x208:                      /* void putC(unsigned int color, unsigned int bg_color, int x, int y, unsigned char s) */
                    buffers[current_plane]->gops->drawChar(font_table[current_font],
                            g_data->color,g_data->bg_color,
                            g_data->x,g_data->y,
                            *str,
                            buffers[current_plane]);
                    break;
                case 0x209:                      /* void drawSprite(unsigned int * palette, SPRITE * sprite, int x, int y) */
                    buffers[current_plane]->gops->drawSprite((unsigned int *)g_data->color,
                                (SPRITE *)pvData,
                                -1, // no trsp atm
                                g_data->x,g_data->y,
                                buffers[current_plane]);
                    break;
                case 0x20A:                      /* void drawBITMAP(BITMAP * bitmap, int x, int y) */
                    buffers[current_plane]->gops->drawBITMAP((BITMAP *)pvData,
                                -1, // no trsp atm
                                g_data->x,g_data->y,
                                buffers[current_plane]);
                    break;
                case 0x20B:                      /* void scrollWindowVert(unsigned int bgColor, int x, int y, 
                                                            int width, int height, int scroll, int UP) */
                    buffers[current_plane]->gops->scrollWindowVert(g_data->bg_color,
                                g_data->x,g_data->y,
                                g_data->w,g_data->h,
                                g_data->delta,
                                g_data->direction,
                                buffers[current_plane]);
                    break;
                case 0x20C:                      /* void scrollWindowHoriz(unsigned int bgColor, int x, int y, 
                                                            int width, int height, int scroll, int RIGHT) */
                    buffers[current_plane]->gops->scrollWindowHoriz(g_data->bg_color,
                                g_data->x,g_data->y,
                                g_data->w,g_data->h,
                                g_data->delta,
                                g_data->direction,
                                buffers[current_plane]);
                    break;
                case 0x20D:
                    //printk("K set palette (%d,%d,%d) at %d\n",g_data->x,g_data->y,g_data->w,g_data->h);
                    osdSetPaletteRGB(g_data->x,g_data->y,g_data->w,g_data->h);
                    break;
                case 0x20E:
                    printk("Buffer @ of plane : %x => @0x%x\n",g_data->x,buffers[g_data->x]->offset);
                    g_data->color = buffers[g_data->x]->offset;
                    break;
                case 0x300:                      /* void setFont(int font_nb) */
                    current_font=*int_data;
                    break;
                case 0x301:                      /* int getFont(void) */
                    *int_data=current_font;
                    break;
            }
           }
    return 0;
}
