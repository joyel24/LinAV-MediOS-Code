
/* 
*   HW_IRQ.cpp
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

#include <HW_IRQ.h>

char * str_irq_fiq[] = {
    "FIQ0",
    "FIQ1",
#ifdef Gmini400
    "FIQ2",
    "null",
#endif
    "IRQ0",
    "IRQ1",
#ifdef Gmini400
    "IRQ2"
#endif
};


HW_IRQ * irq_obj;

#include "irq_cmd_line_fct.h"

HW_IRQ::HW_IRQ(void):HW_access(IRQ_START,IRQ_END,"IRQ/FIQ")
{
    init_irq_static_fct(this);
    for(int i=0;i<NUM_OF_IRQ;i++)
    {
        status[i]=0x0;
        entry[i]=0;
        enable[i]=0;
    }
    have_int_IRQ = false;
    have_int_FIQ = false;
}

uint32_t HW_IRQ::read(uint32_t addr,int size)
{
    int ret_val = 0;
    int num=0;
    int mod_val=0xFFFF;
    switch(addr)
    {
        case BASE_STATUS + FIQ_0:
        case BASE_STATUS + IRQ_0:         //mod_val=0xFFF7; 
        case BASE_STATUS + FIQ_1:
        case BASE_STATUS + IRQ_1:
#ifdef Gmini400
        case BASE_STATUS + FIQ_2:
        case BASE_STATUS + IRQ_2:	
#endif
            num = (addr - BASE_STATUS)/2; //addr - BASE_ENTRY == FIQ_0 or FIQ_1 or ...
            ret_val=status[num];
            status[num] &= 0xFFFF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read %s STATUS @0x%08x, size %x send %x\n",name,str_irq_fiq[num],addr,size,ret_val);            
            break;
        case BASE_ENTRY + FIQ_0:
        case BASE_ENTRY + FIQ_1:
        case BASE_ENTRY + IRQ_0:
        case BASE_ENTRY + IRQ_1:
#ifdef Gmini400        
        case BASE_ENTRY + FIQ_2:
        case BASE_ENTRY + IRQ_2:
#endif
            num = (addr - BASE_ENTRY)/2;
            ret_val=entry[num];
            DEBUG_HW(IRQ_HW_DEBUG,"%s read %s ENTRY @0x%08x, size %x send %x\n",name,str_irq_fiq[num],addr,size,ret_val);            
            break;
        case BASE_ENABLE + FIQ_0:
        case BASE_ENABLE + FIQ_1:        
        case BASE_ENABLE + IRQ_0:
        case BASE_ENABLE + IRQ_1:
#ifdef Gmini400         
        case BASE_ENABLE + IRQ_2:
        case BASE_ENABLE + FIQ_2:
#endif
            num = (addr - BASE_ENABLE)/2;
            ret_val=enable[num];
            DEBUG_HW(IRQ_HW_DEBUG,"%s read %s ENABLE @0x%08x, size %x send %x\n",name,str_irq_fiq[num],addr,size,ret_val);            
            break;
        default:
            ret_val=0;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read @0x%08x, size %x\n",name,addr,size);
            break;
    }
    return ret_val;
}

void HW_IRQ::write(uint32_t addr,uint32_t val,int size)
{
    int num=0;
    switch(addr)
    {
        case BASE_STATUS+FIQ_0:
        case BASE_STATUS+FIQ_1:
        case BASE_STATUS+IRQ_0:
        case BASE_STATUS+IRQ_1:
#ifdef Gmini400
        case BASE_STATUS + FIQ_2:
        case BASE_STATUS + IRQ_2:	
#endif
            num = (addr - BASE_STATUS)/2;
            status[num]=(status[num]|val)&0xFFFF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to %s STATUS @0x%08x, size %x\n",
                name,val,str_irq_fiq[num],addr,size);            
            break;
        case BASE_ENTRY+FIQ_0:
        case BASE_ENTRY+FIQ_1:
        case BASE_ENTRY+IRQ_0:
        case BASE_ENTRY+IRQ_1:
#ifdef Gmini400
        case BASE_ENTRY + FIQ_2:
        case BASE_ENTRY + IRQ_2:	
#endif
            num = (addr - BASE_ENTRY)/2;
            entry[num]=val&0xFFFF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to %s ENTRY @0x%08x, size %x\n",
                name,val,str_irq_fiq[num],addr,size);            
            break;
        case BASE_ENABLE+FIQ_0:
        case BASE_ENABLE+FIQ_1:
        case BASE_ENABLE+IRQ_0:
        case BASE_ENABLE+IRQ_1:
#ifdef Gmini400
        case BASE_ENABLE + FIQ_2:
        case BASE_ENABLE + IRQ_2:	
#endif
            num = (addr - BASE_ENABLE)/2;
            enable[num]=val&0xFFFF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to %s ENABLE @0x%08x, size %x\n",
                name,val,str_irq_fiq[num],addr,size);            
            break;
        default:
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to @0x%08x, size %x\n",
                name,val,addr,size);
            break;
    }
}

void HW_IRQ::do_IRQ_FIQ(int type,int num)
{
    status[(type==FIQ?0:NUM_OF_IRQ/2)+REG_NUM(num)] &= ((~(0x1<<REAL_NUM(num))) & 0xFFFF);
    DEBUG_HW(IRQ_HW_DEBUG,"%s Doing %s num = %x\n",name,type==FIQ?"FIQ":"IRQ",num);
    if(type==FIQ)
    {
        have_int_FIQ = true;
        if(chkFiqFlag())
            cur_irq_fct = cpu_do_fiq;
    }
    else
    {
        have_int_IRQ = true;
        if(chkIrqFlag())
            cur_irq_fct = cpu_do_irq;
    }
}
