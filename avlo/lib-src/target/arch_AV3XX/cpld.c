/*
*   lib/target/arch_AV3XX/cpld.c
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stddef.h>

#include <io.h>
#include <hardware.h>
#include <cpld.h>

int arch_cpld_get_version(void){
    return (cpld_read(CPLD2) & 0x000F) | ((cpld_read(CPLD3) & 0x0007)<<4);
}

