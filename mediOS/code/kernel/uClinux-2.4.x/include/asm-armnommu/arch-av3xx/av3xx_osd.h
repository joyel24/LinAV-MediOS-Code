/*
* asm/arch/av3xx_osd.h
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

#ifndef __ASM_ARCH_AV3XX_OSD_H
#define __ASM_ARCH_AV3XX_OSD_H

#include <asm/types.h>
#include <asm/arch/hardware.h>

#define     AV3XX_OSD_VIDEO1                    0
#define     AV3XX_OSD_VIDEO2                    1
#define     AV3XX_OSD_BITMAP1                   2
#define     AV3XX_OSD_BITMAP2                   3
#define     AV3XX_OSD_CURSOR1                   4
#define     AV3XX_OSD_CURSOR2                   5

#define     AV3XX_OSD_MAIN_BORDER_RAMCLUT       0x01
#define     AV3XX_OSD_MAIN_VIDEO_SCALEX         0x04
#define     AV3XX_OSD_MAIN_VIDEO_SCALEY         0x08
#define     AV3XX_OSD_MAIN_VIDEO_SCALESM        0x10
#define     AV3XX_OSD_MAIN_SWAPRB               0x80

#define     AV3XX_OSD_COMPONENT_ENABLE          0x0001      /* 0000000000000001 */

/* VIDEO Configuration options */

#define     AV3XX_OSD_VIDEO_HALFHEIGHT          0x0002      /* 00000010 */
#define     AV3XX_OSD_VIDEO_ZY0                 0x0000      /* 00001100 */
#define     AV3XX_OSD_VIDEO_ZY1                 0x0004
#define     AV3XX_OSD_VIDEO_ZY2                 0x0008
#define     AV3XX_OSD_VIDEO_ZX0                 0x0000      /* 00110000 */
#define     AV3XX_OSD_VIDEO_ZX1                 0x0010
#define     AV3XX_OSD_VIDEO_ZX2                 0x0020

/* BITMAP Configuration options */

#define     AV3XX_OSD_BITMAP_HALFHEIGHT         0x0002      /* 0000000000000010 */
#define     AV3XX_OSD_BITMAP_MERGEBACK          0x0000      /* 0000000000000100 */
#define     AV3XX_OSD_BITMAP_0TRANS             0x0004

#define     AV3XX_OSD_BITMAP_A_SHIFT            3
#define     AV3XX_OSD_BITMAP_A0                 0x0000      /* 0000000000111000 */
#define     AV3XX_OSD_BITMAP_A1                 0x0008
#define     AV3XX_OSD_BITMAP_A2                 0x0010
#define     AV3XX_OSD_BITMAP_A3                 0x0018
#define     AV3XX_OSD_BITMAP_A4                 0x0020
#define     AV3XX_OSD_BITMAP_A5                 0x0028
#define     AV3XX_OSD_BITMAP_A6                 0x0030
#define     AV3XX_OSD_BITMAP_A7                 0x0038

#define     AV3XX_OSD_BITMAP_1BIT               0x0000      /* 0000000011000000 */
#define     AV3XX_OSD_BITMAP_2BIT               0x0040
#define     AV3XX_OSD_BITMAP_4BIT               0x0080
#define     AV3XX_OSD_BITMAP_8BIT               0x00c0
        
#define     AV3XX_OSD_BITMAP_ZY0                0x0000      /* 0000111100000000 */
#define     AV3XX_OSD_BITMAP_ZY1                0x0100
#define     AV3XX_OSD_BITMAP_ZY2                0x0200
#define     AV3XX_OSD_BITMAP_ZX0                0x0000
#define     AV3XX_OSD_BITMAP_ZX1                0x0400
#define     AV3XX_OSD_BITMAP_ZX2                0x0800

#define     AV3XX_OSD_BITMAP_RAMCLUT            0x1000      /* 0001000000000000 */

/* CURSOR1 Configuration options */

#define     AV3XX_OSD_CURSOR1_RAMCLUT            0x0080
#define     AV3XX_OSD_CURSOR1_BORDERHEIGHT_SHIFT 1       /* 0-7 */
#define     AV3XX_OSD_CURSOR1_BORDERWIDTH_SHIFT  4       /* 0-7 */
#define     AV3XX_OSD_CURSOR1_COLOR_SHIFT        8       /* 0-255 */

/* CURSOR2 Configuration options */

#define     AV3XX_OSD_CURSOR2_HALFHEIGHT         0x0002
#define     AV3XX_OSD_CURSOR2_RAMCLUT            0x0004

#define     AV3XX_OSD_CURSOR2_COLORBANK0         0x0000
#define     AV3XX_OSD_CURSOR2_COLORBANK1         0x0008
#define     AV3XX_OSD_CURSOR2_A_SHIFT            4
#define     AV3XX_OSD_CURSOR2_A0                 0x0000
#define     AV3XX_OSD_CURSOR2_A1                 0x0010
#define     AV3XX_OSD_CURSOR2_A2                 0x0020
#define     AV3XX_OSD_CURSOR2_A3                 0x0030
#define     AV3XX_OSD_CURSOR2_A4                 0x0040
#define     AV3XX_OSD_CURSOR2_A5                 0x0050
#define     AV3XX_OSD_CURSOR2_A6                 0x0060
#define     AV3XX_OSD_CURSOR2_A7                 0x0070
#define     AV3XX_OSD_CURSOR2_ZY0                0x0000      /* 0000111100000000 */
#define     AV3XX_OSD_CURSOR2_ZY1                0x0100
#define     AV3XX_OSD_CURSOR2_ZY2                0x0200
#define     AV3XX_OSD_CURSOR2_ZX0                0x0000
#define     AV3XX_OSD_CURSOR2_ZX1                0x0400
#define     AV3XX_OSD_CURSOR2_ZX2                0x0800
#define     AV3XX_OSD_CURSOR2_FLASH_SPEED0       0x0000
#define     AV3XX_OSD_CURSOR2_FLASH_SPEED1       0x1000
#define     AV3XX_OSD_CURSOR2_FLASH_SPEED2       0x2000
#define     AV3XX_OSD_CURSOR2_FLASH_SPEED3       0x3000
#define     AV3XX_OSD_CURSOR2_FLASH_ENABLE       0x4000

#define     AV3XX_OSD_OFF_HI_SHIFT(index)        ((( index<<31 ) >>31)<<3)

void osdSetCursor2Bitmap (u16 index, u16 data);
void osdSetMainConfig (u16 config);
void osdSetMainShift (u16 horizontal,u16 vertical);
void osdSetPallette (u16 Y, u16 Cr, u16 Cb, u16 index);
void osdSet16CPallete (int bankN, u16 index, u16 value);
void osdSetAltOffset (u32 address);
void osdSetComponentOffset (int component, u32 address);
void osdSetComponentSize (int component, u16 width, u16 height);
void osdSetComponentPosition (int component, u16 x, u16 y);
void osdSetComponentSourceWidth (int component, u16 width);
void osdSetComponentConfig (int component, u16 config);
void osdSetBacklight(int val); /* val = 0/1 */
void osdInit();

#endif

