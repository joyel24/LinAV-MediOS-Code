/* libavos.h
   By William Bland (aka awksedgrep)
   Copyright 2004, the Avos project.

   This file is free software; we give unlimited permission to copy
   and/or distribute it, with or without modifications, as long as this
   notice is preserved.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY, to the extent permitted by law; without
   even the implied warranty of MERCHANTABILITY or FITNESS FOR A
   PARTICULAR PURPOSE.
*/
#ifndef u32_defined
#define u32_defined yes
typedef unsigned long u32;
#endif

struct graphicsBuffer {
    u32 offset;                 // ->The data
    u32 bytesPerLine;
    u32 width;
    u32 height;
    u32 bitsPerPixel;           // eg 32 = YCbCr color
    u32 bitsPerPixelShift;      // eg  5 = YCbCr color
    u32 transparent;            // Color index that shall be transparent
    int (*pallette1) [];        // Pallette (index->1bit)
    int (*pallette2) [];        // Pallette (index->2bit)
    int (*pallette4) [];        // Pallette (index->4bit)
    int (*pallette8) [];        // Pallette (index->8bit)
    int (*pallette16) [];       // Pallette (index->16bit)
    int (*pallette32) [];       // Pallette (index->YCbCr)
};


extern void graphicsSetPixel(struct graphicsBuffer *buffDef, u32 x, u32 y, u32 c);
extern u32 graphicsGetPixel(struct graphicsBuffer *buffDef, u32 x, u32 y);
extern void graphicsBoxf(struct graphicsBuffer *buffDef, u32 x, u32 y, u32 width, u32 height, u32 c);
extern void graphicsSprite(struct graphicsBuffer *buffDest, u32 x, u32 y, struct graphicsBuffer *buffSrc);
extern void graphicsString(struct graphicsBuffer *buffDest, u32 x, u32 y,
                           struct graphicsBuffer *font, char (*fontlut[]) [], u32 dx, u32 dy,
                           char *s);

extern void graphics32SetPixel(struct graphicsBuffer *buffDef, u32 x, u32 y, u32 c);
extern u32 graphics32GetPixel(struct graphicsBuffer *buffDef, u32 x, u32 y);
extern void graphics32Boxf(struct graphicsBuffer *buffDef, u32 x, u32 y, u32 width, u32 height, u32 c);


