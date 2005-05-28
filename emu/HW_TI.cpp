/* 
*   HW_TI.cpp
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

#include "HW_TI.h"
#include "HW_null.h"

#include "HW_uart.h"

HW_TI::HW_TI():HW_access(0x30000,0x3FFFF,"DSC25")
{
    exit_on_not_match = false;
    add_item(new HW_null(0x30680,0x30700,"OSD"));
    add_item(new HW_uart(0x30300,0x30310,"UART0"));
    add_item(new HW_uart(0x30380,0x30390,"UART1"));
}

HW_TI::~HW_TI()
{
    
}

/*uint32_t HW_TI::read(uint32_t addr,int size)
{
    printf("DSC25 HW read @0x%08x, size %x\n",addr,size);
    return 0;
}

void HW_TI::write(uint32_t addr,uint32_t val,int size)
{
    printf("DSC25 HW write %x @0x%08x, size %x\n",val,addr,size);
}
*/
