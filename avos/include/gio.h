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

#define         GIO_LINE_BUTTON_ON  0x00
#define         GIO_LINE_I2C_DATA   0x12
#define         GIO_LINE_I2C_CLOCK  0x13
#define         GIO_LINE_BUTTON_OFF 0x1c

#define         GIO_OUTPUT       0
#define         GIO_INPUT        1
#define         GIO_UNINVERTED   0
#define         GIO_INVERTED     1

void gioSetAllDirectionsA(u32 directions);
u32 gioGetAllDirectionsA();
void gioSetAllInvertsA(u32 inverts);
u32 gioGetAllInvertsA();
void gioSetAllBitsetsA(u32 bitsets);
u32 gioGetAllBitsetsA();
void gioSetAllBitclearsA(u32 bitclears);
u32 gioGetAllBitclearsA();

void gioSetBitA(u32 line);
void gioClearBitA(u32 line);
u32 gioGetBitA(u32 line);

