/* 
*   HW_ECR.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_ECR_H
#define __HW_ECR_H

#include "emu.h"
#include "HW_access.h"


class HW_ECR : public HW_access {
    public:
        HW_ECR();
                
        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size);
        
    private:
        uint32_t bytes_swapped;
        uint32_t bits_swapped;
        uint32_t in_val;
};

#endif // __HW_ECR_H
