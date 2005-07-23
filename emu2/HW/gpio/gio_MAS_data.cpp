/* 
*   HW_ON_OFF.cpp
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

#include <gio_MAS_data.h>
#include <i2c_MAS.h>

char * gio_mas_data_str[] = { "MAS-D0", "MAS-D1", "MAS-D2", "MAS-D3", "MAS-D4", "MAS-D5", "MAS-D6", "MAS-D7" };



gio_MAS_data::gio_MAS_data(int num,i2c_MAS * mas):gpio_port(num+0x8)
{    
    this->name = gio_mas_data_str[num];
    this->mas=mas;
}

void gio_MAS_data::set_gpio(void)
{
    state = 1;
    //printf("MAS-D%d set\n",gpio_num-0x8);
    mas->set_p_data(gpio_num-0x8,1);
}

void gio_MAS_data::clear_gpio(void)
{
    state = 0;
    //printf("MAS-D%d set\n",gpio_num-0x8);
    mas->set_p_data(gpio_num-0x8,0);
}

