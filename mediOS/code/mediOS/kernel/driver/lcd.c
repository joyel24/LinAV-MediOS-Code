/*
*   kernel/driver/lcd.c
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

int lcd_brightness;
bool lcd_state;


void lcd_init(){

    lcd_setBrightness(LCD_MAX_BRIGHTNESS);
    lcd_enable();

    arch_lcd_init();
}

void lcd_enable(){
    lcd_state=true;
    arch_lcd_setBrigthness(lcd_brightness);
    arch_lcd_enable();
}

void lcd_disable(){
    lcd_state=false;
    arch_lcd_setBrigthness(0);
    arch_lcd_disable();
};

bool lcd_enabled(){
    return lcd_state;
}

void lcd_setBrightness(int br){
    lcd_brightness=br;
    if (lcd_state) arch_lcd_setBrigthness(br);
}

int lcd_getBrightness(){
    return lcd_brightness;
}
