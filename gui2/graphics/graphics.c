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
#include <gui_pal.h>
#include "jpeglib.h"

#define STRING_MAXSIZE 200

#define FBIO_INIT               _IO ('F', 0x26)

#define tstXY(x,y)  {if(x>320) return; if(x<0) return; if(y>240) return; if(y<0) return;}
#define tstWH(x,y,w,h)  {if(x+w>320)return; if(x+w<0) return; if(y+h>240) return; if(y+h<0) return;}

char screen1[320*240+40];
char screen2[320*240+40];
char screen3[320*240*4+40];
struct graphicsBuffer BITMAP_1;
struct graphicsBuffer BITMAP_2;
struct graphicsBuffer VIDEO_1;

GC_ID   default_gc=NULL;
FONT_ID default_font=GC_FONT;

GC_ID   gc_bmap1,gc_bmap2,gc_vid1;

int ini_graphics()
{
	int diff;
	/* reset everything */
	osdSetComponentConfig(AV3XX_OSD_VIDEO1,  0);
	osdSetComponentConfig(AV3XX_OSD_VIDEO2,  0);
	osdSetComponentConfig(AV3XX_OSD_BITMAP1, 0);
	osdSetComponentConfig(AV3XX_OSD_BITMAP2, 0);
	osdSetComponentConfig(AV3XX_OSD_CURSOR1, 0);
	osdSetComponentConfig(AV3XX_OSD_CURSOR2, 0);
		
	BITMAP_1.offset            = (int)&screen1;
	BITMAP_1.component         = AV3XX_OSD_BITMAP1;
	BITMAP_1.bytesPerLine      = 320*2;
	BITMAP_1.width             = 320;
	BITMAP_1.height            = 240;
	BITMAP_1.x                 = 0x14;
	BITMAP_1.y                 = 0x12;
	BITMAP_1.bitsPerPixel      = 8;
	BITMAP_1.SWidth            = 0xa;
	BITMAP_1.enable            = AV3XX_OSD_BITMAP_RAMCLUT | AV3XX_OSD_BITMAP_ZX1 |
                                     AV3XX_OSD_BITMAP_8BIT | AV3XX_OSD_COMPONENT_ENABLE;
	
	diff=BITMAP_1.offset % 32;
	if(diff)
		BITMAP_1.offset+=(32-diff);	
	
	iniComponent(&BITMAP_1);
	
	gc_bmap1=createGC(BMAP1);
	setPlane(BMAP1);
	
	BITMAP_2.offset            = (int)&screen2;
	BITMAP_2.component         = AV3XX_OSD_BITMAP2;
	BITMAP_2.bytesPerLine      = 320*2;
	BITMAP_2.width             = 320;
	BITMAP_2.height            = 240;
	BITMAP_2.x                 = 0x14;
	BITMAP_2.y                 = 0x12;
	BITMAP_2.bitsPerPixel      = 8;
	BITMAP_2.bitsPerPixelShift = 3;
	BITMAP_2.SWidth            = 0xa;
	BITMAP_2.enable            = AV3XX_OSD_BITMAP_RAMCLUT | AV3XX_OSD_BITMAP_ZX1 |
                                     AV3XX_OSD_BITMAP_8BIT | AV3XX_OSD_COMPONENT_ENABLE;
	
	diff=BITMAP_2.offset % 32;
	if(diff)
		BITMAP_2.offset+=(32-diff);	
	
	iniComponent(&BITMAP_2);
	
	gc_bmap2=createGC(BMAP2);
	
	VIDEO_1.offset            = (int)&screen3;
	VIDEO_1.component         = AV3XX_OSD_VIDEO1;
	VIDEO_1.bytesPerLine      = 320*2;
	VIDEO_1.width             = 320;
	VIDEO_1.height            = 240;
	VIDEO_1.x                 = 0x14;
	VIDEO_1.y                 = 0x12;
	VIDEO_1.bitsPerPixel      = 32;
	VIDEO_1.bitsPerPixelShift = 5;
	VIDEO_1.SWidth            = 0x28;
	VIDEO_1.enable            = AV3XX_OSD_COMPONENT_ENABLE;
	
	diff=VIDEO_1.offset % 32;
	if(diff)
		VIDEO_1.offset+=(32-diff);	
	
	iniComponent(&VIDEO_1);
	
	gc_vid1=createGC(VID1);
	
	if(iniEvent()<0)
		return -1;

	hidePlane(BMAP2);
	hidePlane(VID1);
	
	setPalette(gui_pal,256);
	
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
		printf("error sending init ioctl\n");
}

