/*
* arch/armnommu/mach-av3xx/av3xx_graphics.c
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
* Whats done?       Destinateion bits per pixel
*              |  1 |  2 |  4 |  8 | 16 | 32 |
* -------------+----+----+----+----+----+----+
* setPixel     |    |    |    |  Y |  Y |  Y |
* getPixel     |    |    |    |  Y |  Y |  Y |
* boxf         |    |    |    |  Y |  Y |  Y |
* sprite1      |    |    |    |    |  Y |  Y |
* sprite2      |    |    |    |    |  Y |  Y |
* sprite4      |    |    |    |    |  Y |  Y |
* sprite8      |    |    |    |    |  Y |  Y |
*
* sprite16     |    |    |    |    |  Y |    |
* sprite32     |    |    |    |    |    |  Y |
*
*
* NB 16bit per pixel is actually kindof 8 bit per pixel.
* it's just 2* 8 bit pixels side by side. This is used when hardware zoom is
* set to x2
* Also, 32 bit is actually likewise 2 pixels. Y0 Cb Y1 Cr.
* This is again used for hardware zoom x2/x4.
*
*/

#include <asm/io.h>
#include <asm/types.h>
#include <asm/arch/hardware.h>
#include <asm/arch/av3xx_graphics.h>


void (*graphicsRoutines[5][6]) = {
         { graphics1SetPixel,    graphics2SetPixel,
           graphics4SetPixel,    graphics8SetPixel,
           graphics16SetPixel,   graphics32SetPixel
		 },
         {
           graphics1GetPixel,    graphics2GetPixel,
           graphics4GetPixel,    graphics8GetPixel,
           graphics16GetPixel,   graphics32GetPixel
		 },
         {
           graphics1Boxf,       graphics2Boxf,
           graphics4Boxf,       graphics8Boxf,
           graphics16Boxf,      graphics32Boxf
		 },
         {
           graphics1Sprite,      graphics2Sprite,
           graphics4Sprite,      graphics8Sprite,
           graphics16Sprite,     graphics32Sprite
		 },
		 {
		   graphics1ScrollUP,      graphics2ScrollUP,
           graphics4ScrollUP,      graphics8ScrollUP,
           graphics16ScrollUP,     graphics32ScrollUP
		 }
};


int graphicsRGB2Packed(int r, int g, int b)
{
/*	int y = (int) ((0.2989 * r) + (0.5866 * g) + (0.1145 * b));
	int Cr = (int) (128 + 0.7132 * (r - y));
	int Cb = (int) (128 + 0.5647 * (b - y));
	return (Cr << 16) + (y << 8) + Cb; */

	int y = (306*r + 601*g + 117*b) >> 10 ; // +16 ??
	int cb = ((-173*r -339*g + 512*b) >> 10) + 128;
	int cr = ((512*r - 429*g - 83*b) >> 10) + 128;

	return  (cr << 16) | (y << 8) | cb;
}


u32 graphicsGetOffset(struct graphicsBuffer * buffer, u32 x, u32 y)
{
	y=y*buffer->bytesPerLine+buffer->offset;
	x*=(buffer->bitsPerPixel)>>3;
	return x+y;
}

void * graphicsFindRoutine(struct graphicsBuffer * buffer, int routineN)
{
	return graphicsRoutines[routineN][buffer->bitsPerPixelShift];
}

void graphicsSetPixel(struct graphicsBuffer * buffer, u32 x, u32 y, int c)
{
	void (*routine)(u32 x, u32 y, int c)=graphicsFindRoutine(buffer,AV3XX_GRAPHICS_ROUTINE_SETPIXEL);
	routine(x,y,c);
}

int graphicsGetPixel(struct graphicsBuffer * buffer, u32 x, u32 y)
{
	int (*routine)(u32 x, u32 y)=graphicsFindRoutine(buffer,AV3XX_GRAPHICS_ROUTINE_GETPIXEL);
	return routine(x,y);
}

void graphicsBoxf(struct graphicsBuffer * buffer, u32 x, u32 y, u32 width, u32 height, int c)
{
	void (*routine)(struct graphicsBuffer * buffer,u32 x, u32 y, u32 width, u32 height, int c)=
			graphicsFindRoutine(buffer,AV3XX_GRAPHICS_ROUTINE_BOXF);
	routine(buffer,x,y,width,height,c);
}

void graphicsSprite(struct graphicsBuffer * buffer, u32 x, u32 y, struct graphicsBuffer *buffSrc)
{
	void (*routine)(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc)=
			graphicsFindRoutine(buffer,AV3XX_GRAPHICS_ROUTINE_SPRITE);
	routine(buffer,x,y,buffSrc);
}

void graphicsString(struct graphicsBuffer *buffDest, u32 x, u32 y,
                           struct graphicsBuffer *font, char** fontlut, u32 dx, u32 dy,char *s)
{

	while (*s)
	{
		int c= (int)fontlut[(int)*s];
		if(c!=0)
		{
			font->offset=c;
			graphicsSprite(buffDest,x,y,font);
		}
		x+=dx;
		y+=dy;
		s++;
	}
}

void graphicsChar(struct graphicsBuffer *buffDest, u32 x, u32 y,
                           struct graphicsBuffer *font, char** fontlut, char s)
{
	if(s!=0)
	{
		int c= (int)fontlut[(int)s];
		if(c!=0)
		{
			font->offset=c;
			graphicsSprite(buffDest,x,y,font);
		}
	}
}

void graphicsScrollUP(struct graphicsBuffer *buffDest,int Hup)
{
	void (*routine)(struct graphicsBuffer * buffDest,u32 Hup)=
			graphicsFindRoutine(buffDest,AV3XX_GRAPHICS_ROUTINE_SCROLLUP);
	routine(buffDest,Hup);
}

/**  graphics 1 **/

#include "graphics/graphics1.c"

/**  graphics 2 **/

#include "graphics/graphics2.c"

/**  graphics 4 **/

#include "graphics/graphics4.c"

/**  graphics 8 **/

#include "graphics/graphics8.c"

/**  graphics 16 **/

#include "graphics/graphics16.c"

/**  graphics 32 **/

#include "graphics/graphics32.c"
