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

#ifdef AV_SCREEN
#include <fcntl.h>
#include <sys/ioctl.h>
#include <osd.h>
#define FBIO_INIT        _IO ('F', 0x26)
#define LCD_UPDATE(x,y,w,h)       {;}
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#define LCD_UPDATE(x,y,w,h)       {lcd_update(UPDATE_ONLY,x,y,w,h);}
Display* display;
Window window;
GC gc;
int screen;
int colorTab[256];
#endif

#include <graphics.h>
#include "events.h"
#include "gui_pal.h"

#ifdef HAVE_JPEG
#include "jpeglib.h"
#endif

#define STRING_MAXSIZE 200

#define tstXY(x,y)  {if(x>SCREEN_WIDTH) return; if(x<0) return; if(y>SCREEN_HEIGHT) return; if(y<0) return;}
#define tstWH(x,y,w,h)  {if(x+w>SCREEN_WIDTH)return; if(x+w<0) return; if(y+h>SCREEN_HEIGHT) return; if(y+h<0) return;}

char screen_BMAP1[SCREEN_WIDTH*SCREEN_HEIGHT+40];
char screen_BMAP2[SCREEN_WIDTH*SCREEN_HEIGHT+40];
char screen_VID1[SCREEN_WIDTH*SCREEN_HEIGHT*4+40];
char screen_VID2[SCREEN_WIDTH*SCREEN_HEIGHT*4+40];

#ifndef AV_SCREEN
char screen_BMAP1_SAV[SCREEN_WIDTH*SCREEN_HEIGHT+40];
#endif

struct graphicsBuffer BITMAP_1 = {
    offset             : 0,
#ifdef AV_SCREEN
    state              : AV3XX_OSD_BITMAP_RAMCLUT | AV3XX_OSD_BITMAP_ZX1 |
                    AV3XX_OSD_BITMAP_8BIT ,
#endif
    width              : SCREEN_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 8,   
};

struct graphicsBuffer BITMAP_2 = {
    offset             : 0,
#ifdef AV_SCREEN
    state              : AV3XX_OSD_BITMAP_RAMCLUT | AV3XX_OSD_BITMAP_ZX1 |
                    AV3XX_OSD_BITMAP_8BIT ,
#endif
    width              : SCREEN_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 8,
};        

struct graphicsBuffer VIDEO_1 = {
    offset             : 0,
#ifdef AV_SCREEN
    state              : 0,
#endif
    width              : SCREEN_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 32,
};

struct graphicsBuffer VIDEO_2 = {
    offset             : 0,
#ifdef AV_SCREEN
    state              : 0,
#endif
    width              : SCREEN_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 32,
};

struct graphicsBuffer CURSOR_1 = {
    offset             : 0,
#ifdef AV_SCREEN
    state              : 0 ,
#endif
    width              : 0,
    height             : 0,
    x                  : 0,
    y                  : 0,
    bitsPerPixel       : 0,   
};

struct graphicsBuffer CURSOR_2 = {
    offset             : 0,
#ifdef AV_SCREEN
    state              : 0 ,
#endif
    width              : 0,
    height             : 0,
    x                  : 0,
    y                  : 0,
    bitsPerPixel       : 0,   
};

#define NB_BUFFER 6

struct graphicsBuffer * buffers[NB_BUFFER];

#ifdef AV_SCREEN
int buffers_comp[NB_BUFFER] = {
    AV3XX_OSD_BITMAP1,
    AV3XX_OSD_BITMAP2,
    AV3XX_OSD_VIDEO1,
    AV3XX_OSD_VIDEO2,
    AV3XX_OSD_CURSOR1,
    AV3XX_OSD_CURSOR2
};
#endif

GC_ID   default_gc=NULL;
FONT_ID default_font=GC_FONT;

GC_ID   gc_bmap1,gc_bmap2,gc_vid1,gc_vid2;

GC_ID listGC[NB_BUFFER];

extern struct graphics_operations g8ops;
extern struct graphics_operations g32ops;

