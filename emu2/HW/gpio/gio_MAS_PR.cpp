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

#include <gio_MAS_PR.h>
#include <i2c_MAS.h>


gio_MAS_PR::gio_MAS_PR(i2c_MAS * mas):gpio_port(GPIO_MAS_PR,"MAS_PR")
{    
    this->mas=mas;
}

void gio_MAS_PR::set_gpio(void)
{
    state = 1;
    mas->set_PR();
}

void gio_MAS_PR::clear_gpio(void)
{
    state = 0;
    mas->clr_PR();
}

