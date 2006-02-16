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

#include <i2c_TSC.h>
#include <i2c_MAS.h>
#include <i2c_RTC.h>
#include <i2c_DVR.h>

#define CL_OLD_HI  (clk->old_state)
#define CL_HI      (clk->new_state)

#define DA_OLD_HI  (data->old_state)
#define DA_HI      (data->new_state)

#define GET_ADDR    1
#define GET_DATA    2

//char * master_str[2] = { "CLK", "DA" };

i2c_gpio::i2c_gpio(int num,i2c_master * master):gpio_port(num)
{
    init_i2c_gpio(num,master);
}

i2c_gpio::i2c_gpio(int num,char * name,i2c_master * master):gpio_port(num)
{
    init_i2c_gpio(num,master);
}

void i2c_gpio::init_i2c_gpio(int num,i2c_master * master)
{
    printf("Creating I2C master %s (%x)\n",num==GPIO_I2C_SDA?"DA":"CLK",num);
    this->name = (num==GPIO_I2C_SDA?"DA":"CLK");
    new_state = old_state = state = 0;
    this->master=master;
}

bool i2c_gpio::is_set(void)
{
    //DEBUG_HW(I2C1_HW_DEBUG,"GPIO%x reading state: %s\n",gpio_num,state?"SET":"CLR");
    return state;
}

void i2c_gpio::set_gpio(void)
{
    /*old_state = state;
    new_state = state = 1;
    DEBUG_HW(I2C1_HW_DEBUG,"I2C %s HI (old = %s)\n",name,state?"HI":"LO");
    master->i2c_state_has_changed();
    old_state = new_state;*/
}

void i2c_gpio::clear_gpio(void)
{
    /*old_state = state;
    new_state = state = 0;
    DEBUG_HW(I2C1_HW_DEBUG,"I2C %s LO (old = %s)\n",name,state?"HI":"LO");
    master->i2c_state_has_changed();
    old_state = new_state;*/
}

void i2c_gpio::gpio_dir_chg(int dir)
{
    /*DEBUG_HW(I2C1_HW_DEBUG,"I2C GPIO Chg dir of %s to %s (cur state: %s\n",name,dir?"input":"output",state?"HI":"LO");
    if(dir && !state)
    {
        DEBUG_HW(I2C1_HW_DEBUG,"I2C %s HI (old = %s)\n",name,state?"HI":"LO");
        old_state = state;
        new_state = state = 1;
        master->i2c_state_has_changed();
        old_state = new_state;
    }*/
    master->i2c_state_has_changed();
}

void i2c_gpio::i2c_gpio_force_state(int state)
{
    this->state=state;
    old_state = new_state = state;
}

void i2c_gpio::i2c_gpio_set_state(int state)
{
    this->state=state;
}

i2c_master::i2c_master(HW_gpio * gpio)
{
    
    clk = new i2c_gpio(GPIO_I2C_SCL,this);
    data = new i2c_gpio(GPIO_I2C_SDA,this);
    
    this->gpio = gpio;
    
    gpio->register_port(GPIO_I2C_SCL,(gpio_port *)clk);
    gpio->register_port(GPIO_I2C_SDA,(gpio_port *)data);
       
    
    /* ini variables */
    
    active = false;
    cur_device = NULL;
    
    i2c_head=NULL;

#ifdef AV3XX
    register_i2c((i2c_device *)new i2c_MAS(gpio));
    register_i2c((i2c_device *)new i2c_TSC(gpio));
#endif

#ifdef AV1XX
    register_i2c((i2c_device *)new i2c_MAS(gpio));
#endif

    register_i2c((i2c_device *)new i2c_RTC(gpio));
    register_i2c((i2c_device *)new i2c_DVR(gpio));
    
    print_i2c_list();
    
}

void i2c_master::print_i2c_list(void)
{
    printf("I2C device list:\n");
    for(i2c_device * ptr=i2c_head;ptr!=NULL;ptr = ptr->nxt)
        printf("  %08x - %s at %x\n",ptr,ptr->name,ptr->address);
}

void i2c_master::register_i2c(i2c_device * device)
{
    printf("registering new device I2C: %x\n",device->address);
    device->nxt=i2c_head;
    i2c_head = device;
}

i2c_device * i2c_master::find_device(int address)
{
    i2c_device * ptr=i2c_head;
    while(ptr!=NULL && ptr->address != (address&0xFE))
        ptr = ptr->nxt;
    return ptr;
}

