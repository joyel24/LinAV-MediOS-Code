/* 
*   HW_dma.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_DMA_H
#define __HW_DMA_H

#include "emu.h"
#include "HW_access.h"
#include "HW_mem.h"

class HW_cpld;

class HW_dma : public HW_access {
    public:
        HW_dma(HW_mem * mem,HW_cpld * hw_cpld);
                
        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size);
        
        void init_ata_xfer(char * data,int data_ptr,int data_size);
        
    private:
        uint32_t dma_src;
        uint32_t dma_dst;
        uint32_t dma_size;
        int device_sel;
        int dma_endian;
        
        char * data;
        int data_ptr;
        int data_size;
        
        HW_mem * mem;
        HW_cpld * hw_cpld;
};

#endif // __HW_DMA_H
