/*
* include/kernel/status_line.h
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

#ifndef __STATUS_LINE_H
#define __STATUS_LINE_H

void statusLine_init(void);
void statusLine_handleEvent(int evt);

struct statusLine_data {
    int x,y;
    int w;
    int bg_color;
    int bat_x,bat_y;
    int clk_x,clk_y,clk_color;
    int has_date;
    int logo_x,logo_y;
    int ver_x,ver_y,ver_color;
    int module_x,module_y;
    int pwr_x,pwr_y;
    int usb_x,usb_y;
};

#endif