void i2c_master::i2c_state_has_changed(void)
{
    int sda_mask;
    int scl_mask;

    CL_OLD_HI = CL_HI;
    DA_OLD_HI = DA_HI;

    sda_mask=1<<GPIO_I2C_SDA;
    scl_mask=1<<GPIO_I2C_SCL;

    if (sda_mask>0xFFFF){
      DA_HI = (gpio->DIR_1 & (sda_mask>>16))!=0;
    }else{
      DA_HI = (gpio->DIR_0 & (sda_mask))!=0;
    }

    if (scl_mask>0xFFFF){
      CL_HI = (gpio->DIR_1 & (scl_mask>>16))!=0;
    }else{
      CL_HI = (gpio->DIR_0 & (scl_mask))!=0;
    }

    clk->i2c_gpio_set_state(CL_HI);
    data->i2c_gpio_set_state(DA_HI);


    if(CL_OLD_HI && CL_HI && DA_OLD_HI && !DA_HI)
    {
        DEBUG_HW(I2C2_HW_DEBUG,"I2C - START\n");
        active = true;
        do_ack = false;
        wait_ack = false;
        step = GET_ADDR;
        clock = 0;
        address = 0;
        cur_device = NULL;
        val = 0;
    }
    
    if(CL_OLD_HI && CL_HI && !DA_OLD_HI && DA_HI)
    {
        /*if(!wait_ack)
        {*/
        DEBUG_HW(I2C2_HW_DEBUG,"I2C - STOP\n");
        active = false;
        if(cur_device)
            cur_device->stop();
        /*}
        else
            DEBUG_HW("I2C - get ACK\n");*/
    }
    
    if(!CL_HI && CL_OLD_HI && wait_ack)
    {
        gpio->DIR_1 = (gpio->DIR_1 & ~0x8);
        //DEBUG_HW("I2C - chg dir: %x\n",gpio->DIR_1);
    }
    
       
    if(CL_HI && !CL_OLD_HI && active)
    {
        //wait_ack = false;
        if(do_ack)
        {
            DEBUG_HW(I2C2_HW_DEBUG,"I2C - send ACK\n");
            data->i2c_gpio_force_state(0x0);
            do_ack = false;
        }
        else if(wait_ack)
        {
            if(DA_HI)
            {
                DEBUG_HW(I2C2_HW_DEBUG,"I2C - get NACK\n");
                active = false;
            }
            else
            {
                DEBUG_HW(I2C2_HW_DEBUG,"I2C - get ACK\n");
            }
            wait_ack = false;
        }
        else
        {
            switch(step)
            {
                case GET_ADDR:
                    if(clock>8)
                    {
                        printf("error\n");
                        exit(0);
                    }
                    
                    //printf("%d - get %x\n",clock,DA_HI);
                    
                    address = (address<<1)|DA_HI;
                    clock++;
                    
                    if(clock==8)
                    {
                        DEBUG_HW(I2C2_HW_DEBUG,"I2C - addr = 0x%02x | %s",address,(address&0x1)?"READ":"WRITE");
                        //exit(0);
                        cur_device = find_device(address);
                        if(cur_device)
                        {
                            DEBUG_HW(I2C2_HW_DEBUG," found %s\n",cur_device->name);
                            cur_device->start(address&0x1);
                        }
                        else
                        {
                            DEBUG_HW(I2C2_HW_DEBUG," no devcice found\n");
                            
                        }
                        clock = 0;
                        step = GET_DATA;
                        sav_val = val = 0;
                        do_ack = true;
                    }                    
                    break;
                case GET_DATA:
                    //DEBUG_HW("I2C - GET_DATA\n");
                    if(address&0x1)   /* read */
                    {
                        if(clock == 0)
                        {
                            if(cur_device)
                                sav_val = val = cur_device->read();
                            else
                                sav_val = val = 0;
                        }
                        
                        data->i2c_gpio_force_state(((val >> (7-clock))&0x1));
                        //DEBUG_HW(I2C2_HW_DEBUG,"sending %x clk:%d|%d val=%x\n",val,clock,7-clock,((val >> (7-clock) )&0x1));
                        //val = val >> 1;
                        clock++;
                        //wait_ack = true;
                    }
                    else              /* write */
                    {
                        val = (val<<1) | DA_HI;
                        clock++;
                    }
                    
                    if(clock == 8) /* a block of data has been xfered */
                    {                        
                        if(address&0x1)
                        {
                            wait_ack = true;                            
                            DEBUG_HW(I2C2_HW_DEBUG,"I2C - READ %x ... done\n",sav_val);
                        }
                        else
                        {
                            DEBUG_HW(I2C2_HW_DEBUG,"I2C - WRITE %x ... done\n",val);
                            do_ack = true;
                            if(cur_device)
                               cur_device->write(val);
                        }
                        sav_val = val = 0;
                        clock = 0;
                    }
                    break;
            }
        }
    }
    
    /*if(!CL_HI && CL_OLD_HI)
    {
        wait_ack = false;
        DEBUG_HW(I2C2_HW_DEBUG,"I2C - WaitAck done\n");
    }*/
    
}
