/* 
*   HW_clock.cpp
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

#include <HW_clock.h>

HW_clock::HW_clock(void):HW_access(0x30880,0x3088C,"CLOCK")
{
    
}

uint32_t HW_clock::read(uint32_t addr,int size)
{
    DEBUG_HW("%s read @0x%08x, size %x\n",name,addr,size);
    switch(addr)
    {
        case 0x30880:
            return 0x8000;
        case 0x30882:
            return 0x8000;
        case 0x30884:
            return 0x8000;
        default:
            break;
    }    
    
    return 0;
}

void HW_clock::write(uint32_t addr,uint32_t val,int size)
{
    DEBUG_HW("%s write %x @0x%08x, size %x\n",name,val,addr,size);
}
