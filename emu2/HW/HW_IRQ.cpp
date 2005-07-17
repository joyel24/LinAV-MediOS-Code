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

#define FIQ_0    0x0
#define FIQ_1    0x2
#define IRQ_0    0x4
#define IRQ_1    0x6

#define BASE_STATUS   0x30500
#define BASE_ENTRY    0x30508
#define BASE_ENABLE   0x30520

char * str_irq_fiq[] = {
    "FIQ0",
    "FIQ1",
    "IRQ0",
    "IRQ1"
};

#define REG_NUM(irq) (irq<16?0:1)
#define REAL_NUM(irq) (irq<16?irq:irq-16)

HW_IRQ * irq_obj;

#include "irq_cmd_line_fct.h"

HW_IRQ::HW_IRQ(void):HW_access(0x30500,0x3055E,"IRQ/FIQ")
{
    init_irq_static_fct(this);
    for(int i=0;i<4;i++)
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
        case BASE_STATUS + IRQ_0:
            mod_val=0xFFF7;
        case BASE_STATUS + FIQ_1:
        case BASE_STATUS + IRQ_1:
            num = (addr - BASE_STATUS)/2;
            ret_val=status[num];
            status[num] &= mod_val;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read %s STATUS @0x%08x, size %x send %x\n",name,str_irq_fiq[num],addr,size,ret_val);            
            break;
        case BASE_ENTRY + FIQ_0:
        case BASE_ENTRY + FIQ_1:
        case BASE_ENTRY + IRQ_0:
        case BASE_ENTRY + IRQ_1:
            num = (addr - BASE_ENTRY)/2;
            ret_val=entry[num];
            DEBUG_HW(IRQ_HW_DEBUG,"%s read %s ENTRY @0x%08x, size %x send %x\n",name,str_irq_fiq[num],addr,size,ret_val);            
            break;
        case BASE_ENABLE + FIQ_0:
        case BASE_ENABLE + FIQ_1:
        case BASE_ENABLE + IRQ_0:
        case BASE_ENABLE + IRQ_1:
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
            num = (addr - BASE_STATUS)/2;
            status[num]=(status[num]|val)&0xFFFF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to %s STATUS @0x%08x, size %x\n",
                name,val,str_irq_fiq[num],addr,size);            
            break;
        case BASE_ENTRY+FIQ_0:
        case BASE_ENTRY+FIQ_1:
        case BASE_ENTRY+IRQ_0:
        case BASE_ENTRY+IRQ_1:
            num = (addr - BASE_ENTRY)/2;
            entry[num]=val&0xFFFF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to %s ENTRY @0x%08x, size %x\n",
                name,val,str_irq_fiq[num],addr,size);            
            break;
        case BASE_ENABLE+FIQ_0:
        case BASE_ENABLE+FIQ_1:
        case BASE_ENABLE+IRQ_0:
        case BASE_ENABLE+IRQ_1:
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
    status[(type==FIQ?0:2)+REG_NUM(num)] &= ((~(0x1<<REAL_NUM(num))) & 0xFFFF);
    DEBUG_HW(IRQ_HW_DEBUG,"%s Doing %s num = %x\n",name,type==FIQ?"FIQ":"IRQ",num);
    if(type==FIQ)
        have_int_FIQ = true;
    else
        have_int_IRQ = true; 
}
