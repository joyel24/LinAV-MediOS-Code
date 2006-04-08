/*
*   include/kernel/osd.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __OSD_H
#define __OSD_H

#include <kernel/hardware.h>

#define OSD_CONF                          (OSD_BASE+0x00)   // OSD Main config and border color
#define OSD_VID0_1_CONF                   (OSD_BASE+0x02)   // Video0 and video1 config
#define OSD_BITMAP0_CONF                  (OSD_BASE+0x04)   // Bitmap0 config
#define OSD_BITMAP1_CONF                  (OSD_BASE+0x06)   // Bitmap1 config
#define OSD_CURSOR0_CONF                  (OSD_BASE+0x08)   // Cursor0 config
#define OSD_CURSOR1_CONF                  (OSD_BASE+0x0a)   // Cursor1 config

#define OSD_COMP_CONF(component)          (OSD_CONF+(component<2?2:component*2))

#define OSD_VID0_BUFF_W                   (OSD_BASE+0x0c)   // Video0 buffer width
#define OSD_VID1_BUFF_W                   (OSD_BASE+0x0e)   // Video1 buffer width
#define OSD_BITMAP0_BUFF_W                (OSD_BASE+0x10)   // Bitmap0 buffer width
#define OSD_BITMAP1_BUFF_W                (OSD_BASE+0x12)   // Bitmap1 buffer width

#define OSD_COMP_BUFF_W(component)        (OSD_VID0_BUFF_W+(component*2))

#define OSD_SDRAM_OFF_HI_VID0_1           (OSD_BASE+0x14)   // SDRAM offset HI Video0 | Video1
#define OSD_SDRAM_OFF_VID0                (OSD_BASE+0x16)   // SDRAM offset video0
#define OSD_SDRAM_OFF_VID1                (OSD_BASE+0x18)   // SDRAM offset video1
#define OSD_SDRAM_OFF_HI_BITMAP0_1        (OSD_BASE+0x1a)   // SDRAM offset HI Bitmap0 | Bitmap1
#define OSD_SDRAM_OFF_BITMAP0             (OSD_BASE+0x1c)   // SDRAM offset Bitmap0
#define OSD_SDRAM_OFF_BITMAP1             (OSD_BASE+0x1e)   // SDRAM offset Bitmap1

#define OSD_BITMAP0_SHIFT_HORIZ           (OSD_BASE+0x20)   // Main shift horizontal
#define OSD_BITMAP0_SHIFT_VERT            (OSD_BASE+0x22)   // Main shift vertical

#define OSD_VID0_X                        (OSD_BASE+0x24)   // Video0 X
#define OSD_VID0_Y                        (OSD_BASE+0x26)   // Video0 Y
#define OSD_VID0_W                        (OSD_BASE+0x28)   // Video0 width
#define OSD_VID0_H                        (OSD_BASE+0x2a)   // Video0 height

#define OSD_VID1_X                        (OSD_BASE+0x2c)   // Video1 X
#define OSD_VID1_Y                        (OSD_BASE+0x2e)   // Video1 Y
#define OSD_VID1_W                        (OSD_BASE+0x30)   // Video1 width
#define OSD_VID1_H                        (OSD_BASE+0x32)   // Video1 height

#define OSD_BITMAP0_X                     (OSD_BASE+0x34)   // Bitmap0 X
#define OSD_BITMAP0_Y                     (OSD_BASE+0x36)   // Bitmap0 Y
#define OSD_BITMAP0_W                     (OSD_BASE+0x38)   // Bitmap0 width
#define OSD_BITMAP0_H                     (OSD_BASE+0x3a)   // Bitmap0 height

#define OSD_BITMAP1_X                     (OSD_BASE+0x3c)   // Bitmap1 X
#define OSD_BITMAP1_Y                     (OSD_BASE+0x3e)   // Bitmap1 Y
#define OSD_BITMAP1_W                     (OSD_BASE+0x40)   // Bitmap1 width
#define OSD_BITMAP1_H                     (OSD_BASE+0x42)   // Bitmap1 height

#define OSD_CURSOR0_X                     (OSD_BASE+0x44)   // Cursor0 X
#define OSD_CURSOR0_Y                     (OSD_BASE+0x46)   // Cursor0 Y
#define OSD_CURSOR0_W                     (OSD_BASE+0x48)   // Cursor0 width
#define OSD_CURSOR0_H                     (OSD_BASE+0x4a)   // Cursor0 height

#define OSD_COMP_W(component)             (OSD_VID0_W+(component*8))
#define OSD_COMP_H(component)             (OSD_VID0_H+(component*8))

#define OSD_CURSOR1_X                     (OSD_BASE+0x4c)   // Cursor1 X
#define OSD_CURSOR1_Y                     (OSD_BASE+0x4e)   // Cursor1 Y

#define OSD_COMP_X(component)             (OSD_VID0_X+(component*8))
#define OSD_COMP_Y(component)             (OSD_VID0_Y+(component*8))

#define OSD_BANK1_0_1                     (OSD_BASE+0x50)   // 16c bank1 pallette #0 | #1
#define OSD_BANK1_2_3                     (OSD_BASE+0x52)   // 16c bank1 pallette #2 | #3
#define OSD_BANK1_4_5                     (OSD_BASE+0x54)   // 16c bank1 pallette #4 | #5
#define OSD_BANK1_6_7                     (OSD_BASE+0x56)   // 16c bank1 pallette #6 | #7
#define OSD_BANK1_8_9                     (OSD_BASE+0x58)   // 16c bank1 pallette #8 | #9
#define OSD_BANK1_a_b                     (OSD_BASE+0x5a)   // 16c bank1 pallette #a | #b
#define OSD_BANK1_c_d                     (OSD_BASE+0x5c)   // 16c bank1 pallette #c | #d
#define OSD_BANK1_e_f                     (OSD_BASE+0x5e)   // 16c bank1 pallette #e | #f

#define OSD_BANK2_0_1                     (OSD_BASE+0x60)   // 16c bank2 pallette #0 | #1
#define OSD_BANK2_2_3                     (OSD_BASE+0x62)   // 16c bank2 pallette #2 | #3
#define OSD_BANK2_4_5                     (OSD_BASE+0x64)   // 16c bank2 pallette #4 | #5
#define OSD_BANK2_6_7                     (OSD_BASE+0x66)   // 16c bank2 pallette #6 | #7
#define OSD_BANK2_8_9                     (OSD_BASE+0x68)   // 16c bank2 pallette #8 | #9
#define OSD_BANK2_a_b                     (OSD_BASE+0x6a)   // 16c bank2 pallette #a | #b
#define OSD_BANK2_c_d                     (OSD_BASE+0x6c)   // 16c bank2 pallette #c | #d
#define OSD_BANK2_e_f                     (OSD_BASE+0x6e)   // 16c bank2 pallette #e | #f


#define GET_BANK(bankN)                   ((bankN == 0) ? OSD_BANK1_0_1 : OSD_BANK2_0_1)
#define GET_BANK_ADDR(bankN,index)        (GET_BANK(bankN) + ((index>>1)<<1))
#define GET_BANK_SHIFT(index)             ((( index<<31 ) >>31)<<3)

//				(OSD_BASE+0x70)	306f0   ? Unknown
#define OSD_CURSOR2_DATA                  (OSD_BASE+0x72)   // Cursor2 data
#define OSD_PAL_ACCESS_STATUS             (OSD_BASE+0x74)   // Pallette access status
#define OSD_CURSOR2_ADD_LATCH             (OSD_BASE+0x74)   // cursor2 address/latch
#define OSD_PAL_DATA_WRITE                (OSD_BASE+0x76)   // Pallette data write
#define OSD_PAL_INDEX_WRITE               (OSD_BASE+0x78)   // Pallette index write

#define OSD_ALT_VID_OFF_LO                (OSD_BASE+0x7c)   // Alternative video sdram offset LO
#define OSD_ALT_VID_OFF_HI                (OSD_BASE+0x7e)   // Alternative video sdram offset HI


#define     OSD_VIDEO1                    0
#define     OSD_VIDEO2                    1
#define     OSD_BITMAP1                   2
#define     OSD_BITMAP2                   3
#define     OSD_CURSOR1                   4
#define     OSD_CURSOR2                   5

#define     OSD_MAIN_BORDER_RAMCLUT       0x01
#define     OSD_MAIN_VIDEO_SCALEX         0x04
#define     OSD_MAIN_VIDEO_SCALEY         0x08
#define     OSD_MAIN_VIDEO_SCALESM        0x10
#define     OSD_MAIN_SWAPRB               0x80

#define     OSD_COMPONENT_ENABLE          0x0001      /* 0000000000000001 */

