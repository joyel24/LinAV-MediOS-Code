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

#define OUTB(val,addr)           (*(volatile char *)(addr) = (val))
#define OUTW(val,addr)           (*(volatile unsigned short *)(addr) = (val))
#define OUTL(val,addr)           (*(volatile unsigned int *)(addr) = (val))
#define INB(addr)                (*(volatile char *)(addr))
#define INW(addr)                (*(volatile unsigned short *)(addr))
#define INL(addr)                (*(volatile unsigned int *)(addr))

/* video */

#define AV3XX_VIDEO_BASE                        0x30800


#define AV3XX_LCD_BACK_LIGHT                    0x02600200

/* osd */
#define AV3XX_OSD_BASE                          0x30680

#define AV3XX_OSD_CONF                          (AV3XX_OSD_BASE+0x00)   // OSD Main config and border color
#define AV3XX_OSD_VID0_1_CONF                   (AV3XX_OSD_BASE+0x02)   // Video0 and video1 config
#define AV3XX_OSD_BITMAP0_CONF                  (AV3XX_OSD_BASE+0x04)   // Bitmap0 config
#define AV3XX_OSD_BITMAP1_CONF                  (AV3XX_OSD_BASE+0x06)   // Bitmap1 config
#define AV3XX_OSD_CURSOR0_CONF                  (AV3XX_OSD_BASE+0x08)   // Cursor0 config
#define AV3XX_OSD_CURSOR1_CONF                  (AV3XX_OSD_BASE+0x0a)   // Cursor1 config

#define AV3XX_OSD_COMP_CONF(component)          (AV3XX_OSD_CONF+(component<2?2:component*2))

#define AV3XX_OSD_VID0_BUFF_W                   (AV3XX_OSD_BASE+0x0c)   // Video0 buffer width
#define AV3XX_OSD_VID1_BUFF_W                   (AV3XX_OSD_BASE+0x0e)   // Video1 buffer width
#define AV3XX_OSD_BITMAP0_BUFF_W                (AV3XX_OSD_BASE+0x10)   // Bitmap0 buffer width
#define AV3XX_OSD_BITMAP1_BUFF_W                (AV3XX_OSD_BASE+0x12)   // Bitmap1 buffer width

#define AV3XX_OSD_COMP_BUFF_W(component)        (AV3XX_OSD_VID0_BUFF_W+(component*2))

#define AV3XX_OSD_SDRAM_OFF_HI_VID0_1           (AV3XX_OSD_BASE+0x14)   // SDRAM offset HI Video0 | Video1
#define AV3XX_OSD_SDRAM_OFF_VID0                (AV3XX_OSD_BASE+0x16)   // SDRAM offset video0
#define AV3XX_OSD_SDRAM_OFF_VID1                (AV3XX_OSD_BASE+0x18)   // SDRAM offset video1
#define AV3XX_OSD_SDRAM_OFF_HI_BITMAP0_1        (AV3XX_OSD_BASE+0x1a)   // SDRAM offset HI Bitmap0 | Bitmap1
#define AV3XX_OSD_SDRAM_OFF_BITMAP0             (AV3XX_OSD_BASE+0x1c)   // SDRAM offset Bitmap0
#define AV3XX_OSD_SDRAM_OFF_BITMAP1             (AV3XX_OSD_BASE+0x1e)   // SDRAM offset Bitmap1

#define AV3XX_OSD_BITMAP0_SHIFT_HORIZ           (AV3XX_OSD_BASE+0x20)   // Main shift horizontal
#define AV3XX_OSD_BITMAP0_SHIFT_VERT            (AV3XX_OSD_BASE+0x22)   // Main shift vertical

#define AV3XX_OSD_VID0_X                        (AV3XX_OSD_BASE+0x24)   // Video0 X
#define AV3XX_OSD_VID0_Y                        (AV3XX_OSD_BASE+0x26)   // Video0 Y
#define AV3XX_OSD_VID0_W                        (AV3XX_OSD_BASE+0x28)   // Video0 width
#define AV3XX_OSD_VID0_H                        (AV3XX_OSD_BASE+0x2a)   // Video0 height

