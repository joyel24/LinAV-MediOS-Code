/* 
*   HW_TI.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_TI_H
#define __HW_TI_H

#include "emu.h"
#include "HW_access.h"
#include "HW_30a24.h"
#include "HW_dma.h"
#include "mem_space.h"
#include "HW_IRQ.h"
#include "HW_mem.h"
#include "HW_cpld.h"
#include "HW_timer.h"


class HW_TI:public HW_access {
    public:
        HW_TI(mem_space * memSpace,HW_mem * mem,HW_cpld * hw_cpld);
        ~HW_TI();  
        HW_30a24 * hw_30a24;
        HW_dma * hw_dma;
        mem_space * memSpace;
        HW_IRQ * HW_irq;
        
        HW_timer * timer_list[4];
        
        /*uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size);      */
};

#endif
