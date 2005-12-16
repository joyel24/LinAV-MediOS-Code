/*
*   include/cpld.h
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __CPLD_H
#define __CPLD_H

#include <hardware.h>
#include <target/arch/cpld.h>

#define CPLD0                  0x0
#define CPLD1                  0x1
#define CPLD2                  0x2
#define CPLD3                  0x3

extern void cpld_chg_state   (int cpld_port,int bit_num,int direction);
extern void cpld_select      (int bit_num,int direction);
extern int  cpld_read        (int cpld_port);

#define cpld_set_port_0(bit_num)     cpld_select(bit_num,1)
#define cpld_clear_port_0(bit_num)   cpld_select(bit_num,0)
#define cpld_set_port_1(bit_num)     cpld_chg_state(CPLD1,bit_num,1)
#define cpld_clear_port_1(bit_num)   cpld_chg_state(CPLD1,bit_num,0)
#define cpld_set_port_2(bit_num)     cpld_chg_state(CPLD2,bit_num,1)
#define cpld_clear_port_2(bit_num)   cpld_chg_state(CPLD2,bit_num,0)
#define cpld_set_port_3(bit_num)     cpld_chg_state(CPLD3,bit_num,1)
#define cpld_clear_port_3(bit_num)   cpld_chg_state(CPLD3,bit_num,0)

extern void cpld_do_select(void);

extern void init_cpld(void);

extern int cpld_get_version(void);

int arch_cpld_get_version(void);

#endif
