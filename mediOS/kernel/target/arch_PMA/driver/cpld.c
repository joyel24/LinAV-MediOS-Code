/*
*   kernel/target/arch_AV4XX/cpld.c
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
    printk("nb 1 : %X\n",cpld_read(CPLD0) & 0x000F);
    printk("nb 2 : %X\n",cpld_read(CPLD1) & 0x000F);
    printk("nb 3 : %X\n",cpld_read(CPLD2) & 0x000F);
    printk("nb 4 : %X\n",cpld_read(CPLD3) & 0x000F);
    printk("nb 5 : %X\n",cpld_read(CPLD4) & 0x000F);
    return (cpld_read(CPLD4) & 0x000F);
}

