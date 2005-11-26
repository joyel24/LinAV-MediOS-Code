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

#define GIO_IN        1
#define GIO_OUT       0

#define GIO_INVERTED  1
#define GIO_NORMAL    0

#define GIO_IRQ       1
#define GIO_NOIRQ     0

#define GIO_BANK0     0
#define GIO_BANK1     1
#define GIO_BANK2     2

/* GIO lines */

#include <kernel/target/arch/gio.h>

/* GIO */

#define GIO_DIRECTION(num)                gio_addr(num,GIO_DIRECTION0,GIO_DIRECTION1,GIO_DIRECTION2)
#define GIO_INVERT(num)                   gio_addr(num,GIO_INVERT0,GIO_INVERT1,GIO_INVERT2)
#define GIO_BITSET(num)                   gio_addr(num,GIO_BITSET0,GIO_BITSET1,GIO_BITSET2)
#define GIO_BITCLEAR(num)                 gio_addr(num,GIO_BITCLEAR0,GIO_BITCLEAR1,GIO_BITCLEAR2)


/*functions */

#define GIO_NUM(num)                    (num<16?num:num-16)
#define GIO_OUT_VAL(dir,num,addr)       {outw((dir?inw(addr)|(0x1 << num):inw(addr)&~(0x1 << num)),addr);}


#define gio_dir(gio_num,direction)      GIO_OUT_VAL(direction,GIO_NUM(gio_num),GIO_DIRECTION(gio_num))
#define gio_inv(gio_num,direction)      GIO_OUT_VAL(direction,GIO_NUM(gio_num),GIO_INVERT(gio_num))
#define gio_set(gio_num)                {outw(0x1<<GIO_NUM(gio_num),GIO_BITSET(gio_num));}
#define gio_clear(gio_num)              {outw(0x1<<GIO_NUM(gio_num),GIO_BITCLEAR(gio_num));}
#define gio_IRQ(gio_num,enable)         GIO_OUT_VAL(enable,gio_num,GIO_ENABLE_IRQ)
#define gio_is_set(gio_num)             ((inw(GIO_BITSET(gio_num)) & (0x1 << GIO_NUM(gio_num))) != 0)
#define gio_raw(data,bank,mask)           { outw(data&mask,gio_bank_sel(bankGIO_BITSET0,GIO_BITSET1,GIO_BITSET2);\
                                          outw(data ^ mask,gio_bank_sel(bankGIO_BITSET0,GIO_BITSET1,GIO_BITSET2));}

#endif
