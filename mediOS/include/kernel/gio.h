/* 
*   include/kernel/gio.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#ifndef __GIO_H
#define __GIO_H

#include <kernel/target/arch/gio.h>

#define GIO_IN        1
#define GIO_OUT       0

#define GIO_INVERTED  1
#define GIO_NORMAL    0

#define GIO_IRQ       1
#define GIO_NOIRQ     0

#define GIO_BANK0     0
#define GIO_BANK1     1
#define GIO_BANK2     2

/* GIO helper macros */

#define GIO_ADDR(num,addr_bk1,addr_bk2,addr_bk3)       (num<16?addr_bk1:(num<32?addr_bk2:addr_bk3))
#define GIO_BANK_SEL(bank,addr_bk1,addr_bk2,addr_bk3)  (bank==0?addr_bk1:(bank==1?addr_bk2:addr_bk3))

#define GIO_DIRECTION_ADDR(num)                        GIO_ADDR(num,GIO_DIRECTION0,GIO_DIRECTION1,GIO_DIRECTION2)
#define GIO_INVERT_ADDR(num)                           GIO_ADDR(num,GIO_INVERT0,GIO_INVERT1,GIO_INVERT2)
#define GIO_BITSET_ADDR(num)                           GIO_ADDR(num,GIO_BITSET0,GIO_BITSET1,GIO_BITSET2)
#define GIO_BITCLEAR_ADDR(num)                         GIO_ADDR(num,GIO_BITCLEAR0,GIO_BITCLEAR1,GIO_BITCLEAR2)

#define GIO_BIT_NUMBER(num)                            (num<16?num:(num<32?num-16:num-32))

#define GIO_OUT_VAL(dir,num,addr)                      {outw((dir?inw(addr)|(0x1 << num):inw(addr)&~(0x1 << num)),addr);}

/* GIO functions */

#define GIO_DIRECTION(gio_num,direction)               GIO_OUT_VAL(direction,GIO_BIT_NUMBER(gio_num),GIO_DIRECTION_ADDR(gio_num))
#define GIO_INVERT(gio_num,direction)                  GIO_OUT_VAL(direction,GIO_BIT_NUMBER(gio_num),GIO_INVERT_ADDR(gio_num))
#define GIO_SET(gio_num)                               {outw(0x1<<GIO_BIT_NUMBER(gio_num),GIO_BITSET_ADDR(gio_num));}
#define GIO_CLEAR(gio_num)                             {outw(0x1<<GIO_BIT_NUMBER(gio_num),GIO_BITCLEAR_ADDR(gio_num));}
#define GIO_IRQ_ENABLE(gio_num,enable)                 GIO_OUT_VAL(enable,gio_num,GIO_ENABLE_IRQ)
#define GIO_IS_SET(gio_num)                            ((inw(GIO_BITSET_ADDR(gio_num)) & (0x1 << GIO_BIT_NUMBER(gio_num))) != 0)
#define GIO_RAW(data,bank,mask)                        { outw(data&mask,GIO_BANK_SEL(bank,GIO_BITSET0,GIO_BITSET1,GIO_BITSET2);\
                                                       outw(data ^ mask,GIO_BANK_SEL(bank,GIO_BITSET0,GIO_BITSET1,GIO_BITSET2));}

#endif
