/* 
*   include/evt.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __EVT_H
#define __EVT_H

struct evt_t {
    int evt;
    unsigned int evt_class;
    void * data;
};

#define        NO_EVENT      0x0000
#define        BTN_UP        0x0001
#define        BTN_DOWN      0x0002
#define        BTN_LEFT      0x0003
#define        BTN_RIGHT     0x0004
#define        BTN_F1        0x0005
#define        BTN_F2        0x0006
#define        BTN_F3        0x0007
#define        BTN_F4        0x0008
#define        BTN_1         0x0009
#define        BTN_2         0x000a
#define        BTN_3         0x000b
#define        BTN_4         0x000c
#define        BTN_ON        0x000d
#define        BTN_OFF       0x000e
#define        BTN_OPT1      0x000f
#define        BTN_OPT2      0x0010

#define        BTN_FM_REC        0x0011
#define        BTN_FM_HOLD       0x0012
#define        BTN_FM_MP3FM      0x0013
#define        BTN_FM_UP         0x0014
#define        BTN_FM_DOWN       0x0015
#define        BTN_FM_PREV       0x0016
#define        BTN_FM_NEXT       0x0016
#define        BTN_FM_PLAY       0x0018

#define        EVT_TIMER         0x0020

#define        EVT_PWR           0x0031
#define        EVT_USB           0x0032
#define        EVT_FW_EXT        0x0033

#define        EVT_REDRAW        0x0050
#define        EVT_MENU_UP_LVL   0x0051

/* EVT class */
#define        ALL_CLASS         0xFFFFFFFF
#define        BTN_CLASS         0x1
#define        CONNECT_CLASS     0x2
#define        GUI_CLASS         0x4
#define        TMR_CLASS         0x8

/*

#define        EVT_WKUP      0x0d
#define        EVT_CF_IN     0x10
#define        EVT_CF_OUT    0x11

#define        EVT_REDRAW      0x51
#define        EVT_SUSPEND     0x52
#define        EVT_RESUME      0x53
#define        EVT_CF_REMOVED  0x54
#define        EVT_CF_ADDED    0x55
#define        EVT_MENU_UP_LVL 0x56
#define        EVT_QUIT        0x57
*/
      



#endif
