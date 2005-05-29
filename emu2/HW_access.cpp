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
}

uint32_t HW_access::read(uint32_t addr,int size)
{
    for(HW_access * ptr=zone_list;ptr!=NULL;ptr=ptr->nxt)
        if(addr>=ptr->start && addr<=(ptr->end-size+1))
            return ptr->read(addr,size);
                
    printf("%s::read(0x%08x,%d) ERROR addr does not belong to a known zone\n",name,addr,size);
    if(exit_on_not_match)
        exit(0);
    return 0;
}

void HW_access::write(uint32_t addr,uint32_t val,int size)
{
    for(HW_access * ptr=zone_list;ptr!=NULL;ptr=ptr->nxt)
        if(addr>=ptr->start && addr<=(ptr->end-size+1))
        {
            ptr->write(addr,val,size);
            return;
        }

    printf("%s::write(0x%08x,%x,%d) ERROR addr does not belong to a known zone\n",name,addr,val,size);
    if(exit_on_not_match)
        exit(0);             
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
