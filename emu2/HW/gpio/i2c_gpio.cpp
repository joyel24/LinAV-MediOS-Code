/* 
*   i2c_gpio.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stdlib.h>
#include <stdio.h>

#include "emu.h"
#include "HW_gpio.h"

#include "i2c_gpio.h"

char * master_str[2] = { "CLK", "DA" };

i2c_gpio::i2c_gpio(int num):gpio_port(num)
{
    DEBUG_HW("Creating I2C master %s (%x)\n",master_str[num-0x12],num);
    this->name = master_str[num-0x12];    
    new_state = old_state = state = 1;
}

i2c_gpio::i2c_gpio(int num,char * name):gpio_port(num)
{
    DEBUG_HW("Creating I2C master %s (%x)\n",master_str[num-0x12],num);
    this->name = master_str[num-0x12];
    new_state = old_state = state = 1;
}

bool i2c_gpio::is_set(void)
{
    //DEBUG_HW("GPIO%x reading state: %s\n",gpio_num,state?"SET":"CLR");
    return state;
}

void i2c_gpio::set_gpio(void)
{
    old_state = new_state;
    new_state = state = 1;
    DEBUG_HW("Setting %s\n",name);
}

void i2c_gpio::clear_gpio(void)
{
    old_state = new_state;
    new_state = state = 0;
    DEBUG_HW("Clearing %s\n",name);
}

void i2c_gpio::gpio_dir_chg(int dir)
{
    DEBUG_HW("Chg dir of %s to %s\n",name,dir?"input":"output");
}

i2C_master::i2C_master(HW_gpio * gpio)
{
    clk = new i2c_gpio(0x12);
    data = new i2c_gpio(0x13);
    
    gpio->register_port(0x12,(gpio_port *)clk);
    gpio->register_port(0x13,(gpio_port *)data);
    
}