int ini_graphics()
{
    int x, y;
    
    buffers[0]=&BITMAP_1;
    buffers[1]=&BITMAP_2;
    buffers[2]=&VIDEO_1;
    buffers[3]=&VIDEO_2;
    buffers[4]=&CURSOR_1;
    buffers[5]=&CURSOR_2;
    
#ifdef AV_SCREEN
    osdInit();
    
    /* reset everything */
    osdSetComponentConfig(AV3XX_OSD_VIDEO1,  0);
    osdSetComponentConfig(AV3XX_OSD_VIDEO2,  0);
    osdSetComponentConfig(AV3XX_OSD_BITMAP1, 0);
    osdSetComponentConfig(AV3XX_OSD_BITMAP2, 0);
    osdSetComponentConfig(AV3XX_OSD_CURSOR1, 0);
    osdSetComponentConfig(AV3XX_OSD_CURSOR2, 0);
    
    /*setting up planes */    
    iniComponent(BMAP1,&BITMAP_1,(unsigned int)&screen_BMAP1); 
    iniComponent(BMAP2,&BITMAP_2,(unsigned int)&screen_BMAP2); 
    iniComponent(VID1,&VIDEO_1,(unsigned int)&screen_VID1);
    iniComponent(VID2,&VIDEO_2,(unsigned int)&screen_VID2);
    
    setPalette(gui_pal,256);
#else
    BITMAP_1.offset=(unsigned int)&screen_BMAP1;
    BITMAP_2.offset=(unsigned int)&screen_BMAP2;
    VIDEO_1.offset=(unsigned int)&screen_VID1;
    VIDEO_2.offset=(unsigned int)&screen_VID2;
    

    
    display = XOpenDisplay(0);  
    if(!display) 
    {
            printf("Error while connecting to X server");
            exit(1);
    }
                
    screen = DefaultScreen(display);
    gc = DefaultGC(display, screen);
    
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
    XStoreName(display, window, "LinAV project");
    
    setPalette(gui_pal,256);
    
    /* initializing the tmp buffer and the screen */
    for(y=0; y<SCREEN_HEIGHT; y++)
        for(x=0; x<SCREEN_WIDTH; x++)
        {
            screen_BMAP1_SAV[y*SCREEN_WIDTH+x]=0;
            XSetForeground(display, gc, colorTab[0]);
            XDrawPoint(display, window, gc, x, y);
        }            
    
    XMapWindow(display, window);
    nxtEvent(); // call needed to force exposure event
#endif  
          
    gc_bmap1=createGC(BMAP1);
    gc_bmap2=createGC(BMAP2);
    gc_vid1=createGC(VID1);
    gc_vid2=createGC(VID2);
    
    listGC[0]=gc_bmap1;
    listGC[1]=gc_bmap2;
    listGC[2]=gc_vid1;
    listGC[3]=gc_vid2;
    
    if(iniEvent()<0)
        return -1;

          
    setPlane(BMAP1);
    showPlane(BMAP1);
        
    return 0;
}

#ifndef AV_SCREEN
void lcd_update(int type, int x_ini, int y_ini, int w, int h)
{
    int x, y;
    unsigned char color;
    
    for(y=y_ini; y<(y_ini+h); y++)
        for(x=x_ini; x<(x_ini+w); x++)
        {
            if(screen_BMAP1[y*SCREEN_WIDTH+x]!=screen_BMAP1_SAV[y*SCREEN_WIDTH+x] || type == FORCE_REDRAW)
            {
                color=screen_BMAP1[y*SCREEN_WIDTH+x];
                XSetForeground(display, gc, colorTab[color]);
                XDrawPoint(display, window, gc, x, y);
                screen_BMAP1_SAV[y*SCREEN_WIDTH+x]=screen_BMAP1[y*SCREEN_WIDTH+x];
            }
        }
}
#endif

