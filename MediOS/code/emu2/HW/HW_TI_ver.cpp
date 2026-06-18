/* 
*   HW_null.cpp
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

#include <HW_TI_ver.h>



HW_TI_ver::HW_TI_ver():HW_access(TI_VER_START,TI_VER_END,"TI version")
{
    
}

uint32_t HW_TI_ver::read(uint32_t addr,int size)
{
    //DEBUG_HW(NULL_HW_DEBUG,"%s read @0x%08x, size %x\n",name,addr,size);
    return TI_VER;
}

void HW_TI_ver::write(uint32_t addr,uint32_t val,int size)
{
    //DEBUG_HW(NULL_HW_DEBUG,"%s write %x @0x%08x, size %x\n",name,val,addr,size);
}
