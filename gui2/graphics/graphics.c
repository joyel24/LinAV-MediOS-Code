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

#define FBIO_INIT               _IO ('F', 0x26)
/* config functions */

char screen1[320*240+40];
char screen2[320*240+40];
struct graphicsBuffer BITMAP_1;
struct graphicsBuffer BITMAP_2;

GC_ID   default_gc=NULL;
FONT_ID default_font=GC_FONT;

GC_ID   gc_bmap1,gc_bmap2;

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
	BITMAP_1.bitsPerPixelShift = 3;
	
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
	
	diff=BITMAP_2.offset % 32;
	if(diff)
		BITMAP_2.offset+=(32-diff);	
	
	iniComponent(&BITMAP_2);
	
	gc_bmap2=createGC(BMAP2);
	
	if(iniEvent()<0)
		return -1;

	hidePlane(BMAP2);
	
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
		default:
			printf("wrong plane\n");
	}			
}

void showPlane(int vplane)
{
	switch(vplane) {
		case BMAP1:
			osdSetComponentConfig(AV3XX_OSD_BITMAP1,  AV3XX_OSD_BITMAP_RAMCLUT | AV3XX_OSD_BITMAP_ZX1 |
                                 AV3XX_OSD_BITMAP_8BIT | AV3XX_OSD_COMPONENT_ENABLE);
			break;
		case BMAP2:
			osdSetComponentConfig(AV3XX_OSD_BITMAP2,  AV3XX_OSD_BITMAP_RAMCLUT | AV3XX_OSD_BITMAP_ZX1 |
                                 AV3XX_OSD_BITMAP_8BIT | AV3XX_OSD_COMPONENT_ENABLE);
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
	osdSetComponentSourceWidth(buff->component, 0xa);
	osdSetComponentConfig(buff->component, AV3XX_OSD_BITMAP_RAMCLUT | AV3XX_OSD_BITMAP_ZX1 |
                                 AV3XX_OSD_BITMAP_8BIT | AV3XX_OSD_COMPONENT_ENABLE);
}

/* drawing functions */

void drawPixel(int color,int x, int y)
{
	default_gc->gops->drawPixel(color, x, y,default_gc->buffer);
}

int readPixel(int x, int y)
{
	return default_gc->gops->readPixel(x,y,default_gc->buffer);
}

void drawRect(int color, int x, int y, int width, int height)
{
	default_gc->gops->drawRect(color,x,y,width,height,default_gc->buffer);
}

void fillRect(int color, int x, int y, int width, int height)
{
	default_gc->gops->fillRect(color,x,y,width,height,default_gc->buffer);
}

void putS(int color, int bg_color, int x, int y, char *s)
{
	FONT_ID font=default_font;
	
	while (*s)
	{
		default_gc->gops->drawChar(font,color,bg_color,x,y,*s,default_gc->buffer);
		x+=font->dx;
		y+=font->dy;
		s++;
	}
}

void putC(int color, int bg_color, int x, int y, char s)
{
	FONT_ID font=default_font;
	
	default_gc->gops->drawChar(font,color,bg_color,x,y,s,default_gc->buffer);
}

void drawSprite(unsigned int * palette, SPRITE * sprite, int x, int y)
{
	
	default_gc->gops->drawSprite(sprite,palette,default_gc->transparent,x,y,default_gc->buffer);
}

void drawBITMAP(BITMAP * bitmap, int x, int y)
{
	default_gc->gops->drawBITMAP(bitmap,default_gc->transparent,x,y,default_gc->buffer);
}

/* font */
void setFont(FONT_ID font)
{
	default_font=font;
}

/**  graphics 8 **/

#include "graphics_8.c"
