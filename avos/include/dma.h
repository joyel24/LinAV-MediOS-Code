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

#define DMA_DEV_CS0     0
#define DMA_DEV_CS1     1       // ATA
#define DMA_DEV_CS2     2
#define DMA_DEV_CS3     3
#define DMA_DEV_CS4     4
#define DMA_DEV_SDRAM   5       // SDRAM

#define DMA_ENDIAN_3210 0       // normal
#define DMA_ENDIAN_2301 0x100
#define DMA_ENDIAN_0123 0x200

extern void dmaSetSourceA(void* address);
extern void dmaSetDestA(void* address);
extern void dmaSetSizeA(u32 size);
extern void dmaDevSelectA(u32 devSource, u32 devDest);
extern void dmaStartA(u32 endianness);
extern u32 dmaIsRunningA();