GC_ID createGC(int vplane)
{
	struct graphics_context * gc=(struct graphics_context *) malloc(sizeof(struct graphics_context));
	
	if(!gc)
	{
		printf("can't allocate GC\n");
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
		default:
			printf("wrong plane\n");
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
		default:
			printf("wrong plane\n");
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
		default:
			printf("wrong plane\n");
	}			
}

void showPlane(int vplane)
{
	switch(vplane) {
		case BMAP1:
			osdSetComponentConfig(AV3XX_OSD_BITMAP1,BITMAP_1.enable);
			break;
		case BMAP2:
			osdSetComponentConfig(AV3XX_OSD_BITMAP2,BITMAP_2.enable);
			break;
		case VID1:
			osdSetComponentConfig(AV3XX_OSD_VIDEO1,VIDEO_1.enable);
			break;
		default:
			printf("wrong plane\n");
	}			
}

void iniComponent(struct graphicsBuffer * buff)
{
	osdSetComponentSize(buff->component, buff->bytesPerLine, buff->height);
	osdSetComponentPosition(buff->component,buff->x, buff->y);
	osdSetComponentOffset(buff->component, buff->offset);
	osdSetComponentSourceWidth(buff->component, buff->SWidth);
	osdSetComponentConfig(buff->component, buff->enable);
}

/* drawing functions */

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

	/*if(font->width*len>STRING_MAXSIZE)
		s[STRING_MAXSIZE/font->width]=0;*/
	if(len>30)
	{
		c=s[30];
		s[30]=0;
	}

	default_gc->gops->drawString(font,color,bg_color,x,y,s,default_gc->buffer);

	if(len>30)
		s[30]=c;
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
	default_gc->gops->drawSprite(sprite,palette,default_gc->transparent,x,y,default_gc->buffer);
}

void drawBITMAP(BITMAP * bitmap, int x, int y)
{
	tstXY(x,y);
	default_gc->gops->drawBITMAP(bitmap,default_gc->transparent,x,y,default_gc->buffer);
}

void scrollWindowVert(int bgColor, int x, int y, int width, int height, int scroll, int UP)
{
	/*if(UP)
	{
		if(x>320) x=320; if(x<0) x=0; if(y>240) y=240; if((y-scroll)<0) y=scroll;		
		tstWH(x,y,width,height);
	}
	else
	{
		tstXY(x,y);
		if(x+width>320) width=320-x; if(x+width<0) width=-x; if((y+height+scroll)>240) height=240-y-scroll; if(y+height<0) height=-y;
	}*/
	default_gc->gops->scrollWindowVert(bgColor,x,y,width,height,scroll,UP,default_gc->buffer);
}

void scrollWindowHoriz(int bgColor, int x, int y, int width, int height, int scroll, int RIGHT)
{
	/*if(RIGHT)
	{
		tstXY(x,y);
		if((x+width+scroll)>320) width=320-x-scroll; if(x+width<0) width=-x; if((y+height)>240) height=240-y; if(y+height<0) height=-y;
	}
	else
	{
		if(x>320) x=320; if(x-scroll<0) x=scroll; if(y>240) y=240; if((y-scroll)<0) y=scroll;
		tstWH(x,y,width,height);
		
	}*/
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
	
	if(cinfo.image_width > 320 || cinfo.image_height > 240)
	{
		for(i=0;i<3;i++)
			if((cinfo.image_width/scale[i])<320 && (cinfo.image_height/scale[i])<240)
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
	for (j=0;j<240;j++)
        	for (i=0;i<320;i++)            
            		screenDirect[j*320 + i] = 0x00800080;
	
	x=(320-cinfo.output_width)/2;
	y=(240-cinfo.output_height)/2;
	offset=buff->offset+x*4+y*buff->width*4;
	
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

/**  graphics 8 **/

#include "graphics_8.c"
