/* 
*   include/cpld.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __CPLD_H
#define __CPLD_H

#include <kernel/hardware.h>

#define CPLD0                  0x0
#define CPLD1                  0x1
#define CPLD2                  0x2
#define CPLD3                  0x3

/* PORT 0 */
/*** HD / CF */
#define CPLD_HD_CF             0x0
#define CPLD_SEL_HD            0x0
#define CPLD_SEL_CF            0x1

#define CPLD_FW_EXT            0x1
#define CPLD_IR                0x2

/* PORT 1*/
#define CPLD_USB               0x0

/* PORT 2*/
#define CPLD2_IR               0x1
#define CPLD_LCD               0x2

/* PORT 3*/
#define CPLD_FM                0x0
#define CPLD3_IR               0x1
#define CPLD_HD_POWER          0x3

#define CPLD_PORT0             (CPLD_BASE+0x000)
#define CPLD_PORT1             (CPLD_BASE+0x100)
#define CPLD_PORT2             (CPLD_BASE+0x200)
#define CPLD_PORT3             (CPLD_BASE+0x300)

void cpld_chg_state   (int cpld_port,int bit_num,int direction);
void cpld_select      (int bit_num,int direction);
int  cpld_read        (int cpld_port);

#define cpld_set_port_0(bit_num)     cpld_select(bit_num,1)
#define cpld_clear_port_0(bit_num)   cpld_select(bit_num,0)
#define cpld_set_port_1(bit_num)     cpld_chg_state(CPLD1,bit_num,1)
#define cpld_clear_port_1(bit_num)   cpld_chg_state(CPLD1,bit_num,0)
#define cpld_set_port_2(bit_num)     cpld_chg_state(CPLD2,bit_num,1)
#define cpld_clear_port_2(bit_num)   cpld_chg_state(CPLD2,bit_num,0)
#define cpld_set_port_3(bit_num)     cpld_chg_state(CPLD3,bit_num,1)
#define cpld_clear_port_3(bit_num)   cpld_chg_state(CPLD3,bit_num,0)

void cpld_do_select(void);
void init_cpld(void);
#endif
