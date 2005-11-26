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

#define GIO_ON_BTN    0x00
#define GIO_OFF_BTN   0x1C

#define GIO_SPDIF     0x01
#define GIO_VID_OUT   0x1D

#define GIO_DVR_IR    0x06

#define GIO_MAS_EOD   0x04
#define GIO_MAS_D0    0x08
#define GIO_MAS_D1    0x09
#define GIO_MAS_D2    0x0a
#define GIO_MAS_D3    0x0b
#define GIO_MAS_D4    0x0c
#define GIO_MAS_D5    0x0d
#define GIO_MAS_D6    0x0e
#define GIO_MAS_D7    0x0f
#define GIO_MAS_PWR   0x10
#define GIO_MAS_RTR   0x1E
#define GIO_MAS_PR    0x1F

#define GIO_I2C_CLOCK 0x12
#define GIO_I2C_DATA  0x13

#define GIO_LCD_BACKLIGHT 0x17

#define gio_addr(num,addr_bk1,addr_bk2,addr_bk3)   (num<16?addr_bk1:addr_bk2)
#define GIO_NUM(num)                               (num<16?num:num-16)
#define gio_bank_sel(bank,addr_bk1,addr_bk2,addr_bk3)   (bank==0?addr_bk1:bank==1?addr_bk2:addr_bk3)

#endif
