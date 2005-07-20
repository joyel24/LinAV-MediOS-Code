/* 
*   mem_space.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __MEM_SPACE_H
#define __MEM_SPACE_H

#include "emu.h"
#include "HW_access.h"
#include <bkpt_list.h>
#include <HW_OSD.h>
#include <HW_cpld.h>
//#include <HW_TI.h>

class HW_TI;

class mem_space:public HW_access {
    public:
        mem_space(char * flash,char * sdram);
        ~mem_space();        
        
        int do_cmd_dump(int argc,char ** argv);
        int do_cmd_add_bk_mem(int argc,char ** argv);
        
        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size);
        
        void set_OSD(HW_OSD * hw_osd);
        
        HW_TI * hw_TI;
        HW_cpld * hw_cpld;
        
    private:
        bkpt_list * bkpt;
        HW_OSD * hw_OSD;
    
       
};

#endif
