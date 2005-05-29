/* 
*   mem_space.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <stdlib.h>
#include <stdio.h>

#include <HW_cpld.h>


HW_cpld::HW_cpld(void):HW_access(0x02000000,0x02ffffff,"CPLD")
{
    exit_on_not_match = false;
}

HW_cpld::~HW_cpld()
{
    
}

uint32_t HW_cpld::read(uint32_t addr,int size)
{
    printf("CPLD read @0x%08x, size %x\n",addr,size);
    return 0;
}

void HW_cpld::write(uint32_t addr,uint32_t val,int size)
{
    printf("CPLD write %x @0x%08x, size %x\n",val,addr,size);
}
