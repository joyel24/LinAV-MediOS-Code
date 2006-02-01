/* 
*   HW_OSD.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_OSD_H
#define __HW_OSD_H

#include "emu.h"
#include "HW_access.h"

class HW_OSD;

#include <HW_lcd.h>
#include <HW_mem.h>

struct buffer_info {
    int x;
    int y;
    int width;
    int height;
};

class HW_OSD : public HW_access {
    public:
        HW_OSD(HW_mem * mem);
                
        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size);
        
        int do_cmd_updt_lcd(int argc,char ** argv);
        int do_cmd_cfg_out(int argc,char ** argv);
        
        void chk_access(uint32_t addr,uint32_t val,int size);
        int nxtEvent(void);
        
    
        int OSD_config_regs[6];
        int OSD_width_regs[4];
        uint32_t OSD_offset_regs[4];
        struct buffer_info OSD_info_regs[6];
        int OSD_main_shift_horiz;
        int OSD_main_shift_vert;        
        int OSD_pallette_bank1[8];
        int OSD_pallette_bank2[8];
        int OSD_cursor_data;
        int OSD_cursor_offset;
        int OSD_pallette_status;
        int OSD_pallette_data_wr;
        int OSD_pallette_index;
        uint32_t OSD_alt_vid_offset;
        
     private:   
        HW_lcd * lcd;
};

#endif // __HW_OSD_H
