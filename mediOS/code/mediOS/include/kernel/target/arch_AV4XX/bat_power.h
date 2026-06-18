/* 
*   include/kernel/target/arch_AV4XX/bat_power.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __ARCH_BAT_POWER_H
#define __ARCH_BAT_POWER_H

#include <kernel/io.h>
#include <kernel/gio.h>


/** power state **/

#define POWER_STATE                       0
#define POWER_CONNECTED                   (GIO_IS_SET(GIO_POWER_CONNECTED))

/** Bat level **/
#include <kernel/cpld.h>
#include <kernel/tsc2003.h>
#define GET_BAT_LEVEL                     (CPLD_VER==0x5?0:tsc2003getVal(CMD_BAT0|INTERNAL_ON))


#endif
