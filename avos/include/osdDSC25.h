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

#define        OSD_VIDEO1          0
#define        OSD_VIDEO2          1
#define        OSD_BITMAP1         2
#define        OSD_BITMAP2         3
#define        OSD_CURSOR1         4
#define        OSD_CURSOR2         5

#define        OSD_MAIN_BORDER_RAMCLUT 0x01
#define        OSD_MAIN_VIDEO_SCALEX   0x04
#define        OSD_MAIN_VIDEO_SCALEY   0x08
#define        OSD_MAIN_VIDEO_SCALEXM  0x10
#define        OSD_MAIN_SWAPRB         0x80
        
#define        OSD_COMPONENT_ENABLE    0x0001       // Eanble the component
        
#define        OSD_VIDEO_HALFHEIGHT    0x0002
#define        OSD_VIDEO_ZY0           0x0000       // video normal zoom y
#define        OSD_VIDEO_ZY1           0x0004       // video x2 zoom y
#define        OSD_VIDEO_ZY2           0x0008       // video x4 zoom y
#define        OSD_VIDEO_ZX0           0x0000       // video normal zoom x
#define        OSD_VIDEO_ZX1           0x0010       // video x2 zoom x
#define        OSD_VIDEO_ZX2           0x0020       // video x4 zoom x

#define        OSD_BITMAP_HALFHEIGHT   0x0002
#define        OSD_BITMAP_MERGEBACK    0x0000       // either alpha blend
                                                    // with bground
#define        OSD_BITMAP_0TRANS       0x0004       // or color 0 = transparent
#define        OSD_BITMAP_A_SHIFT      3            // alpha blending level
#define        OSD_BITMAP_A0           0x0000
#define        OSD_BITMAP_A1           0x0008
#define        OSD_BITMAP_A2           0x0010
#define        OSD_BITMAP_A3           0x0018
#define        OSD_BITMAP_A4           0x0020
#define        OSD_BITMAP_A5           0x0028
#define        OSD_BITMAP_A6           0x0030
#define        OSD_BITMAP_A7           0x0038
        
#define        OSD_BITMAP_1BIT         0x0000       // bitmap is 2 color
#define        OSD_BITMAP_2BIT         0x0040       // bitmap is 4 color
#define        OSD_BITMAP_4BIT         0x0080       // bitmap is 16 color
#define        OSD_BITMAP_8BIT         0x00c0       // bitmap is 256 color
        
#define        OSD_BITMAP_ZY0          0x0000       // normal zoom y
#define        OSD_BITMAP_ZY1          0x0100       // x2 zoom y
#define        OSD_BITMAP_ZY2          0x0200       // x4 zoom y
#define        OSD_BITMAP_ZX0          0x0000       // normal zoom x
#define        OSD_BITMAP_ZX1          0x0400       // x2 zoom x
#define        OSD_BITMAP_ZX2          0x0800       // x4 zoom x

#define        OSD_BITMAP_RAMCLUT      0x1000       // use the ram pallette

#define        OSD_CURSOR1_RAMCLUT             0x0080
#define        OSD_CURSOR1_BORDERHEIGHT_SHIFT  1
#define        OSD_CURSOR1_BORDERWIDTH_SHIFT   4
#define        OSD_CURSOR1_COLOR_SHIFT         8

#define        OSD_CURSOR2_HALFHEIGHT          0x0002
#define        OSD_CURSOR2_RAMCLUT             0x0004
#define        OSD_CURSOR2_COLORBANK0          0x0000   // use 16color bank0
#define        OSD_CURSOR2_COLORBANK1          0x0008   // use 16color bank1
#define        OSD_CURSOR2_A_SHIFT             4        // alpha blend level
#define        OSD_CURSOR2_A0                  0x0000
#define        OSD_CURSOR2_A1                  0x0010
#define        OSD_CURSOR2_A2                  0x0020
#define        OSD_CURSOR2_A3                  0x0030
#define        OSD_CURSOR2_A4                  0x0040
#define        OSD_CURSOR2_A5                  0x0050
#define        OSD_CURSOR2_A6                  0x0060
#define        OSD_CURSOR2_A7                  0x0070
#define        OSD_CURSOR2_ZY0                 0x0000
#define        OSD_CURSOR2_ZY1                 0x0100
#define        OSD_CURSOR2_ZY2                 0x0200
#define        OSD_CURSOR2_ZX0                 0x0000
#define        OSD_CURSOR2_ZX1                 0x0400
#define        OSD_CURSOR2_ZX2                 0x0800
#define        OSD_CURSOR2_FLASH_SPEED0        0x0000
#define        OSD_CURSOR2_FLASH_SPEED1        0x1000
#define        OSD_CURSOR2_FLASH_SPEED2        0x2000
#define        OSD_CURSOR2_FLASH_SPEED3        0x3000
#define        OSD_CURSOR2_FLASH_ENABLE        0x4000   // enable flashing


extern void osdInit();
extern void osdSetComponentOffset(u32 component, u32 offset);
extern void osdSetComponentSize(u32 component, u32 width, u32 height);
extern void osdSetComponentPosition(u32 component, u32 x, u32 y);
extern void osdSetComponentConfig(u32 component, u32 config);
extern void osdSetComponentSourceWidth(u32 component, u32 width);
extern void osdSetCursor2Bitmap(u32 index, u32 data);
extern void osdSetPallette(u32 y, u32 cr, u32 cb, u32 index);
extern void osdSet16CPallette(u32 bank, u32 index, u32 value);
extern void osdSetBrightness(u32 brightness);
extern void osdSetContrast(u32 contrast);
extern void osdSetMainShift(u32 horizontal, u32 vertical);
extern void osdSetMainConfig(u32 config);
extern void osdSetBorderColor(u32 color);
extern void osdSetAltOffset(u32 offset);

extern void osdInitA();
extern void osdSetComponentOffsetA(u32 component, u32 offset);
extern void osdSetComponentSizeA(u32 component, u32 width, u32 height);
extern void osdSetComponentPositionA(u32 component, u32 x, u32 y);
extern void osdSetComponentConfigA(u32 component, u32 config);
extern void osdSetComponentSourceWidthA(u32 component, u32 width);
extern void osdSetCursor2BitmapA(u32 index, u32 data);
extern void osdSetPalletteA(u32 y, u32 cr, u32 cb, u32 index);
extern void osdSet16CPalletteA(u32 bank, u32 index, u32 value);
extern void osdSetBrightnessA(u32 brightness);
extern void osdSetContrastA(u32 contrast);
extern void osdSetMainShiftA(u32 horizontal, u32 vertical);
extern void osdSetMainConfigA(u32 config);
extern void osdSetBorderColorA(u32 color);
extern void osdSetAltOffsetA(u32 offset);
extern void osdSetVideoOutOn();
extern void osdSetVideoOutOff();

