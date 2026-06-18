/* 
*   i2c_DVR.cpp
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

#include <i2c_DVR.h>


int i2c_DVR::read(void)
{
    int val = 0;
    if(index>=0 && index<=MAX_REG)
    {
        val=dvr_reg[index];
        DEBUG_HW(DVR_HW_DEBUG,"I2C - DVR - R (@%x) : %x\n",index,val);
        index++;        
    }
    else
        DEBUG_HW(DVR_HW_DEBUG,"I2C - DVR - R - Error bad address : %x\n",index);
    return val;
}

void i2c_DVR::write(int val)
{
    if(!has_address)
    {
        index = val;
        has_address = true;        
        DEBUG_HW(DVR_HW_DEBUG,"I2C -DVR - W - starting at : %x\n",index);
    }
    else
    {
        if(index>=0 && index<=MAX_REG)
        {
            dvr_reg[index]=val;
            DEBUG_HW(DVR_HW_DEBUG,"I2C - DVR - W (@%x) : %x\n",index,val);
            index++;
        }
        else
            DEBUG_HW(DVR_HW_DEBUG,"I2C -DVR - W - Error bad address : %x\n",index);
    }
}
        
void i2c_DVR::start(int direction)
{
    
}

void i2c_DVR::stop(void)
{
    has_address=false;
    DEBUG_HW(DVR_HW_DEBUG,"I2C -DVR - STOP\n");
}



i2c_DVR::i2c_DVR(HW_gpio * gpio):i2c_device(0x4A,"DVR",gpio)
{
    has_address=false;
    index=0;
    for(int i=0;i<0x100;i++)
        dvr_reg[i]=0;
}
