/*
*   include/kernel/lcd.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#ifndef __LCD_H
#define __LCD_H

#include <sys_def/types.h>
#include <kernel/target/arch/lcd.h>

#define LCD_MAX_BRIGHTNESS 100

void lcd_init();
void lcd_enable();
void lcd_disable();
bool lcd_enabled();
void lcd_setBrightness(int br);
int lcd_getBrightness();

void arch_lcd_init();
void arch_lcd_enable();
void arch_lcd_disable();
void arch_lcd_setBrigthness(int br);

#endif
