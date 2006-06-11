/*
* kernel/target/gfx/gui/main-menu.c
*
* MediOS project
* Copyright (c) 2005 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#include <kernel/hardware.h>
#include <kernel/lcd.h>

#include <sys_def/colordef.h>
#include <sys_def/font.h>
#include <sys_def/stddef.h>

#include <gui/menu.h>
#include <gui/main_menu.h>
#include <gui/gui.h>

struct menu_data menu_cfg = {
    useOwnDisp     : 0,
    x:0,y:0,width:320,height:240,
    txt_color      : COLOR_BLACK,
    bg_color       : COLOR_WHITE,
    select_color   : COLOR_BLUE,
    sub_color      : COLOR_RED,
    font           : STD6X9,
    root           : NULL,
    do_action      : mainMenu_doAction,
    on_action      : mainMenu_doOn,
    off_action     : mainMenu_doOff,
    f1_action      : mainMenu_doF1,
    f2_action      : mainMenu_doF2,
    f3_action      : mainMenu_doF3,
    item_str       : mainMenu_mkItemStr,
    submenu_str    : mainMenu_mkSubmenuStr,
    getSubIcon     : mainMenu_mkSubIcon,
    getItemIcon    : mainMenu_mkItemIcon,
    isTxtMenu      : 0,
    border_color   : COLOR_BLACK,
    has_border     : 0,
    title          : NULL,
};

struct statusBar_data barData = {
    x:0,y:0,
    w:SCREEN_REAL_WIDTH,
    bg_color:COLOR_LIGHT_BLUE,
    bat_x:293,
    bat_y:2,
    clk_x:135,
    clk_y:3,
    clk_color:COLOR_BLACK,
    has_date:1,
    logo_x:2,
    logo_y:2,
    ver_x:60,
    ver_y:3,  
    ver_color:COLOR_DARK_GREY,  
    module_x:242,
    module_y:4,
    pwr_x:278,
    pwr_y:4,
    usb_x:260,
    usb_y:4    
};
