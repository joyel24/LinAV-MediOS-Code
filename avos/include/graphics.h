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
    u32 offset;
    u32 bytesPerLine;
    u32 width;
    u32 height;
    u32 bitsPerPixel;
};


extern void graphicsSetPixel(struct graphicsBuffer *buffDef, u32 x, u32 y, u32 c);
extern u32 graphicsGetPixel(struct graphicsBuffer *buffDef, u32 x, u32 y);
extern void graphicsBoxf(struct graphicsBuffer *buffDef, u32 x, u32 y, u32 width, u32 height, u32 c);

extern void graphics32SetPixel(struct graphicsBuffer *buffDef, u32 x, u32 y, u32 c);
extern u32 graphics32GetPixel(struct graphicsBuffer *buffDef, u32 x, u32 y);
extern void graphics32Boxf(struct graphicsBuffer *buffDef, u32 x, u32 y, u32 width, u32 height, u32 c);


