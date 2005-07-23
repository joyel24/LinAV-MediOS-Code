/* 
*   i2c_device.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __I2C_DEVICE_H
#define __I2C_DEVICE_H

#include <emu.h>

class i2c_device;

#include <i2c_gpio.h>

class i2c_device {
    public:
        i2c_device(int address,char * name,HW_gpio * gpio);
        
        virtual int read(void);
        virtual void write(int val);
        
        virtual void start(int direction);
        virtual void stop(void);
        
        i2c_device * nxt;
        
        int address;
        char * name;
        
        HW_gpio * gpio;
};

#endif /* __I2C_DEVICE_H */