/* VIDEO Configuration options */

#define     OSD_VIDEO_HALFHEIGHT          0x0002      /* 00000010 */
#define     OSD_VIDEO_ZY0                 0x0000      /* 00001100 */
#define     OSD_VIDEO_ZY1                 0x0004
#define     OSD_VIDEO_ZY2                 0x0008
#define     OSD_VIDEO_ZX0                 0x0000      /* 00110000 */
#define     OSD_VIDEO_ZX1                 0x0010
#define     OSD_VIDEO_ZX2                 0x0020

/* BITMAP Configuration options */

#define     OSD_BITMAP_HALFHEIGHT         0x0002      /* 0000000000000010 */
#define     OSD_BITMAP_MERGEBACK          0x0000      /* 0000000000000100 */
#define     OSD_BITMAP_0TRANS             0x0004

#define     OSD_BITMAP_A_SHIFT            3
#define     OSD_BITMAP_A0                 0x0000      /* 0000000000111000 */
#define     OSD_BITMAP_A1                 0x0008
#define     OSD_BITMAP_A2                 0x0010
#define     OSD_BITMAP_A3                 0x0018
#define     OSD_BITMAP_A4                 0x0020
#define     OSD_BITMAP_A5                 0x0028
#define     OSD_BITMAP_A6                 0x0030
#define     OSD_BITMAP_A7                 0x0038

