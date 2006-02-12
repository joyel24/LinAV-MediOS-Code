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

#define SCREEN_WIDTH                      320
#define SCREEN_REAL_WIDTH                 320
#define SCREEN_HEIGHT                     240
#define SCREEN_ORIGIN_X                   20
#define SCREEN_ORIGIN_Y                   18

#define MAX_COL                           78
#define MAX_LINE                          38

#define lcd_ON() { \
    CPLD_SET_PORT2(CPLD_LCD); \
    GIO_CLEAR(GIO_LCD_BACKLIGHT); \
}

#define lcd_OFF() { \
    CPLD_CLEAR_PORT2(CPLD_LCD); \
    GIO_SET(GIO_LCD_BACKLIGHT); \
}

#endif
