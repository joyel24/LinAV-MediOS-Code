/*
*   HW_cpld.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_CPLD_H
#define __HW_CPLD_H

#include <stdlib.h>
#include <stdio.h>

#include "emu.h"
#include "HW_access.h"
#include "HW_30a24.h"

#include <HW_ON_OFF.h>
#include <HW_gpio.h>

#define BTN_UP    0x0
#define BTN_LEFT  0x1
#define BTN_RIGHT 0x2
#define BTN_DOWN  0x3
#define BTN_F3    0x4
#define BTN_F1    0x5
#define BTN_F2    0x6
#define BTN_JOY   0x7
#define BTN_ON    0x8
#define BTN_OFF   0x9
#define BTN_OK    0xa

#define BTN_INIT_VAL  0x2

class HW_cpld:public HW_access {
    public:
        HW_cpld();
        ~HW_cpld();  
        
        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size); 
#ifdef HAS_HW_30A24
        void set30A24(HW_30a24 * hw_30a24);
#endif
        void setONOFF(HW_gpio * gpio);
        
        void setModule(int module);
        
        
        int do_cmd_btn(int btn);
        
        int btn_var[0xb];
        
        HW_ON_OFF * ON_btn;
        HW_ON_OFF * OFF_btn;
        
    private:
#ifdef HAS_HW_30A24
        HW_30a24 * hw_30a24;  
#endif
        int cpld_ata_mode;
        int cpld_module_type;
        
        int cpld2_val; 
        
        int fw_connected; 
        int hdd_on;
        int bck_light;
        int ide_reset;
};

#endif
