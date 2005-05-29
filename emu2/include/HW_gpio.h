/* 
*   HW_gpio.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_GPIO_H
#define __HW_GPIO_H

#include "emu.h"
#include "HW_access.h"


class HW_gpio : public HW_access {
    public:
        HW_gpio(void);
                
        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size);
    private:
        int DIR_0,DIR_1,INV_0,INV_1;
        int FSEL,BITRATE;
        int IRQPORT;
        
        /* using variables for now */
        int BITSET_0,BITSET_1,BITCLR_0,BITCLR_1;
};

#endif // __HW_GPIO_H
