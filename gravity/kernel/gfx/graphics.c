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


#include <sys_def/stddef.h>
#include <sys_def/malloc.h>
#include <kernel/kernel.h>

#include <kernel/graphics.h>
#include <kernel/osd.h>

#include <kernel/gui_pal.h>
#include <kernel/colordef.h>

#define STRING_MAXSIZE 200

#define tstXY(x,y)  {if(x>SCREEN_WIDTH) return 0; if(x<0) return 0; if(y>SCREEN_HEIGHT) return 0; if(y<0) return 0;}
#define tstWH(x,y,w,h)  {if(x+w>SCREEN_WIDTH)return 0; if(x+w<0) return 0; if(y+h>SCREEN_HEIGHT) return 0; if(y+h<0) return 0;}

#define abs(val)    (val<0?-val:val)

char screen_ERROR[SCREEN_WIDTH*SCREEN_HEIGHT+40];
char screen_BMAP1[SCREEN_WIDTH*SCREEN_HEIGHT+40];
char screen_BMAP2[SCREEN_WIDTH*SCREEN_HEIGHT+40];
char screen_VID1[SCREEN_WIDTH*SCREEN_HEIGHT*4+40];
char screen_VID2[SCREEN_WIDTH*SCREEN_HEIGHT*4+40];

struct graphicsBuffer ERROR_SCR = {
    offset             : 0,
    state              : OSD_BITMAP_RAMCLUT | OSD_BITMAP_ZX1 |
                    OSD_BITMAP_8BIT | COLOR_TRSP << OSD_BITMAP_A_SHIFT,
    enable             : 0,
    width              : SCREEN_WIDTH,
    real_width         : SCREEN_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 8,   
};

struct graphicsBuffer BITMAP_1 = {
    offset             : 0,
    state              : OSD_BITMAP_RAMCLUT | OSD_BITMAP_ZX1 |
                    OSD_BITMAP_8BIT | COLOR_TRSP << OSD_BITMAP_A_SHIFT,
    enable             : 0,
    width              : SCREEN_WIDTH,
    real_width         : SCREEN_WIDTH,
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
    real_width         : SCREEN_WIDTH,
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
    real_width         : SCREEN_WIDTH,
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
    real_width         : SCREEN_WIDTH,
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


GC_ID   default_gc=NULL;
FONT_ID default_font=GC_FONT;
int     default_plane=0;

GC_ID   gc_bmap1,gc_bmap2,gc_vid1,gc_vid2;
//struct graphics_context gc_bmap1_data,gc_bmap2_data,gc_vid1_data,gc_vid2_data;

GC_ID listGC[NB_BUFFER];

extern struct graphics_operations g8ops;
extern struct graphics_operations g32ops;

/********************************************************* Error screen ******/

static int error_scr_state;

void ini_error_scr(void)
{   
    iniComponent(BMAP1,&ERROR_SCR,(unsigned int)&screen_ERROR);
    osdSetComponentConfig(buffers_comp[BMAP1],ERROR_SCR.state|OSD_COMPONENT_ENABLE);
    ERROR_SCR.enable=1;
}

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
        restoreComponent(BMAP1,&ERROR_SCR);
        printk("Switching to debug screen\n");
    }
}

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

    error_scr_state=1;
}

void open_graphics()
{
    error_scr_state=0;
    
    /* hidding bmap1 */
    osdSetComponentConfig(OSD_BITMAP1, 0);
    
    /*setting up planes */   
    
    iniComponent(BMAP1,&BITMAP_1,(unsigned int)&screen_BMAP1); 
    iniComponent(BMAP2,&BITMAP_2,(unsigned int)&screen_BMAP2); 
    iniComponent(VID1,&VIDEO_1,(unsigned int)&screen_VID1);
    iniComponent(VID2,&VIDEO_2,(unsigned int)&screen_VID2);
    
    
    gc_bmap1=createGC(BMAP1);
    gc_bmap2=createGC(BMAP2);
    gc_vid1=createGC(VID1);
    gc_vid2=createGC(VID2);
    
    /*gc_bmap1=&gc_bmap1_data;
    gc_bmap2=&gc_bmap2_data;
    gc_vid1=&gc_vid1_data;
    gc_vid2=&gc_vid2_data;
    
    initGC(BMAP1,gc_bmap1);
    initGC(BMAP2,gc_bmap2);
    initGC(VID1,gc_vid1);
    initGC(VID2,gc_vid2);*/
    
    listGC[0]=gc_bmap1;
    listGC[1]=gc_bmap2;
    listGC[2]=gc_vid1;
    listGC[3]=gc_vid2;
    

          
    setPlane(BMAP1);
    showPlane(BMAP1);
}

