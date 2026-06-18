/*
*   HW_MemCfg.cpp
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
#include <string.h>

#include <HW_MemCfg.h>

HW_MemCfg::HW_MemCfg(void):HW_access(MEM_CFG_START,MEM_CFG_STOP,"mem_cfg")
{
   memset(data,0,sizeof(data));
}

uint32_t HW_MemCfg::read(uint32_t addr,int size)
{
    addr -= MEM_CFG_START;
    return (data[addr]|(data[addr+2]<<8));    
}

void HW_MemCfg::write(uint32_t addr,uint32_t val,int size)
{
    addr -= MEM_CFG_START;
    data[addr]=(val&0xFF);
    data[addr+2]=((val>>8)&0xFF);
}