void setPalette(int palette[256][3],int size)
{
#ifdef AV_SCREEN
    int i=0;
    int y,cr,cb;
    for(i=0;i<size;i++)
    {
        y = (306*palette[i][0] + 601*palette[i][1] + 117*palette[i][2]) >> 10 ; 
        cb = ((-173*palette[i][0] -339*palette[i][1] + 512*palette[i][2]) >> 10) + 128;
        cr = ((512*palette[i][0] - 429*palette[i][1] - 83*palette[i][2]) >> 10) + 128;
    
        osdSetPallette (y, cr, cb, i);
    }
#else
    int r,g,b,i;
    XColor c;
    Colormap pal = DefaultColormap(display,screen);

    for(i=0; i<size; i++)
    {
        r = palette[i][0];
        g = palette[i][1];
        b = palette[i][2];
        c.red = r*0x100+r;
        c.green = g*0x100+g;
        c.blue = b*0x100+b;
        XAllocColor(display, pal, &c);
        colorTab[i] = c.pixel;
    }
#endif
}

void close_graphics()
{
#ifdef AV_SCREEN
    int fd=open("/dev/fb0",O_WRONLY);
    if(fd<0)
        printf("error opening /dev/fb\n");
    if(ioctl(fd,FBIO_INIT,NULL)<0)
        fprintf(stderr,"error sending init ioctl\n");
#else
#endif
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
            gc->gops=&g32ops;
            gc->buffer=&VIDEO_1;
            break;
        case VID2:
            gc->gops=&g32ops;
            gc->buffer=&VIDEO_2;
            break;
        default:
            fprintf(stderr,"wrong plane %d\n",vplane);
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
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        default_gc=listGC[vplane];
    }
    else
    {
        fprintf(stderr,"wrong plane %d\n",vplane);
    } 
}

void hidePlane(int vplane)
{
#ifdef AV_SCREEN
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        osdSetComponentConfig(buffers_comp[vplane],0);
    }
    else
    {
        fprintf(stderr,"wrong plane %d\n",vplane);
    }    
#endif
}

void showPlane(int vplane)
{
#ifdef AV_SCREEN
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        osdSetComponentConfig(buffers_comp[vplane],buffers[vplane]->state|AV3XX_OSD_COMPONENT_ENABLE);
    }
    else
    {
        fprintf(stderr,"wrong plane %d\n",vplane);
    }
#endif          
}

void setState(int vplane,int state)
{
#ifdef AV_SCREEN
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        buffers[vplane]->state=state;
    }
    else
    {
        fprintf(stderr,"wrong plane %d\n",vplane);
    }  
#endif          
}

int getState(int vplane)
{
#ifdef AV_SCREEN
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        return buffers[vplane]->state;
    }
    else
    {
        fprintf(stderr,"wrong plane %d\n",vplane);
        return -1;
    }  
#endif          
}

void setSize(int vplane,int width,int height,int bitsPerPixel)
{
#ifdef AV_SCREEN
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        printf("processing setsize: %d,%d,%d for %d\n",width,height,bitsPerPixel,vplane);
        buffers[vplane]->width=width;
        buffers[vplane]->height=height;
        buffers[vplane]->bitsPerPixel=bitsPerPixel;
        osdSetComponentSize(buffers_comp[vplane], 2*width, height);
        osdSetComponentSourceWidth(buffers_comp[vplane], ((width*bitsPerPixel)/32)/8);
    }
    else
    {
        fprintf(stderr,"wrong plane %d\n",vplane);
    }  
#endif          
}

void getSize(int vplane,int * width,int * height,int * bitsPerPixel)
{
#ifdef AV_SCREEN
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        if(width)
            *width=buffers[vplane]->width;
        if(height)
            *height=buffers[vplane]->height;
        if(bitsPerPixel)
            *bitsPerPixel=buffers[vplane]->bitsPerPixel;
    }
    else
    {
        fprintf(stderr,"wrong plane %d\n",vplane);
    }  
#endif          
}

void setPos(int vplane,int x,int y)
{
#ifdef AV_SCREEN
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        buffers[vplane]->x=x;
        buffers[vplane]->y=y;
        osdSetComponentPosition(buffers_comp[vplane],x,y);        
    }
    else
    {
        fprintf(stderr,"wrong plane %d\n",vplane);
    }  
