/*
*   include/avlo_cfg.h
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __AVLO_CFG_H_
#define __AVLO_CFG_H_

struct avlo_cfg {
    /* color */
    
    /*position / size */
    int menu_x,menu_y;
    int status_x,status_y;
    int bar_x,bar_y,bar_w,bar_h;
    int usb_x,usb_y;
    int bat_x,bat_y;
};

struct avlo_cfg default_cfg = {
    menu_x      : 117,
    menu_y      : 54,
    status_x    : 2,
    status_y    : 228,
    bar_x       : 10,
    bar_y       : 200,
    bar_w       : 100,
    bar_h       : 5,
    usb_x       : 290,
    usb_y       : 228,
    bat_x       : 293,
    bat_y       : 2
};

#endif
