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

#include <kernel/cpld.h>
#include <kernel/gio.h>

#define LCD_BACK_LIGHT                    0x02600200

#define SCREEN_WIDTH                      672
#define SCREEN_REAL_WIDTH                 330
#define SCREEN_HEIGHT                     240
#define SCREEN_ORIGIN_X                   0x0
#define SCREEN_ORIGIN_Y                   0x0

#define SCREEN_LCD_SHIFT_X                143
#define SCREEN_LCD_SHIFT_Y                21
#define SCREEN_PAL_SHIFT_X                143
#define SCREEN_PAL_SHIFT_Y                21
#define SCREEN_NTSC_SHIFT_X               143
#define SCREEN_NTSC_SHIFT_Y               21

#define LCD_WIDTH                         168
#define LCD_HEIGHT                        120

//#define LCD_WIDTH SCREEN_REAL_WIDTH
//#define LCD_HEIGHT SCREEN_HEIGHT

#define OSD_BMAP_1_CFG      0x68
#define OSD_BMAP_2_CFG      0x68
#define OSD_VID_1_CFG       0xF0
#define OSD_VID_2_CFG       0
#define OSD_CUR_1_CFG       0
#define OSD_CUR_2_CFG       0

#define OSD_CON_BMAP_CFG    0x68

#if 0
#define lcd_ON() { \
    CPLD_SET_PORT2(CPLD_LCD); \
    GIO_CLEAR(GIO_LCD_BACKLIGHT); \
}

#define lcd_OFF() { \
    CPLD_CLEAR_PORT2(CPLD_LCD); \
    GIO_SET(GIO_LCD_BACKLIGHT); \
}
#endif

#define lcd_ON()
#define lcd_OFF()

#endif
