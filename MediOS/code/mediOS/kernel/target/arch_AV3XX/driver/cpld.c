/*
*   kernel/target/arch_AV3XX/cpld.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/cpld.h>

int arch_cpld_getVersion(void){
    return (cpld_read(CPLD2) & 0x000F) | ((cpld_read(CPLD3) & 0x0007)<<4);
}

