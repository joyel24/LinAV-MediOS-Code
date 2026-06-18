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

#include <emu.h>
#include <i2c_gpio.h>
#include <i2c_device.h>
#include <i2c_gpio.h>

i2c_device::i2c_device(int address,char * name,HW_gpio * gpio)
{
    //printf("i2c_device constructor %x\n",this);
    this->nxt=NULL;
    this->address=address;
    this->name=name;
}

int i2c_device::read(void)
{
    printf("WARN general I2C read\n");
}

void i2c_device::write(int val)
{
    printf("WARN general I2C write\n");
}
        
void i2c_device::start(int direction)
{
    printf("WARN general I2C start\n");
}

void i2c_device::stop(void)
{
    printf("WARN general I2C stop\n");
}
