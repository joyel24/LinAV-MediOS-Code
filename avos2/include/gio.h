/* gio.h
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

#define         GIO_LINE_BUTTON_ON          0x00

#define         GIO_LINE_MAS_EOD            0x04

#define         GIO_LINE_MAS_D0             0x08
#define         GIO_LINE_MAS_D1             0x09
#define         GIO_LINE_MAS_D2             0x0a
#define         GIO_LINE_MAS_D3             0x0b
#define         GIO_LINE_MAS_D4             0x0c
#define         GIO_LINE_MAS_D5             0x0d
#define         GIO_LINE_MAS_D6             0x0e
#define         GIO_LINE_MAS_D7             0x0f

#define         GIO_LINE_MAS_POWER          0x10    // 1 = power

#define         GIO_LINE_I2C_DATA           0x12
#define         GIO_LINE_I2C_CLOCK          0x13
#define         GIO_LINE_MODULE_SENSE       0x15    // 0 if CF card in
#define         GIO_LINE_MEMCARD_INSERT     0x16    // goes high when card
                                                    // inserted then goes
                                                    // back to low.
#define         GIO_LINE_BACKLIGHT_POWER    0x17    // 0 = power
#define         GIO_LINE_BUTTON_OFF         0x1c
#define         GIO_LINE_EXT_VIDEO          0x1d
#define         GIO_LINE_MAS_RTR            0x1e
#define         GIO_LINE_MAS_PR             0x1f

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
void gioSetAllIRQsA(u32 bitset);
void gioSetBitA(u32 line);
void gioClearBitA(u32 line);
u32 gioGetBitA(u32 line);

