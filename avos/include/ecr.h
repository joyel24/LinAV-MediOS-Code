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

extern u32 ecrSwapBytes(u32 value);
extern u32 ecrSwapBits(u32 value);
extern void ecrSwapBytes(char* bufferIn, char* bufferOut, u32 count);
extern void ecrSwapBits(char* bufferIn, char* bufferOut, u32 count);

