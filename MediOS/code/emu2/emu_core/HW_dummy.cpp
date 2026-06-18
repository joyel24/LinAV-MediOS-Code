/*
*   hw_dummy.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stdio.h>
#include <stdlib.h>

#include <emu.h>

#include <HW_dummy.h>

HW_dummy::HW_dummy(char * name):HW_access(0,0,name)
{
}

uint32_t HW_dummy::read(uint32_t addr,int size)
{
    DEBUG_HW(MEM_HW_DEBUG,"%s::read(0x%08x,%d) ERROR addr does not belong to a known zone\n",name,addr,size);
    return 0;
}

void HW_dummy::write(uint32_t addr,uint32_t val,int size)
{
    DEBUG_HW(MEM_HW_DEBUG,"%s::write(0x%08x,%x,%d) ERROR addr does not belong to a known zone\n",name,addr,val,size);
}

