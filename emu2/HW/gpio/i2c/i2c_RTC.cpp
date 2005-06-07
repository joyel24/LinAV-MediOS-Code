/* 
*   i2c_RTC.cpp
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

#include <i2c_RTC.h>

i2c_RTC::i2c_RTC(void):i2c_device(0xD0,"RTC")
{
    //printf("i2c_RTC constructor for %x:%s %x\n",this,name,address);
    has_address=false;
    index=0;
    for(int i=0;i<0x14;i++)
        rtc_reg[i]=0;
}

int i2c_RTC::read(void)
{
    int val = 0;
    if(index>=0 && index<0x14)
    {
            val=rtc_reg[index];
            index++;
    }
    else
        DEBUG_HW("I2C - RTC - R - Error bad address : %x\n",index);
    return val;
}

void i2c_RTC::write(int val)
{
    if(!has_address)
    {
        index = val;
        has_address = true;
        DEBUG_HW("I2C -RTC - W - starting at : %x\n",index);
    }
    else
    {
        if(index>=0 && index<0x14)
        {
            rtc_reg[index]=val;
            index++;
        }
        else
            DEBUG_HW("I2C -RTC - W - Error bad address : %x\n",index);
    }
}
        
void i2c_RTC::start(int direction)
{
    
}

void i2c_RTC::stop(void)
{
    has_address=false;
}
