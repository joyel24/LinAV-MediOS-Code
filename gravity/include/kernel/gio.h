/* 
*   include/gio.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GIO_H
#define __GIO_H

#define GIO_IN        1
#define GIO_OUT       0

#define GIO_INVERTED  1
#define GIO_NORMAL    0

#define GIO_IRQ       1
#define GIO_NOIRQ     0

#define GIO_BANK0     0
#define GIO_BANK1     1


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

/* GIO */
#define GIO_DIRECTION0                    (GIO_BASE+0x00)  // GIO 0-15
#define GIO_DIRECTION1                    (GIO_BASE+0x02)  // GIO 16-31
#define GIO_INVERT0                       (GIO_BASE+0x04)  // GIO 0-15
#define GIO_INVERT1                       (GIO_BASE+0x06)  // GIO 16-31
#define GIO_BITSET0                       (GIO_BASE+0x08)  // GIO 0-15
#define GIO_BITSET1                       (GIO_BASE+0x0a)  // GIO 16-31
#define GIO_BITCLEAR0                     (GIO_BASE+0x0c)  // GIO 0-15
#define GIO_BITCLEAR1                     (GIO_BASE+0x0e)  // GIO 16-31
#define GIO_ENABLE_IRQ                    (GIO_BASE+0x10)  // GIO 0-7

#define gio_addr(num,addr_bk1,addr_bk2)   (num<16?addr_bk1:addr_bk2)
#define GIO_DIRECTION(num)                gio_addr(num,GIO_DIRECTION0,GIO_DIRECTION1)
#define GIO_INVERT(num)                   gio_addr(num,GIO_GIO_INVERT0,GIO_GIO_INVERT1)
#define GIO_BITSET(num)                   gio_addr(num,GIO_BITSET0,GIO_BITSET1)
#define GIO_BITCLEAR(num)                 gio_addr(num,GIO_BITCLEAR0,GIO_BITCLEAR1)


/*functions */

#define GIO_NUM(num)                    (num<16?num:num-16)
#define GIO_OUT_VAL(dir,num,addr)       {outw((dir?inw(addr)|(0x1 << num):inw(addr)&~(0x1 << num)),addr);}


#define gio_dir(gio_num,direction)      GIO_OUT_VAL(direction,GIO_NUM(gio_num),GIO_DIRECTION(gio_num))
#define gio_inv(gio_num,direction)      GIO_OUT_VAL(direction,GIO_NUM(gio_num),GIO_INVERT(gio_num))
#define gio_set(gio_num)                {outw(0x1<<GIO_NUM(gio_num),GIO_BITSET(gio_num));}
#define gio_clear(gio_num)              {outw(0x1<<GIO_NUM(gio_num),GIO_BITCLEAR(gio_num));}
#define gio_IRQ(gio_num,enable)         GIO_OUT_VAL(enable,gio_num,GIO_ENABLE_IRQ)
#define gio_is_set(gio_num)             ((inw(GIO_BITSET(gio_num)) & (0x1 << GIO_NUM(gio_num))) != 0)
#define gio_raw(data,bk,mask)           { outw(data&mask,bank==0?GIO_BITSET0:GIO_BITSET1);\
                                          outw(data ^ mask,bank==0?GIO_BITSET0:GIO_BITSET1);}

#endif
