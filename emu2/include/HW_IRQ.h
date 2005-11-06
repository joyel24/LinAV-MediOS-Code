/* 
*   HW_IRQ.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/



#ifndef __HW_IRQ_H
#define __HW_IRQ_H

#include "emu.h"
#include "HW_access.h"

#define FIQ  0x0
#define IRQ  0x1


class HW_IRQ : public HW_access {
    public:
        HW_IRQ();
                
        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size);
        
        bool have_int_IRQ;
        bool have_int_FIQ;
        
        int do_IRQ_cmd(int type,int num);
        void do_IRQ_FIQ(int type,int num);
        
    private:
        int status[NUM_OF_IRQ];
        int entry[NUM_OF_IRQ];
        int enable[NUM_OF_IRQ];
        
};

#endif // __HW_IRQ_H
