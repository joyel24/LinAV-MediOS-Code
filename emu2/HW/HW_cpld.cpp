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

#define CPLD_V_LOW
#define CPLD_V_HIGH

HW_cpld::HW_cpld(void):HW_access(0x02000000,0x02ffffff,"CPLD")
{
    exit_on_not_match = false;
}

HW_cpld::~HW_cpld()
{
    
}

uint32_t HW_cpld::read(uint32_t addr,int size)
{
    uint32_t ret_val;
    if(addr >= 0x02400000 && addr < 0x02400400 )
    {
        DEBUG_HW(CPLD_HW_DEBUG,"CPLD - read ATA (@0x%08x), size %x\n",addr,size);
        ret_val=0;        
    }
    else
    {
        switch(addr)
        {
            case 0x02600000:
            case 0x02600100:
            case 0x02600200:
            case 0x02600300:
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD - read CPLD (@0x%08x), size %x\n",addr,size);
                ret_val=0;
                break;
            case 0x02600680:
            case 0x02600700:
            case 0x02600780:
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD - read buttons (@0x%08x), size %x\n",addr,size);
                ret_val=0;
                break;
            default:
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD - read ERROR ukn addr: @0x%08x, size %x\n",addr,size);
                ret_val=0;
                break;
        }
    }
    return ret_val;
}

void HW_cpld::write(uint32_t addr,uint32_t val,int size)
{
    if(addr >= 0x02400000 && addr < 0x02400400 )
    {
        DEBUG_HW(CPLD_HW_DEBUG,"CPLD - write ATA (@0x%08x), size %x\n",addr,size);
    }
    else
    {
        switch(addr)
        {
            case 0x02600000:
            case 0x02600100:
            case 0x02600200:
            case 0x02600300:
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD - write CPLD (@0x%08x), size %x\n",addr,size);
                break;
            case 0x02600680:
            case 0x02600700:
            case 0x02600780:
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD - write buttons (@0x%08x), size %x\n",addr,size);
                break;
            default:
                DEBUG_HW(CPLD_HW_DEBUG,"CPLD - write ERROR ukn addr: @0x%08x, size %x\n",addr,size);
                break;
        }
    }
}
