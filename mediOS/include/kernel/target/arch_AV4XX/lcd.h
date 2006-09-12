/*
*   include/kernel/target/arch_GMINI4XX/lcd.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __ARCH_LCD_H
#define __ARCH_LCD_H

#include <kernel/clkc.h>

#define LCD_BACK_LIGHT                    0x0

#define SCREEN_WIDTH                      320
#define SCREEN_REAL_WIDTH                 320
#define SCREEN_HEIGHT                     240
#define SCREEN_ORIGIN_X                   20
#define SCREEN_ORIGIN_Y                   19

#define SCREEN_LCD_SHIFT_X                121
#define SCREEN_LCD_SHIFT_Y                0
#define SCREEN_PAL_SHIFT_X                121
#define SCREEN_PAL_SHIFT_Y                0
#define SCREEN_NTSC_SHIFT_X               121
#define SCREEN_NTSC_SHIFT_Y               0

#define LCD_WIDTH SCREEN_REAL_WIDTH
#define LCD_HEIGHT SCREEN_HEIGHT

#define OSD_BMAP_1_CFG      OSD_BITMAP_RAMCLUT | OSD_BITMAP_ZX1 \
                | OSD_BITMAP_8BIT | COLOR_TRSP << OSD_BITMAP_A_SHIFT
#define OSD_BMAP_2_CFG      OSD_BITMAP_RAMCLUT | OSD_BITMAP_ZX1 \
                | OSD_BITMAP_8BIT | OSD_BITMAP_0TRANS | COLOR_TRSP << OSD_BITMAP_A_SHIFT
#define OSD_VID_1_CFG       0
#define OSD_VID_2_CFG       0
#define OSD_CUR_1_CFG       0
#define OSD_CUR_2_CFG       0

#define OSD_CON_BMAP_CFG    OSD_BITMAP_ZX1 | OSD_BITMAP_8BIT | COLOR_TRSP << OSD_BITMAP_A_SHIFT

// only backlight off for now
#define lcd_ON() {outw(0xffff,CLKC_PWM1_HIGH);}
#define lcd_OFF() {outw(0x0000,CLKC_PWM1_HIGH);}

#endif
