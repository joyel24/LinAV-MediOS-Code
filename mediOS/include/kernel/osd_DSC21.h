/*
*   include/osd.h
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __OSD_DSC21_H
#define __OSD_DSC21_H

#include <kernel/hardware.h>

#define OSD_CONF                          (OSD_BASE+0x00)   // OSD Main config and border color
#define OSD_VID0_1_CONF                   (OSD_BASE+0x02)   // Video0 config (no video1)

#define OSD_BITMAP0_CONF                  (OSD_BASE+0x38)   // Bitmap0 config
#define OSD_BITMAP1_CONF                  (OSD_BASE+0x3A)   // Bitmap1 config
#define OSD_CURSOR0_CONF                  (OSD_BASE+0x36)   // Cursor0 config
//#define OSD_CURSOR1_CONF                  (0)               // NO Cursor1 config

//#define OSD_COMP_CONF(component)          ()

#define OSD_VID0_BUFF_W                   (OSD_BASE+0x04)   // Video0 buffer width
//#define OSD_VID1_BUFF_W                   (0)               // NO Video1 buffer width
//#define OSD_BITMAP0_BUFF_W                (0)               // NO Bitmap0 buffer width
//#define OSD_BITMAP1_BUFF_W                (0)               // NO Bitmap1 buffer width

//#define OSD_COMP_BUFF_W(component)        (OSD_VID0_BUFF_W+(component*2))

#define OSD_SDRAM_OFF_HI_VID0              (OSD_BASE+0x06)   // SDRAM offset HI Video0 (no Video1)
#define OSD_SDRAM_OFF_LO_VID0              (OSD_BASE+0x08)   // SDRAM offset video0
//#define OSD_SDRAM_OFF_VID1                (0)               // NO SDRAM offset video1
#define OSD_SDRAM_OFF_HI_CURSOR           (OSD_BASE+0x0a)   // SDRAM offset HI CURSOR 
#define OSD_SDRAM_OFF_LO_CURSOR           (OSD_BASE+0x0c)   // SDRAM offset LO CURSOR 
#define OSD_SDRAM_OFF_HI_BITMAP0          (OSD_BASE+0x0e)   // SDRAM offset HI Bitmap0
#define OSD_SDRAM_OFF_LO_BITMAP0          (OSD_BASE+0x10)   // SDRAM offset LO Bitmap0
#define OSD_SDRAM_OFF_HI_BITMAP1          (OSD_BASE+0x12)   // SDRAM offset HI Bitmap1
#define OSD_SDRAM_OFF_LO_BITMAP1          (OSD_BASE+0x14)   // SDRAM offset LO Bitmap1

#define OSD_MAIN_SHIFT_HORIZ              (OSD_BASE+0x16)   // Main shift horizontal
#define OSD_MAIN_SHIFT_VERT               (OSD_BASE+0x18)   // Main shift vertical

#define OSD_VID_W                         (OSD_BASE+0x1A)  // Video X size (*2?)
#define OSD_VID_H                         (OSD_BASE+0x1C)  // Video Y size 

#define OSD_CURSOR_LEFT                   (OSD_BASE+0x1E)  // Cursor Left
#define OSD_CURSOR_TOP                    (OSD_BASE+0x20)  // Cursor Top
#define OSD_CURSOR_RIGHT                  (OSD_BASE+0x22)  // Cursor Right
#define OSD_CURSOR_BOTTOM                 (OSD_BASE+0x24)  // Cursor Bottom

#define OSD_BITMAP0_LEFT                  (OSD_BASE+0x26)  // Bitmap0 Left
#define OSD_BITMAP0_TOP                   (OSD_BASE+0x28)  // Bitmap0 Top
#define OSD_BITMAP0_RIGHT                 (OSD_BASE+0x2A)  // Bitmap0 Right
#define OSD_BITMAP0_BOTTOM                (OSD_BASE+0x2C)  // Bitmap0 Bottom

#define OSD_BITMAP1_LEFT                  (OSD_BASE+0x2E)  // Bitmap1 Left
#define OSD_BITMAP1_TOP                   (OSD_BASE+0x30)  // Bitmap1 Top
#define OSD_BITMAP1_RIGHT                 (OSD_BASE+0x32)  // Bitmap1 Right
#define OSD_BITMAP1_BOTTOM                (OSD_BASE+0x34)  // Bitmap1 Bottom

#define OSD_BANK1_0_1                     (OSD_BASE+0x3C)   // 16c bank1 pallette #0 | #1
#define OSD_BANK1_2_3                     (OSD_BASE+0x3E)   // 16c bank1 pallette #2 | #3
#define OSD_BANK1_4_5                     (OSD_BASE+0x40)   // 16c bank1 pallette #4 | #5
#define OSD_BANK1_6_7                     (OSD_BASE+0x42)   // 16c bank1 pallette #6 | #7
#define OSD_BANK1_8_9                     (OSD_BASE+0x44)   // 16c bank1 pallette #8 | #9
#define OSD_BANK1_a_b                     (OSD_BASE+0x46)   // 16c bank1 pallette #a | #b
#define OSD_BANK1_c_d                     (OSD_BASE+0x48)   // 16c bank1 pallette #c | #d
#define OSD_BANK1_e_f                     (OSD_BASE+0x4A)   // 16c bank1 pallette #e | #f

#define OSD_BANK2_0_1                     (OSD_BASE+0x4C)   // 16c bank2 pallette #0 | #1
#define OSD_BANK2_2_3                     (OSD_BASE+0x4E)   // 16c bank2 pallette #2 | #3
#define OSD_BANK2_4_5                     (OSD_BASE+0x50)   // 16c bank2 pallette #4 | #5
#define OSD_BANK2_6_7                     (OSD_BASE+0x52)   // 16c bank2 pallette #6 | #7
#define OSD_BANK2_8_9                     (OSD_BASE+0x54)   // 16c bank2 pallette #8 | #9
#define OSD_BANK2_a_b                     (OSD_BASE+0x56)   // 16c bank2 pallette #a | #b
#define OSD_BANK2_c_d                     (OSD_BASE+0x58)   // 16c bank2 pallette #c | #d
#define OSD_BANK2_e_f                     (OSD_BASE+0x58)   // 16c bank2 pallette #e | #f

#define GET_BANK(bankN)                   ((bankN == 0) ? OSD_BANK1_0_1 : OSD_BANK2_0_1)
#define GET_BANK_ADDR(bankN,index)        (GET_BANK(bankN) + ((index>>1)<<1))
#define GET_BANK_SHIFT(index)             ((( index<<31 ) >>31)<<3)


#define     OSD_VIDEO                     0
#define     OSD_VIDEO1                    0
#define     OSD_VIDEO2                    1
#define     OSD_BITMAP1                   2
#define     OSD_BITMAP2                   3
#define     OSD_CURSOR                    4
#define     OSD_CURSOR1                   4
#define     OSD_CURSOR2                   5

#define     OSD_MAIN_BORDER_RAMCLUT       0x01
#define     OSD_MAIN_VIDEO_SCALEX         0x04
#define     OSD_MAIN_VIDEO_SCALEY         0x08
#define     OSD_MAIN_VIDEO_SCALESM        0x10
#define     OSD_MAIN_SWAPRB               0x80

#define     OSD_COMPONENT_ENABLE(component)    (component==OSD_VIDEO?0x4:0x1)

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

#endif

