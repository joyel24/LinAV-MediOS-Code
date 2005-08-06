/*
*   hw_node.cpp
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

#include <HW_node.h>

HW_node::HW_node(uint32_t start,uint32_t end, uint32_t granularity_shift, char * name):HW_access(start,end,name)
{
    this->start = start;
    this->end = end;
    this->granularity_shift = granularity_shift;
    this->name = name;


    dummy_hw = new HW_dummy(name);

    int length=(end-start)>>granularity_shift;

    access_array = new HW_access * [length];

    for (int i=0;i<length;++i)
      access_array[i]=dummy_hw;

    exit_on_not_match = true;
    data_abt_on_not_match = false;
    full_size = false;
}

extern bool data_abort;

uint32_t HW_node::read(uint32_t addr,int size)
{
    HW_access * acc = access_array[(addr-start)>>granularity_shift];

    return (size<4 || acc->full_size)?acc->read(addr,size):((acc->read(addr,2) | acc->read(addr+2,2) <<16));
}

void HW_node::write(uint32_t addr,uint32_t val,int size)
{
    HW_access * acc = access_array[(addr-start)>>granularity_shift];
    if (size<4 || acc->full_size){
      acc->write(addr,val,size);
    }else{
      acc->write(addr,val&0xffff,2);
      acc->write(addr+2,val>>16,2);
    }
}

void HW_node::add_item(HW_access * HW)
{
//    printf("%s %s %08x %08x\n",name,HW->name,HW->start,HW->end);

    int i=(HW->start-start) >> granularity_shift;
    int end=(HW->end-start) >> granularity_shift;

    if ((HW->end-start) & (0xffffffff>>(32-granularity_shift))) ++end;

    do{
        access_array[i]=HW;
//        printf("%d ",i);
        ++i;
    }while(i<end);
//    printf("\n",i);
}

/*
void HW_node::rm_item(HW_access * HW)
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
  */
