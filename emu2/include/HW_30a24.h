/* 
*   HW_30a24.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_30A24_H
#define __HW_30A24_H

#include "emu.h"
#include "HW_access.h"


class HW_30a24 : public HW_access {
    public:
        HW_30a24();
                
        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size);
        
        int HDD_power;
        int power_connected;
        int usb_connected;
};

#endif // __HW_30A24_H