#define AV3XX_OSD_VID1_X                        (AV3XX_OSD_BASE+0x2c)   // Video1 X
#define AV3XX_OSD_VID1_Y                        (AV3XX_OSD_BASE+0x2e)   // Video1 Y
#define AV3XX_OSD_VID1_W                        (AV3XX_OSD_BASE+0x30)   // Video1 width
#define AV3XX_OSD_VID1_H                        (AV3XX_OSD_BASE+0x32)   // Video1 height

#define AV3XX_OSD_BITMAP0_X                     (AV3XX_OSD_BASE+0x34)   // Bitmap0 X
#define AV3XX_OSD_BITMAP0_Y                     (AV3XX_OSD_BASE+0x36)   // Bitmap0 Y
#define AV3XX_OSD_BITMAP0_W                     (AV3XX_OSD_BASE+0x38)   // Bitmap0 width
#define AV3XX_OSD_BITMAP0_H                     (AV3XX_OSD_BASE+0x3a)   // Bitmap0 height

#define AV3XX_OSD_BITMAP1_X                     (AV3XX_OSD_BASE+0x3c)   // Bitmap1 X
#define AV3XX_OSD_BITMAP1_Y                     (AV3XX_OSD_BASE+0x3e)   // Bitmap1 Y
#define AV3XX_OSD_BITMAP1_W                     (AV3XX_OSD_BASE+0x40)   // Bitmap1 width
#define AV3XX_OSD_BITMAP1_H                     (AV3XX_OSD_BASE+0x42)   // Bitmap1 height

#define AV3XX_OSD_CURSOR0_X                     (AV3XX_OSD_BASE+0x44)   // Cursor0 X
#define AV3XX_OSD_CURSOR0_Y                     (AV3XX_OSD_BASE+0x46)   // Cursor0 Y
#define AV3XX_OSD_CURSOR0_W                     (AV3XX_OSD_BASE+0x48)   // Cursor0 width
#define AV3XX_OSD_CURSOR0_H                     (AV3XX_OSD_BASE+0x4a)   // Cursor0 height

#define AV3XX_OSD_COMP_W(component)             (AV3XX_OSD_VID0_W+(component*8))
#define AV3XX_OSD_COMP_H(component)             (AV3XX_OSD_VID0_H+(component*8))

#define AV3XX_OSD_CURSOR1_X                     (AV3XX_OSD_BASE+0x4c)   // Cursor1 X
#define AV3XX_OSD_CURSOR1_Y                     (AV3XX_OSD_BASE+0x4e)   // Cursor1 Y

#define AV3XX_OSD_COMP_X(component)             (AV3XX_OSD_VID0_X+(component*8))
#define AV3XX_OSD_COMP_Y(component)             (AV3XX_OSD_VID0_Y+(component*8))

#define AV3XX_OSD_BANK1_0_1                     (AV3XX_OSD_BASE+0x50)   // 16c bank1 pallette #0 | #1
#define AV3XX_OSD_BANK1_2_3                     (AV3XX_OSD_BASE+0x52)   // 16c bank1 pallette #2 | #3
#define AV3XX_OSD_BANK1_4_5                     (AV3XX_OSD_BASE+0x54)   // 16c bank1 pallette #4 | #5
#define AV3XX_OSD_BANK1_6_7                     (AV3XX_OSD_BASE+0x56)   // 16c bank1 pallette #6 | #7
#define AV3XX_OSD_BANK1_8_9                     (AV3XX_OSD_BASE+0x58)   // 16c bank1 pallette #8 | #9
#define AV3XX_OSD_BANK1_a_b                     (AV3XX_OSD_BASE+0x5a)   // 16c bank1 pallette #a | #b
#define AV3XX_OSD_BANK1_c_d                     (AV3XX_OSD_BASE+0x5c)   // 16c bank1 pallette #c | #d
#define AV3XX_OSD_BANK1_e_f                     (AV3XX_OSD_BASE+0x5e)   // 16c bank1 pallette #e | #f

