/*
*   include/kernel/cpld.h
*
*   MediOS project
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
#include <kernel/target/arch/cpld.h>

#define CPLD0                  0x0
#define CPLD1                  0x1
#define CPLD2                  0x2
#define CPLD3                  0x3

#define CPLD_SET_PORT0(bit_num)     cpld_select(bit_num,1)
#define CPLD_CLEAR_PORT0(bit_num)   cpld_select(bit_num,0)
#define CPLD_SET_PORT1(bit_num)     cpld_changeState(CPLD1,bit_num,1)
#define CPLD_CLEAR_PORT1(bit_num)   cpld_changeState(CPLD1,bit_num,0)
#define CPLD_SET_PORT2(bit_num)     cpld_changeState(CPLD2,bit_num,1)
#define CPLD_CLEAR_PORT2(bit_num)   cpld_changeState(CPLD2,bit_num,0)
#define CPLD_SET_PORT3(bit_num)     cpld_changeState(CPLD3,bit_num,1)
#define CPLD_CLEAR_PORT3(bit_num)   cpld_changeState(CPLD3,bit_num,0)

extern void cpld_changeState   (int cpld_port,int bit_num,int direction);
extern void cpld_select      (int bit_num,int direction);
extern int  cpld_read        (int cpld_port);
extern void cpld_doSelect(void);
extern void cpld_init(void);
extern int cpld_getVersion(void);
int arch_cpld_getVersion(void);

#endif
