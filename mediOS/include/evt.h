/* 
*   include/kernel/evt.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __EVT_H
#define __EVT_H

#define        NO_EVENT      0x00
#define        BTN_UP        0x01
#define        BTN_DOWN      0x02
#define        BTN_LEFT      0x03
#define        BTN_RIGHT     0x04
#define        BTN_F1        0x05
#define        BTN_F2        0x06
#define        BTN_F3        0x07
#define        BTN_F4        0x08
#define        BTN_1         0x09
#define        BTN_2         0x0a
#define        BTN_3         0x0b
#define        BTN_4         0x0c
#define        BTN_ON        0x0d
#define        BTN_OFF       0x0e
#define        BTN_OPT1      0x0f
#define        BTN_OPT2      0x10


#define        EVT_TIMER     0x11
#define        EVT_PWR       0x12


/* EVT class */
#define        ALL_CLASS         0xFFFFFFFF
#define        BTN_CLASS         0x1
#define        CONNECT_CLASS     0x2
/*

#define        EVT_WKUP      0x0d
#define        EVT_USB       0x0e
#define        EVT_PWR       0x0f
#define        EVT_CF_IN     0x10
#define        EVT_CF_OUT    0x11
#define        EVT_FW_EXT    0x12

#define        EVT_REDRAW      0x51
#define        EVT_SUSPEND     0x52
#define        EVT_RESUME      0x53
#define        EVT_CF_REMOVED  0x54
#define        EVT_CF_ADDED    0x55
#define        EVT_MENU_UP_LVL 0x56
#define        EVT_QUIT        0x57
*/
      



#endif
