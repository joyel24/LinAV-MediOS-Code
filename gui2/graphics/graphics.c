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
#include <fcntl.h>
#include <sys/ioctl.h>

#include <osd.h>
#include <graphics.h>
#include "events.h"
#include "graphics_8.h"
#include "gui_pal.h"
#include "jpeglib.h"

#define STRING_MAXSIZE 200

#define FBIO_INIT               _IO ('F', 0x26)

#define tstXY(x,y)  {if(x>SCREEN_WIDTH) return; if(x<0) return; if(y>SCREEN_HEIGHT) return; if(y<0) return;}
#define tstWH(x,y,w,h)  {if(x+w>SCREEN_WIDTH)return; if(x+w<0) return; if(y+h>SCREEN_HEIGHT) return; if(y+h<0) return;}

char screen_BMAP1[SCREEN_WIDTH*SCREEN_HEIGHT+40];
char screen_BMAP2[SCREEN_WIDTH*SCREEN_HEIGHT+40];
char screen_VID1[SCREEN_WIDTH*SCREEN_HEIGHT*4+40];
char screen_VID2[SCREEN_WIDTH*SCREEN_HEIGHT*4+40];

struct graphicsBuffer BITMAP_1 = {
    offset             : 0,
    component          : AV3XX_OSD_BITMAP1,
    bytesPerLine       : SCREEN_WIDTH*2,
    width              : SCREEN_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 8,
    bitsPerPixelShift  : 3,
    SWidth             : 0xa,
    enable             : AV3XX_OSD_BITMAP_RAMCLUT | AV3XX_OSD_BITMAP_ZX1 |
                    AV3XX_OSD_BITMAP_8BIT | AV3XX_OSD_COMPONENT_ENABLE
};

struct graphicsBuffer BITMAP_2 = {
    offset             : 0,
    component          : AV3XX_OSD_BITMAP2,
    bytesPerLine       : SCREEN_WIDTH*2,
    width              : SCREEN_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 8,
    bitsPerPixelShift  : 3,
    SWidth             : 0xa,
    enable             : AV3XX_OSD_BITMAP_RAMCLUT | AV3XX_OSD_BITMAP_ZX1 |
                    AV3XX_OSD_BITMAP_8BIT | AV3XX_OSD_COMPONENT_ENABLE
};        

struct graphicsBuffer VIDEO_1 = {
    offset             : 0,
    component          : AV3XX_OSD_VIDEO1,
    bytesPerLine       : SCREEN_WIDTH*2,
    width              : SCREEN_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 32,
    bitsPerPixelShift  : 5,
    SWidth             : 0x28,
    enable             : AV3XX_OSD_COMPONENT_ENABLE
};

struct graphicsBuffer VIDEO_2 = {
    offset             : 0,
    component          : AV3XX_OSD_VIDEO2,
    bytesPerLine       : SCREEN_WIDTH*2,
    width              : SCREEN_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : 0x14,
    y                  : 0x12,
    bitsPerPixel       : 32,
    bitsPerPixelShift  : 5,
    SWidth             : 0x28,
    enable             : AV3XX_OSD_COMPONENT_ENABLE
};

GC_ID   default_gc=NULL;
FONT_ID default_font=GC_FONT;

GC_ID   gc_bmap1,gc_bmap2,gc_vid1,gc_vid2;

extern struct graphics_operations g8ops;

int ini_graphics()
{
    /* reset everything */
    osdSetComponentConfig(AV3XX_OSD_VIDEO1,  0);
    osdSetComponentConfig(AV3XX_OSD_VIDEO2,  0);
    osdSetComponentConfig(AV3XX_OSD_BITMAP1, 0);
    osdSetComponentConfig(AV3XX_OSD_BITMAP2, 0);
    osdSetComponentConfig(AV3XX_OSD_CURSOR1, 0);
    osdSetComponentConfig(AV3XX_OSD_CURSOR2, 0);
    
    iniComponent(&BITMAP_1,(unsigned int)&screen_BMAP1);    
    gc_bmap1=createGC(BMAP1);

    iniComponent(&BITMAP_2,(unsigned int)&screen_BMAP2);    
    gc_bmap2=createGC(BMAP2);

    iniComponent(&VIDEO_1,(unsigned int)&screen_VID1);    
    gc_vid1=createGC(VID1);
        
    iniComponent(&VIDEO_2,(unsigned int)&screen_VID2);    
    gc_vid2=createGC(VID2);
    
    if(iniEvent()<0)
        return -1;

    setPalette(gui_pal,256);
            
    setPlane(BMAP1);
    showPlane(BMAP1);
        
    return 0;
}

