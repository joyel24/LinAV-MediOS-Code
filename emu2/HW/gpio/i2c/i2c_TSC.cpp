/* 
*   i2c_TSC.cpp
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
#include <i2c_TSC.h>

char * tsc_str[] = { "TEMP0","Vbat1","IN1","UKN","TEMP1","Vbat2","IN2","UKN",
                    "ActivateX-","ActivateY-","ActivateY+,X-","UKN",
                    "X pos","Y pos","Z1 pos","Z2 pos"};

i2c_TSC::i2c_TSC(void):i2c_device(0x90,"TSC-BAT")
{
    //printf("i2c_TSC constructor for %x:%s %x\n",this,name,address);
    wait_cmd=true;
    do_write=false;
}

int i2c_TSC::read(void)
{
    int val=0;
    switch(step)
    {
        case 0:
            val = 0xFF;
            step++;
            break;
        case 1:
            if(M)
            {
                DEBUG_HW("I2C - TSC - ERROR : we are in 8 bit mode\n");
                val = 0;
            }
            else
                val = 0xf;
            step++;
            break;
        default:
            DEBUG_HW("I2C - TSC - ERROR : should not be here: address = %d\n",step);
            val = 0;
            break;
    }
    
    return val;
}

void i2c_TSC::write(int val)
{
    if(wait_cmd)
    {
        wait_cmd = false;
        cmd = (val>>4)&0xF;
        param = (val>>2)&0x3;
        M=(val>>1)&0x1;
        DEBUG_HW("I2C - TSC - get cmd %s(%x), param: %x, mode: %x\n",tsc_str[cmd],cmd,param,M);
        step = 0;
    }
    else
    {
        DEBUG_HW("I2C - TSC - Should not be here\n");
    }
}
        
void i2c_TSC::start(int direction)
{
    if(direction)
    {
        wait_cmd = false;
    }
    else
    {
        wait_cmd = true;
    }
}

void i2c_TSC::stop(void)
{
    
}
