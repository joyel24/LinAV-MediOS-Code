/* 
*   HW_wdt.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_WDT_H
#define __HW_WDT_H

#include "emu.h"
#include "HW_access.h"
#include "HW_IRQ.h"

class HW_wdt : public HW_access {
    public:
        HW_wdt(HW_IRQ * HW_irq);
                
        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size);
        void HW_wdt::nxt_cycle(void);
                
    private:
        
        int output_selection;
        int enable_bit;
        int pre_scalar;
        int count_cur;
        int count_ini;
        int tm_ps;
        HW_IRQ * HW_irq;
};

#endif // __HW_WDT_H
