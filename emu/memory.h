/* 
*   memory.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __MEMORY_H
#define __MEMORY_H

#include "emu.h"

class HW_Zone {

};

enum MEM_TYPE {HARDWARE,MEMORY};

struct MemZone {
    uint32_t        start;
    uint32_t        end;
    enum MEM_TYPE   type;
    char *          name;
    char *          mem;
    class HW_zone * hw;
};

class Memory {
    public:
        Memory(char * flash,char * sdram);
        ~Memory();
    
        uint32_t read_m(uint32_t addr,int size);
        void write_m(uint32_t addr,uint32_t val,int size); 
    private:
        struct MemZone * mem_zone;
        char * flash_file;
        char * sdram_file;
        void reset_init(void);
        void flash_init(void);
        void sdram_init(void); 
};

#endif // __MEMORY_H
