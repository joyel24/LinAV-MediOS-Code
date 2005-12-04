/*
*   include/kernel/target/arch_AV3XX/gio.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#ifndef __GIO_ARCH_H
#define __GIO_ARCH_H

/* GIO lines */

#define GIO_ON_BTN    0x01
#define GIO_OFF_BTN   0x02

#define GIO_I2C_CLOCK 0x09
#define GIO_I2C_DATA  0x08

#define GIO_BTN_SELECT_UDLR   0x0e
#define GIO_BTN_SELECT_MENU   0x10
#define GIO_BTN_SELECT_SQCR   0x11

#define GIO_SPDIF     0xff
#define GIO_VID_OUT   0xff

#define GIO_IR        0xff

#define GIO_LCD_BACKLIGHT 0xFF

#define gio_addr(num,addr_bk1,addr_bk2,addr_bk3)   (num<16?addr_bk1:addr_bk2)
#define GIO_NUM(num)                               (num<16?num:num-16)

#endif