void setPalette(int palette[256][3],int size)
{
    int i=0;
    int y,cr,cb;
    for(i=0;i<size;i++)
    {
        y = (306*palette[i][0] + 601*palette[i][1] + 117*palette[i][2]) >> 10 ; 
        cb = ((-173*palette[i][0] -339*palette[i][1] + 512*palette[i][2]) >> 10) + 128;
        cr = ((512*palette[i][0] - 429*palette[i][1] - 83*palette[i][2]) >> 10) + 128;
    
        osdSetPallette (y, cr, cb, i);
    }
}

void close_graphics()
{
    int fd=open("/dev/fb0",O_WRONLY);
    if(fd<0)
        printf("error opening /dev/fb\n");
    if(ioctl(fd,FBIO_INIT,NULL)<0)
        fprintf(stderr,"error sending init ioctl\n");
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
    switch(vplane) {
        case BMAP1:
            osdSetComponentConfig(AV3XX_OSD_BITMAP1, 0);
            break;
        case BMAP2:
            osdSetComponentConfig(AV3XX_OSD_BITMAP2, 0);
            break;
        case VID1:
            osdSetComponentConfig(AV3XX_OSD_VIDEO1, 0);
            break;
                case VID2:
            osdSetComponentConfig(AV3XX_OSD_VIDEO2, 0);
            break;
        default:
            fprintf(stderr,"wrong plane\n");
    }
}

void showPlane(int vplane)
{
    switch(vplane) {
        case BMAP1:
            //tstPlane(&BITMAP_1);
            osdSetComponentConfig(AV3XX_OSD_BITMAP1,BITMAP_1.enable);
            break;
        case BMAP2:
            //tstPlane(&BITMAP_2);
            osdSetComponentConfig(AV3XX_OSD_BITMAP2,BITMAP_2.enable);
            break;
        case VID1:
            //tstPlane(&VIDEO_1);
            osdSetComponentConfig(AV3XX_OSD_VIDEO1,VIDEO_1.enable);
            break;
        case VID2:
            //tstPlane(&VIDEO_2);
            osdSetComponentConfig(AV3XX_OSD_VIDEO2,VIDEO_2.enable);
            break;
        default:
            fprintf(stderr,"wrong plane\n");
    }            
}

/*void tstPlane(struct graphicsBuffer * plane)
{
    int diff;
    if(plane->offset == 0)
    {
        plane->offset=(unsigned int)malloc(sizeof(char)*(SCREEN_WIDTH*SCREEN_HEIGHT*(plane->bitsPerPixel>>3)+40));
            if(plane->offset)
            {
                diff=plane->offset % 32;
                if(diff)
                        plane->offset+=(32-diff);
                osdSetComponentOffset(plane->component, plane->offset);
            }
            else
            {
                fprintf(stderr,"Can't allocate buffer for new Plane");
                _exit(0);
            }
    }
}
*/

void iniComponent(struct graphicsBuffer * buff,unsigned int offset)
{    
    int diff=offset % 32;
    if(diff)
        offset+=(32-diff);
    buff->offset=offset;
    osdSetComponentOffset(buff->component,offset);
    osdSetComponentSize(buff->component, buff->bytesPerLine, buff->height);
    osdSetComponentPosition(buff->component,buff->x, buff->y);
    osdSetComponentSourceWidth(buff->component, buff->SWidth);
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

/* images */

void drawImage(char * filename)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE * img_file;
    char * offset;
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