#endif          
}

void getPos(int vplane,int * x,int * y)
{
#ifdef AV_SCREEN
    if(vplane>=0 && vplane < NB_BUFFER)
    {
        if(x)
            *x=buffers[vplane]->x;
        if(y)
            *y=buffers[vplane]->y;
    }
    else
    {
        fprintf(stderr,"wrong plane %d\n",vplane);
    }  
#endif          
}

#ifdef AV_SCREEN
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
#endif

/* drawing functions */
void clearScreen(unsigned int color)
{
    default_gc->gops->fillRect(color,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,default_gc->buffer);
    LCD_UPDATE(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
}

void drawPixel(unsigned int color,int x, int y)
{
    tstXY(x,y);
    default_gc->gops->drawPixel(color, x, y,default_gc->buffer);
    LCD_UPDATE(x,y,1,1)
}

unsigned int readPixel(int x, int y)
{
    tstXY(x,y);
    return default_gc->gops->readPixel(x,y,default_gc->buffer);
}

void drawRect(unsigned int color, int x, int y, int width, int height)
{
    tstXY(x,y);
    tstWH(x,y,width,height);
    default_gc->gops->drawRect(color,x,y,width,height,default_gc->buffer);
    LCD_UPDATE(x,y,width,height)
}

void fillRect(unsigned int color, int x, int y, int width, int height)
{
    tstXY(x,y);
    tstWH(x,y,width,height);
    default_gc->gops->fillRect(color,x,y,width,height,default_gc->buffer);
    LCD_UPDATE(x,y,width,height)
}

void drawLine(unsigned int color, int x1, int y1, int x2, int y2)
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
        LCD_UPDATE(x1,y1,1,y2-y1+1)
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
        default_gc->gops->drawHLine(color,x1,y1,x2-x1+1,default_gc->buffer);
        LCD_UPDATE(x1,y1,x2-x1+1,1)
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
    
#ifndef AV_SCREEN
    if(x1 > x2)
    {
        xinc1=x1;
        xinc2=x2;
    }
    else
    {
        xinc1=x2;
        xinc2=x1;
    }
    
    if(y1 > y2)
    {
        yinc1=y1;
        yinc2=y2;
    }
    else
    {
        yinc1=y2;
        yinc2=y1;
    }
    
    LCD_UPDATE(xinc1,yinc1,xinc2-xinc1+1,yinc2-yinc1+1)
#endif    
}

