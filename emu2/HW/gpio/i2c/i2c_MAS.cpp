/* 
*   i2c_MAS.cpp
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
#include <i2c_MAS.h>

i2c_MAS::i2c_MAS(void):i2c_device(0x3c,"MAS")
{
    //printf("i2c_MAS constructor for %x:%s %x\n",this,name,address);
}

int i2c_MAS::read(void)
{
    return 0;
}

void i2c_MAS::write(int val)
{

}
        
void i2c_MAS::start(int direction)
{

}

void i2c_MAS::stop(void)
{
    
}
