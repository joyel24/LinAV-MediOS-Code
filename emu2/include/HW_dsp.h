/* 
*   HW_dsp.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_DSP_H
#define __HW_DSP_H

#include "emu.h"
#include "HW_access.h"

#define DSP_START 0x30600
#define DSP_END 0x30610

class HW_dsp : public HW_access {
    public:
        HW_dsp(void);
                
        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size);
};

#endif // __HW_MEM_H
