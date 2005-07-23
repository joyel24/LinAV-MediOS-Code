/* 
*   HW_ON_OFF.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either exPWess of implied.
*/
#include <stdlib.h>
#include <stdio.h>

#include <gio_MAS_PW.h>
#include <i2c_MAS.h>


gio_MAS_PW::gio_MAS_PW(void):gpio_port(0x10,"MAS_PW")
{    
}

void gio_MAS_PW::set_gpio(void)
{
    printf("set PW\n");
    state = 1;
}

void gio_MAS_PW::clear_gpio(void)
{
    printf("clr PW\n");
    state = 0;
}

void gio_MAS_PW::gpio_dir_chg(int dir)
{
    printf("PW dir changed to %d\n",dir);
}

