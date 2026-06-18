/*
*   HW_wdt.cpp
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

#include <HW_wdt.h>

#define DIV_FACTOR 1




HW_wdt::HW_wdt(HW_IRQ * HW_irq):HW_access(MEM_CFG_START,MEM_CFG_START,"mem_cfg")
{
   
    
    this->HW_irq=HW_irq;
    output_selection=0;
    enable_bit=0;
    pre_scalar=0;
    count_cur=count_ini=0;
}

uint32_t HW_wdt::read(uint32_t addr,int size)
{
    int val;
    switch(addr&0xF)
    {
        case 0x0: /* output selection + enable */
            val = (enable_bit&0x1) | ((output_selection&0x1)<<2);
            DEBUG_HW(TMR_HW_DEBUG,"%s WDTMD read @0x%08x, size %x => %s,%s\n",name,addr,size,
                enable_bit?"enable":"disable",output_selection?"INT+RESET MCU":"INT MCU");
            break;
        case 0x2: /* reset */
            DEBUG_HW(TMR_HW_DEBUG,"%s WDTRST read @0x%08x, size %x => reset should not be called\n",name,addr,size);
            val=0;
            break;
        case 0x4: /* prescalar */
            DEBUG_HW(TMR_HW_DEBUG,"%s WDTPRSCL read @0x%08x, size %x => prescalar=%x\n",name,addr,size,pre_scalar);
            val=pre_scalar;
            break;
        case 0x6: /* count */
            DEBUG_HW(TMR_HW_DEBUG,"%s WDTVAL read @0x%08x, size %x => count ini=%x cur=%x\n",name,addr,size,
                count_ini,count_cur);
            val=count_cur;
            break;        
        default:
            DEBUG_HW(TMR_HW_DEBUG,"%s UKN write @0x%08x, size %x\n",name,addr,size);
            break;
    }
    return val;
}

void HW_wdt::write(uint32_t addr,uint32_t val,int size)
{
    switch(addr&0xF)
    {
        case 0x0: /* output selection + enable */
            enable_bit=val&1;
            output_selection=(val>>2)&0x1;
            
            if(enable_bit)
            {
                count_cur=count_ini;
                tm_ps=0;
            }
            DEBUG_HW(TMR_HW_DEBUG,"%s WDTMD write @0x%08x, size %x => %s,%s\n",name,addr,size,
                enable_bit?"enable":"disable",output_selection?"INT+RESET MCU":"INT MCU");
            break;
        case 0x2: /* reset */
            count_cur=count_ini;
            DEBUG_HW(TMR_HW_DEBUG,"%s WDTRST write @0x%08x, size %x => %s\n",name,addr,size,val&0x1?"reset":"no Reset");
            break;
        case 0x4: /* prescalar */
            pre_scalar=val;
            DEBUG_HW(TMR_HW_DEBUG,"%s WDTPRSCL write @0x%08x, size %x => prescalar=%x\n",name,addr,size,pre_scalar);
            break;
        case 0x6: /* count */
            count_cur=count_ini=val;
            DEBUG_HW(TMR_HW_DEBUG,"%s WDTVAL write @0x%08x, size %x => count ini=%x cur=%x\n",name,addr,size,
                count_ini,count_cur);            
            break;
        
        default:
            DEBUG_HW(TMR_HW_DEBUG,"%s UKN write @0x%08x, size %x\n",name,addr,size);
            break;
    }
}

void HW_wdt::nxt_cycle(void)
{
    if(enable_bit)
    {
        tm_ps++;
        if(tm_ps >= pre_scalar)
        {
            tm_ps = 0;
            count_cur--;
            if(count_cur==0)
            {
                HW_irq->do_IRQ_FIQ(FIQ,WDT_FIQ);
                count_cur=count_ini;
            }
        }
    }
}
