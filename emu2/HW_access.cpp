/* 
*   hw_access.cpp
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

#include <HW_access.h>

HW_access::HW_access(uint32_t start,uint32_t end, char * name)
{
    this->start = start;
    this->end = end;
    this->name=name;
    
    zone_list = NULL;
    nxt = NULL;
    
    exit_on_not_match = true;
    data_abt_on_not_match = false;
    full_size = false;
}

extern bool data_abort;

uint32_t HW_access::read(uint32_t addr,int size)
{
    for(HW_access * ptr=zone_list;ptr!=NULL;ptr=ptr->nxt)
        if(addr>=ptr->start && addr<(ptr->end-size+1))
        {
            switch(size)
            {
                case 1:
                    return (ptr->read(addr,size)&0xFF);
                case 2:
                    return ptr->read(addr,size);
                case 4:
                    if(ptr->full_size)
                        return ptr->read(addr,size);
                    else
                        return ((ptr->read(addr,2)&0xFFFF) | ((ptr->read(addr+2,2)&0xFFFF)<<16));
            }
        }
                
    if(exit_on_not_match)
    {
        printf("%s::read(0x%08x,%d) ERROR addr does not belong to a known zone\n",name,addr,size);
        exit(0);
    }
    
    if(data_abt_on_not_match)
    {
        data_abort=true;
    }
    
    DEBUG_HW(MEM_HW_DEBUG,"%s::read(0x%08x,%d) ERROR addr does not belong to a known zone\n",name,addr,size);
    return 0;
}

void HW_access::write(uint32_t addr,uint32_t val,int size)
{
    for(HW_access * ptr=zone_list;ptr!=NULL;ptr=ptr->nxt)
        if(addr>=ptr->start && addr<(ptr->end-size+1))
        {
            switch(size)
            {
                case 1:
                    ptr->write(addr,val&0xFF,size);
                    break;
                case 2:
                    ptr->write(addr,val&0xFFFF,size);
                    break;
                case 4:
                    if(ptr->full_size)
                        ptr->write(addr,val,size);
                    else
                    {
                        ptr->write(addr,val&0xFFFF,size);
                        ptr->write(addr+2,(val>>16)&0xFFFF,size);
                    }
                    break;
            }
            return;
        }

    if(exit_on_not_match)
    {
        printf("%s::write(0x%08x,%x,%d) ERROR addr does not belong to a known zone\n",name,addr,val,size);          
        exit(0);   
    }
    
    if(data_abt_on_not_match)
    {
        data_abort=true;
    }
    
    DEBUG_HW(MEM_HW_DEBUG,"%s::write(0x%08x,%x,%d) ERROR addr does not belong to a known zone\n",name,addr,val,size);          
}

void HW_access::add_item(HW_access * HW)
{
    class HW_access * ptr=zone_list;
    
    if(zone_list == NULL || HW->start < zone_list->start)
    {
        HW->nxt=zone_list;
        zone_list = HW;
    }
    else
    {
        while(ptr->nxt!=NULL&& ptr->nxt->start<HW->start) ptr=ptr->nxt;
        HW->nxt=ptr->nxt;
        ptr->nxt=HW;
    }
}

void HW_access::rm_item(HW_access * HW)
{
    class HW_access * ptr=zone_list;
    if(zone_list==HW)
        zone_list=HW->nxt;
    else
    {
        while(ptr->nxt!=NULL && ptr->nxt != HW) ptr=ptr->nxt;
        if(ptr->nxt!=NULL) ptr->nxt = HW->nxt;
    }
}
