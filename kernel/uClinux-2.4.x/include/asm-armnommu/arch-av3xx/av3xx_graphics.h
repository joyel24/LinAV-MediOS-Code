/*
* asm/arch/av3xx_graphics.h
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

#ifndef __ASM_ARCH_AV3XX_GRAPHICS_H
#define __ASM_ARCH_AV3XX_GRAPHICS_H

#include <asm/types.h>

#define     AV3XX_GRAPHICS_ROUTINE_SETPIXEL       0x0000
#define     AV3XX_GRAPHICS_ROUTINE_GETPIXEL       0x0001
#define     AV3XX_GRAPHICS_ROUTINE_BOXF           0x0002
#define     AV3XX_GRAPHICS_ROUTINE_SPRITE         0x0003
#define     AV3XX_GRAPHICS_ROUTINE_SCROLLUP       0x0004

struct graphicsBuffer {
    u32 offset;                 // ->The data
    u32 bytesPerLine;
    u32 width;
    u32 height;
    u32 bitsPerPixel;           // eg 32 = YCbCr color
    u32 bitsPerPixelShift;      // eg  5 = YCbCr color
    u32 transparent;            // Color index that shall be transparent
    int **pallette1;            // Pallette (index->1bit)
    int **pallette2;            // Pallette (index->2bit)
    int **pallette4;            // Pallette (index->4bit)
    int **pallette8;            // Pallette (index->8bit)
    int **pallette16;           // Pallette (index->16bit)
    int **pallette32;           // Pallette (index->YCbCr)
};

int graphicsRGB2Packed(int r, int g, int b);
u32 graphicsGetOffset(struct graphicsBuffer * buffer, u32 x, u32 y);
void * graphicsFindRoutine(struct graphicsBuffer * buffer, int routineN);
void graphicsSetPixel(struct graphicsBuffer * buffer, u32 x, u32 y, int c);
int graphicsGetPixel(struct graphicsBuffer * buffer, u32 x, u32 y);
void graphicsBoxf(struct graphicsBuffer * buffer, u32 x, u32 y, u32 width, u32 height, int c);
void graphicsSprite(struct graphicsBuffer * buffer, u32 x, u32 y, struct graphicsBuffer *buffSrc);
void graphicsString(struct graphicsBuffer *buffDest, u32 x, u32 y,
                           struct graphicsBuffer *font, char** fontlut, u32 dx, u32 dy,char *s);
void graphicsChar(struct graphicsBuffer *buffDest, u32 x, u32 y,
                           struct graphicsBuffer *font, char** fontlut, char s);
void graphicsScrollUP(struct graphicsBuffer *buffDest,int Hup);

void graphics1SetPixel(struct graphicsBuffer * buffer,u32 x, u32 y, u32 c);
u32  graphics1GetPixel(struct graphicsBuffer * buffer,u32 x, u32 y);
void graphics1Boxf(struct graphicsBuffer * buffer,u32 x, u32 y, u32 width, u32 height, u32 c);
void graphics1Sprite(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc);
void graphics1String(struct graphicsBuffer * buffer,u32 x, u32 y,struct graphicsBuffer *font,
					char** fontlut, u32 dx, u32 dy,char *s);
void graphics1ScrollUP(struct graphicsBuffer *buffDest,int Hup);

void graphics2SetPixel(struct graphicsBuffer * buffer,u32 x, u32 y, u32 c);
u32  graphics2GetPixel(struct graphicsBuffer * buffer,u32 x, u32 y);
void graphics2Boxf(struct graphicsBuffer * buffer,u32 x, u32 y, u32 width, u32 height, u32 c);
void graphics2Sprite(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc);
void graphics2String(struct graphicsBuffer * buffer,u32 x, u32 y,struct graphicsBuffer *font,
					char** fontlut, u32 dx, u32 dy,char *s);
void graphics2ScrollUP(struct graphicsBuffer *buffDest,int Hup);

void graphics4SetPixel(struct graphicsBuffer * buffer,u32 x, u32 y, u32 c);
u32  graphics4GetPixel(struct graphicsBuffer * buffer,u32 x, u32 y);
void graphics4Boxf(struct graphicsBuffer * buffer,u32 x, u32 y, u32 width, u32 height, u32 c);
void graphics4Sprite(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc);
void graphics4String(struct graphicsBuffer * buffer,u32 x, u32 y,struct graphicsBuffer *font,
					char** fontlut, u32 dx, u32 dy,char *s);
void graphics4ScrollUP(struct graphicsBuffer *buffDest,int Hup);

void graphics8SetPixel(struct graphicsBuffer * buffer,u32 x, u32 y, u32 c);
u32  graphics8GetPixel(struct graphicsBuffer * buffer,u32 x, u32 y);
void graphics8Boxf(struct graphicsBuffer * buffer,u32 x, u32 y, u32 width, u32 height, u32 c);
void graphics8Sprite(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc);
void graphics8String(struct graphicsBuffer * buffer,u32 x, u32 y,struct graphicsBuffer *font,
					char** fontlut, u32 dx, u32 dy,char *s);
void graphics8ScrollUP(struct graphicsBuffer *buffDest,int Hup);

void graphics16SetPixel(struct graphicsBuffer * buffer,u32 x, u32 y, u32 c);
u32  graphics16GetPixel(struct graphicsBuffer * buffer,u32 x, u32 y);
void graphics16Boxf(struct graphicsBuffer * buffer,u32 x, u32 y, u32 width, u32 height, u32 c);
void graphics16Sprite(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc);
void graphics16String(struct graphicsBuffer * buffer,u32 x, u32 y,struct graphicsBuffer *font,
					char** fontlut, u32 dx, u32 dy,char *s);
void graphics16ScrollUP(struct graphicsBuffer *buffDest,int Hup);

void graphics32SetPixel(struct graphicsBuffer * buffer,u32 x, u32 y, u32 c);
u32  graphics32GetPixel(struct graphicsBuffer * buffer,u32 x, u32 y);
void graphics32Boxf(struct graphicsBuffer * buffer,u32 x, u32 y, u32 width, u32 height, u32 c);
void graphics32Sprite(struct graphicsBuffer * buffer,u32 x, u32 y, struct graphicsBuffer *buffSrc);
void graphics32String(struct graphicsBuffer * buffer,u32 x, u32 y,struct graphicsBuffer *font,
					char** fontlut, u32 dx, u32 dy,char *s);
void graphics32ScrollUP(struct graphicsBuffer *buffDest,int Hup);

#endif