#define     OSD_BITMAP_1BIT               0x0000      /* 0000000011000000 */
#define     OSD_BITMAP_2BIT               0x0040
#define     OSD_BITMAP_4BIT               0x0080
#define     OSD_BITMAP_8BIT               0x00c0
        
#define     OSD_BITMAP_ZY0                0x0000      /* 0000111100000000 */
#define     OSD_BITMAP_ZY1                0x0100
#define     OSD_BITMAP_ZY2                0x0200
#define     OSD_BITMAP_ZX0                0x0000
#define     OSD_BITMAP_ZX1                0x0400
#define     OSD_BITMAP_ZX2                0x0800

#define     OSD_BITMAP_RAMCLUT            0x1000      /* 0001000000000000 */

/* CURSOR1 Configuration options */

#define     OSD_CURSOR1_RAMCLUT            0x0080
#define     OSD_CURSOR1_BORDERHEIGHT_SHIFT 1       /* 0-7 */
#define     OSD_CURSOR1_BORDERWIDTH_SHIFT  4       /* 0-7 */
#define     OSD_CURSOR1_COLOR_SHIFT        8       /* 0-255 */

/* CURSOR2 Configuration options */

#define     OSD_CURSOR2_HALFHEIGHT         0x0002
#define     OSD_CURSOR2_RAMCLUT            0x0004

#define     OSD_CURSOR2_COLORBANK0         0x0000
#define     OSD_CURSOR2_COLORBANK1         0x0008
#define     OSD_CURSOR2_A_SHIFT            4
#define     OSD_CURSOR2_A0                 0x0000
#define     OSD_CURSOR2_A1                 0x0010
#define     OSD_CURSOR2_A2                 0x0020
#define     OSD_CURSOR2_A3                 0x0030
#define     OSD_CURSOR2_A4                 0x0040
#define     OSD_CURSOR2_A5                 0x0050
#define     OSD_CURSOR2_A6                 0x0060
#define     OSD_CURSOR2_A7                 0x0070
#define     OSD_CURSOR2_ZY0                0x0000      /* 0000111100000000 */
#define     OSD_CURSOR2_ZY1                0x0100
#define     OSD_CURSOR2_ZY2                0x0200
#define     OSD_CURSOR2_ZX0                0x0000
#define     OSD_CURSOR2_ZX1                0x0400
#define     OSD_CURSOR2_ZX2                0x0800
#define     OSD_CURSOR2_FLASH_SPEED0       0x0000
#define     OSD_CURSOR2_FLASH_SPEED1       0x1000
#define     OSD_CURSOR2_FLASH_SPEED2       0x2000
#define     OSD_CURSOR2_FLASH_SPEED3       0x3000
#define     OSD_CURSOR2_FLASH_ENABLE       0x4000

//#define     OSD_OFF_HI_SHIFT(index)        ((( index<<31 ) >>31)<<3)


int  osd_RGB2Packed(int r, int g, int b);
void osd_setCursor2Bitmap (int index, int data);
void osd_setMainConfig (int config);
void osd_setMainShift (int horizontal,int vertical);
void osd_setEntirePalette (int palette[256][3],int size);
void osd_setPaletteRGB(int r,int g,int b,int index);
void osd_setPalette (int Y, int Cr, int Cb, int index);
void osd_set16CPalette (int bankN, int index, int value);
void osd_setAltOffset (int address);
void osd_setComponentOffset (int component, int address);
void osd_setComponentSize (int component, int width, int height);
void osd_setComponentPosition (int component, int x, int y);
void osd_setComponentSourceWidth (int component, int width);
void osd_setComponentConfig (int component, int config);
void osd_restorePlane(int component, unsigned int address, int x, int y, int w, int h, int bpp, int state,int enable);
void osd_init();
void arch_osd_init(void);


#define RGB2Y(r,g,b)  ((306*r + 601*g + 117*b) >> 10)
#define RGB2Cb(r,g,b)  (((-173*r -339*g + 512*b) >> 10) + 128)
#define RGB2Cr(r,g,b)  (((512*r - 429*g - 83*b) >> 10) + 128)

#endif

