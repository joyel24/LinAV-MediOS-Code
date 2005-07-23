/* 
*   i2c_MAS.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __I2C_MAS_H
#define __I2C_MAS_H

#include <emu.h>
#include <HW_mem.h>
#include <i2c_device.h>
#include <gio_MAS_data.h>
#include <gio_MAS_PR.h>
#include <HW_gpio.h>
#include <gio_MAS_EOD.h>
#include <gio_MAS_PW.h>

class i2c_MAS:i2c_device {
    public:
        i2c_MAS(HW_gpio * gpio);
        
        int read(void);
        void write(int val);
        
        void start(int direction);
        void stop(void);
        
        void set_p_data(int num,int val);
        
        void set_PR(void);
        void clr_PR(void);
        
        i2c_device * nxt;
        
        int address;
        char * name;
        
    private:
        int index;
        int cmd;
        int reg_addr;
        int xfer_size;
        int xfer_addr;
        
        int mas_data;
        
        int control_reg,DCCF_reg,DCFR_reg;
        
        int codec_reg[0x30];
        
        int cur_p_data;
              
        HW_mem * D0_ram;
        HW_mem * D1_ram;
        
        
        gio_MAS_data * gio_p_data[8];
        gio_MAS_PR * gio_pr;
        gpio_port * gio_rtr;
        gio_MAS_EOD * gio_eod;
        gio_MAS_PW * gio_pw;
        
        
};

#endif /* __I2C_MAS_H */