void setPalette(int palette[256][3],int size)
{
    int i=0;
    int y,cr,cb;
    for(i=0;i<size;i++)
    {
        y = (306*palette[i][0] + 601*palette[i][1] + 117*palette[i][2]) >> 10 ; 
        cb = ((-173*palette[i][0] -339*palette[i][1] + 512*palette[i][2]) >> 10) + 128;
        cr = ((512*palette[i][0] - 428*palette[i][1] - 84*palette[i][2]) >> 10) + 128;
    
        /*printk("[setPalette] (%03d) RGB=(%03d,%03d,%03d) YCrCb=(%03d,%03d,%03d)\n",i,
                    palette[i][0],palette[i][1],palette[i][2],
                    y,cr,cb);*/
        
        osdSetPallette (y, cr, cb, i);
    }
}

void close_graphics()
{

}

int initGC(int vplane,GC_ID gc)
{
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
            gc->gops=&g32ops;
            gc->buffer=&VIDEO_1;
            break;
        case VID2:
            gc->gops=&g32ops;
            gc->buffer=&VIDEO_2;
            break;
        default:
            printk("wrong plane %d\n",vplane);
            return 0;
    }
    return 1;
}

GC_ID createGC(int vplane)
{
    struct graphics_context * gc;
    gc=(struct graphics_context *) malloc(sizeof(struct graphics_context));
    
    if(!gc)
    {
        printk("can't allocate GC\n");
        return NULL;
    }
    /* default ini of GC */
    
    if(!initGC(vplane,gc))
        return NULL;
            
    return gc;
}

void destroyGC(GC_ID gc)
{
    if(gc!=NULL)
        free(gc);
}

void setPlane(int vplane)
{
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        default_gc=listGC[vplane];
        default_plane=vplane;
    }
    else
    {
        printk("wrong plane %d\n",vplane);
    } 
}

int getPlane(void)
{
    return default_plane;
}

void hidePlane(int vplane)
{
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        buffers[vplane]->enable=0;
        if(!error_scr_state)
            osdSetComponentConfig(buffers_comp[vplane],0);
    }
    else
    {
        printk("wrong plane %d\n",vplane);
    }    
}

void showPlane(int vplane)
{
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        buffers[vplane]->enable=1;
        if(!error_scr_state)
            osdSetComponentConfig(buffers_comp[vplane],buffers[vplane]->state|OSD_COMPONENT_ENABLE);
    }
    else
    {
        printk("wrong plane %d\n",vplane);
    }          
}

int isShown(int vplane)
{
    return buffers[vplane]->enable;
}

void setState(int vplane,int state)
{
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        buffers[vplane]->state=state;
    }
    else
    {
        printk("wrong plane %d\n",vplane);
    }       
}

int getState(int vplane)
{
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        return buffers[vplane]->state;
    }
    else
    {
        printk("wrong plane %d\n",vplane);
        return -1;
    }        
}

void setSize(int vplane,int width,int height,int bitsPerPixel)
{
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        buffers[vplane]->real_width=width;
        if(width%32)
            buffers[vplane]->width=width+(32-(width%32));
        else
            buffers[vplane]->width=width;
        buffers[vplane]->height=height;
        buffers[vplane]->bitsPerPixel=bitsPerPixel;
        if(!error_scr_state)
        {
            osdSetComponentSize(buffers_comp[vplane], 2*buffers[vplane]->real_width, height);
            osdSetComponentSourceWidth(buffers_comp[vplane], ((buffers[vplane]->width*bitsPerPixel)/32)/8);
        }
    }
    else
    {
        printk("wrong plane %d\n",vplane);
    }           
}

void getSize(int vplane,int * width,int * height,int * bitsPerPixel)
{
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        if(width)
            *width=buffers[vplane]->real_width;
        if(height)
            *height=buffers[vplane]->height;
        if(bitsPerPixel)
            *bitsPerPixel=buffers[vplane]->bitsPerPixel;
    }
    else
    {
        printk("wrong plane %d\n",vplane);
    }          
}

void setPos(int vplane,int x,int y)
{
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        buffers[vplane]->x=x;
        buffers[vplane]->y=y;
        if(!error_scr_state)
            osdSetComponentPosition(buffers_comp[vplane],x,y);        
    }
    else
    {
        printk("wrong plane %d\n",vplane);
    }        
}

void getPos(int vplane,int * x,int * y)
{
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        if(x)
            *x=buffers[vplane]->x;
        if(y)
            *y=buffers[vplane]->y;
    }
    else
    {
        printk("wrong plane %d\n",vplane);
    }         
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
}

