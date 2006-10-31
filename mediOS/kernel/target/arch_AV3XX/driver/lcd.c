/*
*   kernel/target/arch_AV3XX/lcd.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/lcd.h>
#include <kernel/io.h>
#include <kernel/gio.h>
#include <kernel/cpld.h>

int lcd_bright=0;

void arch_lcd_init(){
}

void arch_lcd_enable(){
    CPLD_SET_PORT2(CPLD_LCD);
    GIO_CLEAR(GIO_LCD_BACKLIGHT);
}

void arch_lcd_disable(){
    CPLD_CLEAR_PORT2(CPLD_LCD);
    GIO_SET(GIO_LCD_BACKLIGHT);
}

void arch_lcd_setBrigthness(int br){
    lcd_bright=br*10;
}

