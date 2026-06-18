/*
*   kernel/target/arch_AV4XX/lcd.c
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
#include <kernel/clkc.h>
#include <kernel/cpld.h>
#include <kernel/gio.h>

void arch_lcd_init(){
}

void arch_lcd_enable(){
    if(CPLD_VER==0x2)
    {
        GIO_DIRECTION(GIO_LCD_BL_AV4100,GIO_OUT);
        GIO_SET(GIO_LCD_BL_AV4100);
    }
}

void arch_lcd_disable(){
    if(CPLD_VER==0x2)
    {
        GIO_DIRECTION(GIO_LCD_BL_AV4100,GIO_OUT);
        GIO_CLEAR(GIO_LCD_BL_AV4100);
    }
}

void arch_lcd_setBrigthness(int br){
    outw(br*0xffff/100,CLKC_PWM1_HIGH);
}

