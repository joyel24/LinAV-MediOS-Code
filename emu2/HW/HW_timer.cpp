/* 
*   HW_timer.cpp
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

#include <HW_timer.h>


uint32_t timer_start[] = {
    0x30000,
    0x30080,
    0x30100,
    0x30180
};


HW_timer::HW_timer(int timer_num):HW_access(0,0,"TMR")
{
    this->start     = timer_start[timer_num];
    this->end       = timer_start[timer_num]+0xC;
    this->timer_num = timer_num;
    
    zone_list = NULL;
    nxt = NULL;
    
    timer_num=0;
    
    tm_mode=0;
    tm_clk=0;
    tm_p_scaler=0;
    tm_max=0;
    tm_trigger=0;
    tm_cnt=0;
    
}

uint32_t HW_timer::read(uint32_t addr,int size)
{
    switch(addr&0xF)
    {
        case 0x0:
            DEBUG_HW(TMR_HW_DEBUG,"%s%d read mode=%s\n",name,timer_num,
                (tm_mode&0x3)==0x0?"Stop":
                (tm_mode&0x3)==0x1?"One-Shot":
                (tm_mode&0x3)==0x2?"Free-run":"RESERVED");
            return (tm_mode&0xFF);
        case 0x2:
            DEBUG_HW(TMR_HW_DEBUG,"%s%d read ClockSelect(%x)=%s\n",name,timer_num,tm_clk&0x1,tm_clk&0x1?"Ext or SYSCLK clock":"Current ARM clock");
            return (tm_clk&0x1);
        case 0x4:
            DEBUG_HW(TMR_HW_DEBUG,"%s%d read Prescaler divide value=%x\n",name,timer_num,tm_p_scaler&0x3FF);
            return (tm_p_scaler&0x3FF);
        case 0x6:
            DEBUG_HW(TMR_HW_DEBUG,"%s%d read Max counter val=%x\n",name,timer_num,tm_max&0xFFFF);
            return (tm_max&0xFFFF);
        case 0x8:
            DEBUG_HW(TMR_HW_DEBUG,"%s%d read Timer Triger => SHOULD NOT READ\n",name,timer_num);
            return 0;
        case 0xA:
            DEBUG_HW(TMR_HW_DEBUG,"%s%d read Timer count val=%x\n",name,timer_num,tm_cnt&0xFFFF);
            return (tm_cnt&0xFFFF);
        default:
            DEBUG_HW(TMR_HW_DEBUG,"%s%d UKN read @0x%08x, size %x\n",name,timer_num,addr,size);
    }
    return 0;
}

void HW_timer::write(uint32_t addr,uint32_t val,int size)
{
    switch(addr&0xF)
    {
        case 0x0:
            DEBUG_HW(TMR_HW_DEBUG,"%s%d write mode(%x)=%s\n",name,timer_num,val&0xFF,
                (val&0x3)==0x0?"Stop":
                (val&0x3)==0x1?"One-Shot":
                (val&0x3)==0x2?"Free-run":"RESERVED");
            tm_mode= val&0xFF;
            break;
        case 0x2:
            DEBUG_HW(TMR_HW_DEBUG,"%s%d write ClockSelect(%x)=%s\n",name,timer_num,val&0x1,(val&0x1)==0x1?"Ext or SYSCLK clock":"Current ARM clock");
            tm_clk=val&0x1;
            break;
        case 0x4:
            DEBUG_HW(TMR_HW_DEBUG,"%s%d write Prescaler divide value=%x\n",name,timer_num,val&0x3FF);
            tm_p_scaler=val&0x3FF;
            break;
        case 0x6:
            DEBUG_HW(TMR_HW_DEBUG,"%s%d write Max counter val=%x\n",name,timer_num,val&0xFFFF);
            tm_max=val&0xFFFF;
            break;
        case 0x8:
            DEBUG_HW(TMR_HW_DEBUG,"%s%d write Timer Triger => %s\n",name,timer_num,(val&0x1)==0x1?"GO":"NO GO");
            tm_trigger=val&0x1;
            break;
        case 0xA:
            DEBUG_HW(TMR_HW_DEBUG,"%s%d write Timer count => SHOULD NOT READ\n",name,timer_num);
            break;
        default:
            DEBUG_HW(TMR_HW_DEBUG,"%s%d UKN write @0x%08x, size %x\n",name,timer_num,addr,size);
            break;
    }
}
