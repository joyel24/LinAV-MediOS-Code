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

#include <HW_gpio.h>

HW_gpio::HW_gpio(void):HW_access(0x30580,0x30593,"GPIO")
{
    DIR_0    = 0xFFFF;
    DIR_1    = 0xFFFF;
    INV_0    = 0x0;
    INV_1    = 0x0;
    IRQPORT  = 0x0;
    FSEL     = 0x0;
    BITRATE  = 0x0;
    BITSET_0 = 0x0;
    BITSET_1 = 0x0;
    BITCLR_0 = 0x0;
    BITCLR_1 = 0x0;
}

uint32_t HW_gpio::read(uint32_t addr,int size)
{    
    switch(addr)
    {
        case 0x30580:            
            break;
        case 0x30582:
            break;
        case 0x30584:
            break;
        case 0x30586:
            break;
        case 0x30588:
            break;
        case 0x3058A:
            break;
        case 0x3058C:
            break;
        case 0x3058E:
            break;
        case 0x30590:
            break;
        case 0x30592:
            break;
        case 0x30594:
            break;
    }
    
    return 0;
}

void HW_gpio::write(uint32_t addr,uint32_t val,int size)
{
    switch(addr)
    {
        case 0x30580:
            
            break;
        case 0x30582:
            break;
        case 0x30584:
            break;
        case 0x30586:
            break;
        case 0x30588:
            break;
        case 0x3058A:
            break;
        case 0x3058C:
            break;
        case 0x3058E:
            break;
        case 0x30590:
            break;
        case 0x30592:
            break;
        case 0x30594:
            break;
    }
}
