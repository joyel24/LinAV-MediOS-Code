/* 
*   i2c_DVR.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __I2C_DVR_H
#define __I2C_DVR_H

#include <emu.h>
#include <i2c_device.h>

#define MAX_REG 0x100

class i2c_DVR:i2c_device {
    public:
        i2c_DVR(HW_gpio * gpio);
        
        int read(void);
        void write(int val);
        
        void start(int direction);
        void stop(void);
        
        i2c_device * nxt;
        
        int address;
        char * name;
    private:
        bool has_address;
        int index;
        
        int dvr_reg[MAX_REG];
};

#endif /* __I2C_DVR_H */