void restoreComponent(int vplane,struct graphicsBuffer * buff)
{
    osdSetComponentOffset(buffers_comp[vplane],buff->offset);
    osdSetComponentSize(buffers_comp[vplane], 2*buff->width, buff->height);
    osdSetComponentPosition(buffers_comp[vplane],buff->x, buff->y);
    osdSetComponentSourceWidth(buffers_comp[vplane], ((buff->width*buff->bitsPerPixel)/32)/8);
    if(buff->enable)
        osdSetComponentConfig(buffers_comp[vplane],buff->state|OSD_COMPONENT_ENABLE);
}

void restoreAllComponent(void)
{
    int i;
    for(i=0;i<NB_BUFFER;i++)
        restoreComponent(i,buffers[i]);
}


/* drawing functions */
void clearScreen(unsigned int color)
{
    default_gc->gops->fillRect(color,0,0,default_gc->buffer->width,default_gc->buffer->height,default_gc->buffer);    
}

int drawPixel(unsigned int color,int x, int y)
{
    tstXY(x,y);
    default_gc->gops->drawPixel(color, x, y,default_gc->buffer);
    return 1;
}

unsigned int readPixel(int x, int y)
{
    tstXY(x,y);
    return default_gc->gops->readPixel(x,y,default_gc->buffer);
}

int drawRect(unsigned int color, int x, int y, int width, int height)
{
    tstXY(x,y);
    tstWH(x,y,width,height);
    default_gc->gops->drawRect(color,x,y,width,height,default_gc->buffer);
    return 1;
}

int fillRect(unsigned int color, int x, int y, int width, int height)
{
    tstXY(x,y);
    tstWH(x,y,width,height);
    default_gc->gops->fillRect(color,x,y,width,height,default_gc->buffer);
    return 1;
}

int drawLine(unsigned int color, int x1, int y1, int x2, int y2)
{
    int numpixels;
    int i;
    int deltax, deltay;
    int d, dinc1, dinc2;
    int x, xinc1, xinc2;
    int y, yinc1, yinc2;

    tstXY(x1,y1);
    tstXY(x2,y2);
    
    if(x1==x2)
    {
        if(y1>y2)
        {
            i=y1;
            y1=y2;
            y2=i;
        }
        default_gc->gops->drawVLine(color,x1,y1,y2-y1+1,default_gc->buffer);
        return 1;        
    }
    
    if(y1==y2)
    {
        if(x1>x2)
        {
            i=x1;
            x1=x2;
            x2=i;
        }
        default_gc->gops->drawHLine(color,x1,y1,x2-x1+1,default_gc->buffer);
        return 1;
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
    
     return 1;  
}

int putS(unsigned int color, unsigned int bg_color, int x, int y, unsigned char *s)
{
    FONT_ID font=default_font;
    //int len=strlen(s);
    int h=0,w=0;
    unsigned char c=0;

    tstXY(x,y);    
        
    getStringS(s,&w,&h);
    
    if(y+h>SCREEN_HEIGHT)
    {
        printk("!!!!!!!!!! str going out of screen\n");
        //return 0;
    }
    
    if(x+w>SCREEN_WIDTH)
    {
        printk("!!!!!!!!!! string too long %s\n",s);
        c = s[(SCREEN_WIDTH-x)/font->width];
        s[(SCREEN_WIDTH-x)/font->width]=0;                
    }

    default_gc->gops->drawString(font,color,bg_color,x,y,s,default_gc->buffer);

    if(c!=0)
    {
        s[(SCREEN_WIDTH-x)/font->width]=c;
    }
  
    return 1;
}

void getStringS(unsigned char *str, int *w, int *h)
{
    FONT_ID font=default_font;
    
    if ( w )
    {
        *w=0;
        while(*str++)
            *w += font->width;
    }
    if ( h )
        *h = font->height;
}

int putC(unsigned int color, unsigned int bg_color, int x, int y, unsigned char s)
{
    FONT_ID font=default_font;
    tstXY(x,y);

    default_gc->gops->drawChar(font,color,bg_color,x,y,s,default_gc->buffer);
    return 1;
}

int drawSprite(unsigned int * palette, SPRITE * sprite, int x, int y)
{
    tstXY(x,y);
    default_gc->gops->drawSprite(palette,sprite,default_gc->transparent,x,y,default_gc->buffer);
    return 1;
}

int drawBITMAP(BITMAP * bitmap, int x, int y)
{
    tstXY(x,y);
    default_gc->gops->drawBITMAP(bitmap,default_gc->transparent,x,y,default_gc->buffer);
    return 1;
}

void scrollWindowVert(unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP)
{
    default_gc->gops->scrollWindowVert(bgColor,x,y,width,height,scroll,UP,default_gc->buffer);
}

void scrollWindowHoriz(unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT)
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
