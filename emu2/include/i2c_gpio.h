/* 
*   i2c_gpio.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __I2C_GPIO_H
#define __I2C_GPIO_H

#include "emu.h"
#include "HW_gpio.h"

class i2c_gpio : gpio_port {
    public:
        i2c_gpio(int num);
        i2c_gpio(int num,char * name);        
        bool is_set(void);
        void set_gpio(void);
        void clear_gpio(void);
        void gpio_dir_chg(int dir);
        
        int new_state;
        int old_state;
        
    private:
        int state;
        int gpio_num;
        
};

class i2C_master {
    public:
        i2C_master(HW_gpio * gpio);
        
    private:
        i2c_gpio * clk;
        i2c_gpio * data;
};

#endif // __I2C_GPIO_H