#define AV3XX_OSD_BANK2_0_1                     (AV3XX_OSD_BASE+0x60)   // 16c bank2 pallette #0 | #1
#define AV3XX_OSD_BANK2_2_3                     (AV3XX_OSD_BASE+0x62)   // 16c bank2 pallette #2 | #3
#define AV3XX_OSD_BANK2_4_5                     (AV3XX_OSD_BASE+0x64)   // 16c bank2 pallette #4 | #5
#define AV3XX_OSD_BANK2_6_7                     (AV3XX_OSD_BASE+0x66)   // 16c bank2 pallette #6 | #7
#define AV3XX_OSD_BANK2_8_9                     (AV3XX_OSD_BASE+0x68)   // 16c bank2 pallette #8 | #9
#define AV3XX_OSD_BANK2_a_b                     (AV3XX_OSD_BASE+0x6a)   // 16c bank2 pallette #a | #b
#define AV3XX_OSD_BANK2_c_d                     (AV3XX_OSD_BASE+0x6c)   // 16c bank2 pallette #c | #d
#define AV3XX_OSD_BANK2_e_f                     (AV3XX_OSD_BASE+0x6e)   // 16c bank2 pallette #e | #f


#define AV3XX_GET_BANK(bankN)                   ((bankN == 0) ? AV3XX_OSD_BANK1_0_1 : AV3XX_OSD_BANK2_0_1)
#define AV3XX_GET_BANK_ADDR(bankN,index)        (AV3XX_GET_BANK(bankN) + ((index>>1)<<1))
#define AV3XX_GET_BANK_SHIFT(index)             ((( index<<31 ) >>31)<<3)

//				(AV3XX_OSD_BASE+0x70)	306f0   ? Unknown
#define AV3XX_OSD_CURSOR2_DATA                  (AV3XX_OSD_BASE+0x72)   // Cursor2 data
#define AV3XX_OSD_PAL_ACCESS_STATUS             (AV3XX_OSD_BASE+0x74)   // Pallette access status
#define AV3XX_OSD_CURSOR2_ADD_LATCH             (AV3XX_OSD_BASE+0x74)   // cursor2 address/latch
#define AV3XX_OSD_PAL_DATA_WRITE                (AV3XX_OSD_BASE+0x76)   // Pallette data write
#define AV3XX_OSD_PAL_INDEX_WRITE               (AV3XX_OSD_BASE+0x78)   // Pallette index write

#define AV3XX_OSD_ALT_VID_OFF_LO                (AV3XX_OSD_BASE+0x7c)   // Alternative video sdram offset LO
#define AV3XX_OSD_ALT_VID_OFF_HI                (AV3XX_OSD_BASE+0x7e)   // Alternative video sdram offset HI


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

//#define     AV3XX_OSD_OFF_HI_SHIFT(index)        ((( index<<31 ) >>31)<<3)


int  osdRGB2Packed(int r, int g, int b);
void osdSetCursor2Bitmap (int index, int data);
void osdSetMainConfig (int config);
void osdSetMainShift (int horizontal,int vertical);
void osdSetPallette (int Y, int Cr, int Cb, int index);
void osdSet16CPallette (int bankN, int index, int value);
void osdSetAltOffset (int address);
void osdSetComponentOffset (int component, int address);
void osdSetComponentSize (int component, int width, int height);
void osdSetComponentPosition (int component, int x, int y);
void osdSetComponentSourceWidth (int component, int width);
void osdSetComponentConfig (int component, int config);
void osdSetBacklight(int val); /* val = 0/1 */
void osdInit();

#endif