void putS(unsigned int color, unsigned int bg_color, int x, int y, unsigned char *s)
{
    FONT_ID font=default_font;
    int len=strlen(s);
    int h=0,w=0;
    unsigned char c=0;

    tstXY(x,y);
    
        
    getStringS(s,&w,&h);
    
    if(y+h>SCREEN_HEIGHT)
    {
        printf("!!!!!!!!!! str going out of screen\n");
        return 0;
    }
    
    if(x+w>SCREEN_WIDTH)
    {
        
        c = s[(SCREEN_WIDTH-x)/font->width];
        s[(SCREEN_WIDTH-x)/font->width]=0;
        printf("!!!!!!!!!! string too long\n");        
    }

    default_gc->gops->drawString(font,color,bg_color,x,y,s,default_gc->buffer);

    if(c!=0)
    {
        s[(SCREEN_WIDTH-x)/font->width]=c;
    }
  
    LCD_UPDATE(x,y,w,h)
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

void putC(unsigned int color, unsigned int bg_color, int x, int y, unsigned char s)
{
    FONT_ID font=default_font;
    int h,w;

    tstXY(x,y);

    default_gc->gops->drawChar(font,color,bg_color,x,y,s,default_gc->buffer);
#ifndef AV_SCREEN
    getStringS("W",&w,&h);
    LCD_UPDATE(x,y,w,h)
#endif
}

void drawSprite(unsigned int * palette, SPRITE * sprite, int x, int y)
{
    tstXY(x,y);
    default_gc->gops->drawSprite(palette,sprite,default_gc->transparent,x,y,default_gc->buffer);
    LCD_UPDATE(x,y,sprite->width,sprite->height);
}

void drawBITMAP(BITMAP * bitmap, int x, int y)
{
    tstXY(x,y);
    default_gc->gops->drawBITMAP(bitmap,default_gc->transparent,x,y,default_gc->buffer);
    LCD_UPDATE(x,y,bitmap->width,bitmap->height);
}

void scrollWindowVert(unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP)
{
    default_gc->gops->scrollWindowVert(bgColor,x,y,width,height,scroll,UP,default_gc->buffer);
#ifndef AV_SCREEN
    if(UP)
        LCD_UPDATE(x,y-scroll,width,height)
    else
        LCD_UPDATE(x,y,width,height+scroll)
#endif
}

void scrollWindowHoriz(unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT)
{
    default_gc->gops->scrollWindowHoriz(bgColor,x,y,width,height,scroll,RIGHT,default_gc->buffer);
#ifndef AV_SCREEN
    if(RIGHT)
        LCD_UPDATE(x,y,width+scroll,height)
    else
        LCD_UPDATE(x-scroll,y,width,height)
#endif
}

/* images */

void drawImage(char * filename)
{
#ifdef HAVE_JPEG    
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * img_file;
    unsigned char * offset;
    struct graphicsBuffer * buff=&VIDEO_1;
    JSAMPROW rowptr[1];
    int scale[]={2,4,8};
    int i,j,x,y;
    unsigned int * screenDirect;
    
    /* Initialize the JPEG decompression object with default error handling. */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    
    if ((img_file = fopen(filename, "rb")) == NULL)
    {
        fprintf(stderr, "drawImage: can't open %s\n", filename);
        exit(EXIT_FAILURE);
    }
    
    jpeg_stdio_src(&cinfo, img_file);
    
    jpeg_read_header(&cinfo,TRUE);
    
    cinfo.out_color_space=JCS_CUST;
    
    if(cinfo.image_width > SCREEN_WIDTH || cinfo.image_height > SCREEN_HEIGHT)
    {
        for(i=0;i<3;i++)
            if((cinfo.image_width/scale[i])<SCREEN_WIDTH && (cinfo.image_height/scale[i])<SCREEN_HEIGHT)
                break;
        if(i==3)
        {
            fprintf(stderr, "drawImage: image too big %s\n", filename);
            exit(EXIT_FAILURE);
        }
        else
            cinfo.scale_denom=scale[i];
    }
 
    /* changes proposed by tgb */       
    cinfo.two_pass_quantize = FALSE;
    cinfo.dither_mode = JDITHER_NONE;
    cinfo.desired_number_of_colors = 256;
    cinfo.dct_method = JDCT_FASTEST;
    cinfo.do_fancy_upsampling = FALSE;
    /**********************************/
    
    jpeg_start_decompress(&cinfo);
    
    
    
    //cinfo.out_color_components=4;
    
    hidePlane(BMAP1);
    hidePlane(BMAP2);
    showPlane(VID1);
    
    screenDirect=(unsigned int *)buff->offset;
    for (j=0;j<SCREEN_HEIGHT;j++)
            for (i=0;i<SCREEN_WIDTH;i++)
                    screenDirect[j*SCREEN_WIDTH + i] = 0x00800080;
    
    x=(SCREEN_WIDTH-cinfo.output_width)/2;
    y=(SCREEN_HEIGHT-cinfo.output_height)/2;
    offset=(char*)(buff->offset+x*4+y*buff->width*4);
    
    while(cinfo.output_scanline < cinfo.output_height)
    {
        rowptr[0] = (JSAMPROW)offset;
        if(jpeg_read_scanlines(&cinfo, rowptr,1))
            offset+=buff->width*4;
    }
    
    
    
    jpeg_destroy_decompress(&cinfo);
    fclose(img_file);
#endif
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
