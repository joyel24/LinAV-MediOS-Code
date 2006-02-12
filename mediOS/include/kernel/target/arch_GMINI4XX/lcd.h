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

#define LCD_BACK_LIGHT                    0x0

#define SCREEN_WIDTH                      224
#define SCREEN_REAL_WIDTH                 220
#define SCREEN_HEIGHT                     176
#define SCREEN_ORIGIN_X                   8
#define SCREEN_ORIGIN_Y                   8

#define MAX_COL                           53
#define MAX_LINE                          27

#define lcd_ON()
#define lcd_OFF()

#endif
