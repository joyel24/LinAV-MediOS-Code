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

#include <HW_ON_OFF.h>



HW_ON_OFF::HW_ON_OFF(int num):gpio_port(0)
{
    this->gpio_num=(num==ON_GPIO?GPIO_ON_NUM:GPIO_OFF_NUM);
    this->name = (num==ON_GPIO?"ON":"OFF");
    state = 1;  /* default => btn not pressed */
}

void HW_ON_OFF::set_gpio(void)
{

}

void HW_ON_OFF::clear_gpio(void)
{

}

