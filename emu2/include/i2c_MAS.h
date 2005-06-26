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

class i2c_MAS:i2c_device {
    public:
        i2c_MAS(void);
        
        int read(void);
        void write(int val);
        
        void start(int direction);
        void stop(void);
        
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
              
        HW_mem * D0_ram;
        HW_mem * D1_ram;
        
};

#endif /* __I2C_MAS_H */
