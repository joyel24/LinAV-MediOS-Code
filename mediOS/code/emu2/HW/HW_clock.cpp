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

HW_clock::HW_clock(void):HW_access(CLOCK_START,CLOCK_END,"CLOCK")
{
    
}

uint32_t HW_clock::read(uint32_t addr,int size)
{
    int ret_val = 0;
    switch(addr)
    {
        case CLOCK_START+0x0:
            ret_val = 0x8000;
            break;
        case CLOCK_START+0x2:
            ret_val = 0x8000;
            break;
        case CLOCK_START+0x4:
            ret_val = 0x8000;
            break;
        default:
            break;
    }    
    DEBUG_HW(CLOCK_HW_DEBUG,"%s read @0x%08x, (size %x) => send %x\n",name,addr,size,ret_val);
    
    return ret_val;
}

void HW_clock::write(uint32_t addr,uint32_t val,int size)
{
    DEBUG_HW(CLOCK_HW_DEBUG,"%s write %x @0x%08x, size %x\n",name,val,addr,size);
}
