/* 
*   HW_ECR.cpp
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

#include <HW_ECR.h>


HW_ECR::HW_ECR():HW_access(0x30900,0x30905,"ECR")
{
    bytes_swapped=0;
    bits_swapped=0;
    in_val=0;
}

uint32_t HW_ECR::read(uint32_t addr,int size)
{
    uint32_t ret_val=0;
    switch(addr)
    {
        case 0x30900:
            DEBUG_HW(ECR_HW_DEBUG,"%s INPUT read => %x (size %x)\n",name,in_val,size);
            ret_val=in_val;
            break;
        case 0x30902:
            DEBUG_HW(ECR_HW_DEBUG,"%s BYTES_SWAPPED read => %x (size %x)\n",name,bytes_swapped,size);
            ret_val=bytes_swapped;
            break;
        case 0x30904:
            DEBUG_HW(ECR_HW_DEBUG,"%s BITS_SWAPPED read => %x (size %x)\n",name,bits_swapped,size);
            ret_val=bits_swapped;
            break;
    }
    return ret_val;
}

void HW_ECR::write(uint32_t addr,uint32_t val,int size)
{
    if(addr == 0x30900)
    {
        uint32_t tmp_val;
        tmp_val = in_val = val;
        bytes_swapped = ((in_val & 0xFF) << 8) | ((in_val & 0xFF00) >> 8);
        bits_swapped = 0;        
        for(int i=0;i<16;i++)
        {
            bits_swapped = (bits_swapped << 1) | (tmp_val & 0x1);
            tmp_val=tmp_val >> 1;
        }
        DEBUG_HW(ECR_HW_DEBUG,"%s InPuT write %04x (size %x) => bytes=%04x , bits=%04x\n",name,in_val,size,bytes_swapped,bits_swapped);
    }
    else
        DEBUG_HW(ECR_HW_DEBUG,"%s BAD ADDR (@0x%08x) write %x (size %x)\n",name,addr,val,size);
    
}
