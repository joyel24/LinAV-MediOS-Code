/*
*   include/target/arch_AV1XX/default_cfg.h
*
*   AvLo - MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __DEFAULT_CFG_H
#define __DEFAULT_CFG_H



struct avlo_cfg default_cfg = {
    magic       : {'A','V','L','O','_','L','Y','T'},
    menu_x      : 20,
    menu_y      : 15,
    status_x    : 0,
    status_y    : 100,
    bar_x       : 10,
    bar_y       : 90,
    bar_w       : 100,
    bar_h       : 5,
    usb_x       : 1,
    usb_y       : 1,
    bat_x       : 140,
    bat_y       : 1,
    INIT_COLOR(color_txt,color_txt_index,7)//COLOR_WHITE)
    INIT_COLOR(color_box,color_box_index,4)//COLOR_BLUE)
    INIT_COLOR(color_sel,color_sel_index,1)//COLOR_RED)
    INIT_COLOR(color_load,color_load_index,5)//COLOR_RED3)
    INIT_COLOR(color_wait,color_wait_index,49)//COLOR_ORANGE)
    INIT_COLOR(color_msg_box_0,color_msg_box_0_index,14)//COLOR_DARK_GRAY)
    INIT_COLOR(color_msg_box_1,color_msg_box_1_index,15)//COLOR_GRAY)
    INIT_COLOR(color_pwr_charge,color_pwr_charge_index,3)//COLOR_YELLOW)
    INIT_COLOR(color_pwr_l0,color_pwr_l0_index,51)//COLOR_DARK_RED)
    INIT_COLOR(color_pwr_l1,color_pwr_l1_index,1)//COLOR_RED)
    INIT_COLOR(color_pwr_l2,color_pwr_l2_index,50)//COLOR_ORANGE2)
    INIT_COLOR(color_pwr_l3,color_pwr_l3_index,6)//COLOR_TURQUOISE)
    INIT_COLOR(color_pwr_frame_dc_on,color_pwr_frame_dc_on_index,14)//COLOR_DARK_GRAY)
    INIT_COLOR(color_pwr_frame_dc_off,color_pwr_frame_dc_off_index,14)//COLOR_DARK_GRAY)
};

#endif
