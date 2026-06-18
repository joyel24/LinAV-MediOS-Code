/* 
*   HW_dsp.cpp
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

#include <HW_dsp.h>

HW_dsp::HW_dsp(void):HW_access(DSP_START,DSP_END,"dsp_arm")
{
    
}

uint32_t HW_dsp::read(uint32_t addr,int size)
{
    DEBUG_HW(DSP_HW_DEBUG,"%s read @0x%08x, size %x\n",name,addr,size);
    return 0;
}

void HW_dsp::write(uint32_t addr,uint32_t val,int size)
{
    DEBUG_HW(DSP_HW_DEBUG,"%s write %x @0x%08x, size %x\n",name,val,addr,size);
}
