/*
*   include/target/arch_AV1XX/gio.h
*
*   AvLo - MediOS project
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

#define GIO_ON_BTN    0x19

//#define GIO_UKN_3     0x03

#define GIO_OFF_BTN   0x18

#define GIO_I2C_CLOCK 0x12
#define GIO_I2C_DATA  0x13

//#define GIO_LCD_BACKLIGHT 0x17

#define GIO_HD_POWER 0x1A

#define gio_addr(num,addr_bk1,addr_bk2,addr_bk3)   (num<16?addr_bk1:addr_bk2)
#define GIO_NUM(num)                               (num<16?num:num-16)
#define gio_bank_sel(bank,addr_bk1,addr_bk2,addr_bk3)   (bank==0?addr_bk1:bank==1?addr_bk2:addr_bk3)

#endif
