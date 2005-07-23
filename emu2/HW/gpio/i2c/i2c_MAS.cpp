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

#define MAS_CMD_STR(CMD)   (CMD==0x6a?"CONTROL":       \
                            CMD==0x76?"DCCF":          \
                            CMD==0x77?"DCFR":          \
                            CMD==0x68?"DATA WRITE":    \
                            CMD==0x69?"DATA READ":     \
                            CMD==0x6C?"CODEC WRITE":   \
                            CMD==0x6D?"CODEC READ":    \
                            "UKN!!!!"                  \
                            )
                            
#include "mas_codec_reg_name.h"

i2c_MAS::i2c_MAS(HW_gpio * gpio):i2c_device(0x3c,"MAS",gpio)
{
    //printf("i2c_MAS constructor for %x:%s %x\n",this,name,address);
    control_reg=0x3000;
    DCCF_reg=0x5050;
    DCFR_reg=0x0;
    
    for(int i=0;i<0x30;i++)
        codec_reg[i]=0;
    
    codec_reg[0x0c] = 0x500;
    codec_reg[0x0d] = 0x500;
    
    D0_ram = new HW_mem(NULL,0x0,0x1000,"MAS_D0");
    D1_ram = new HW_mem(NULL,0x0,0x1000,"MAS_D1");
    
    D0_ram->write(0x7F0,0xa0264,4);
    D0_ram->write(0x7F1,0x124,4);
    D0_ram->write(0x7F2,0x5,4);
    D0_ram->write(0x7F3,0x18432,4);
    D0_ram->write(0x7F4,0x80000,4);
    D0_ram->write(0x7F8,0x8200,4);
    D0_ram->write(0x7FC,0x80000,4);
    D0_ram->write(0x7FF,0x80000,4);
    
    cur_p_data = 0;
    for(int i=0;i<8;i++)
    {
        gio_p_data[i] = new gio_MAS_data(i,this);
        gpio->register_port(i+0x8,gio_p_data[i]);
        
    }
    gio_pr = new gio_MAS_PR(this);
    gpio->register_port(0x1F,gio_pr);
    gio_rtr = new gpio_port(0x1E,"MAS_RPR");
    gpio->register_port(0x1E,gio_rtr);
    gio_eod = new gio_MAS_EOD();
    gpio->register_port(0x04,gio_eod);
    gio_pw = new gio_MAS_PW();
    gpio->register_port(0x10,gio_pw);
}

#define MAS_READ_REG(REG)              \
    {                                  \
        if(index==0)                   \
        {                              \
            ret_val=(REG>>8)&0xFF;  \
        }                              \
        else if(index == 1)            \
        {                              \
            ret_val=REG&0xFF;   \
            DEBUG_HW(MAS_HW_DEBUG,"MAS %s read => %x\n",MAS_CMD_STR(cmd),REG);       \
        }                              \
    }

    
#define MAS_WRITE_REG(REG,STEP)              \
    {                                  \
        if(index == (1+(STEP)))                 \
        {                              \
            mas_data = val&0xFF;       \
        }                              \
        else if(index == (2+(STEP)))            \
        {                              \
            mas_data = mas_data << 8 | (val&0xFF);  \
            REG=mas_data;      \
            DEBUG_HW(MAS_HW_DEBUG,"MAS %s write %x\n",MAS_CMD_STR(cmd),mas_data);   \
        }                              \
    }

void i2c_MAS::set_p_data(int num,int val)
{
    int mask = (~(0x1<<num))&0xFF;
    cur_p_data = cur_p_data&mask;
    if(val)
        cur_p_data |= (0x1<<num);
    cur_p_data &= 0xFF;
}

void i2c_MAS::set_PR(void)
{
    //printf("Raising PR\n");
    gio_rtr->set_gpio();
}

void i2c_MAS::clr_PR(void)
{
    //printf("clear latch (PR)\n");
#if 1
    gio_eod->cnt--;
    
    if(gio_eod->cnt<=0)
    {
        gio_eod->state=0;  /*emulate MAS buffer full */ 
        //printf("EOD 0\n");
        gio_eod->cnt_loop=0x600;
        gio_eod->cnt = 0;
    }
#endif    
    gio_rtr->clear_gpio();
}

        
/* MAS version in revers : 3587 01 02 */
#define MAS_VER     0x02018735
    
int i2c_MAS::read(void)
{
    int ret_val=0;
    switch(cmd)
    {
        case 0x6A:
            MAS_READ_REG(control_reg);
            break;
        case 0x76:
            MAS_READ_REG(DCCF_reg);
            break;
        case 0x77:
            MAS_READ_REG(DCFR_reg);
            break;
        case 0x68:
        case 0x69:
            switch((reg_addr>>12)&0xF)
            {
                case 0x7:
                    ret_val = (MAS_VER >> (index*8)) & 0xFF;
                    if(index == 3)
                        DEBUG_HW(MAS_HW_DEBUG,"MAS %s: version read\n",MAS_CMD_STR(cmd));
                    break;
                case 0xA:
                    DEBUG_HW(MAS_HW_DEBUG,"MAS %s: reg %x read %x\n",MAS_CMD_STR(cmd),xfer_addr,0);
                    ret_val=0;
                    break;
                case 0xC:
                case 0xD:
                {
                    //printf("read %x %x\n",(reg_addr>>8)&0xf,reg_addr);
                    int dest=(reg_addr>>12) & 0x1;
                    if(((reg_addr>>8)&0xf)==4) /* short read */
                    {
                        ret_val = (mas_data >> ((1-(index%2))*8) )&0xff;
                        if(index%2 == 1)
                        {
                            DEBUG_HW(MAS_HW_DEBUG,"MAS reading: %x at %x (left %x)\n",mas_data&0xFFFF,xfer_addr,xfer_size-1);
                            xfer_size--;
                            xfer_addr++;
                            if(xfer_size)
                                mas_data = (dest?D1_ram->read(xfer_addr,2):D0_ram->read(xfer_addr,2))&0xFFFF;
                        }
                    }
                    else
                    {
                        ret_val = (mas_data >> ((3-(index%4))*8) )&0xff;
                        if(index%4 == 3)
                        {
                            DEBUG_HW(MAS_HW_DEBUG,"MAS reading: %x at %x (left %x)\n",mas_data,xfer_addr,xfer_size-1);
                            xfer_size--;
                            xfer_addr++;
                            if(xfer_size)
                                mas_data = (dest?D1_ram->read(xfer_addr,4):D0_ram->read(xfer_addr,4))&0xFFFFF;
                        }
                    }
                }
                    break;
            }            
            break;
        case 0x6C:
        case 0x6D:
            /*DEBUG_HW(MAS_HW_DEBUG,"MAS %s (%x) - send reg %s (%x) : %x\n",
                    MAS_CMD_STR(cmd),cmd,codec_reg_name[reg_addr],reg_addr,codec_reg[reg_addr]);*/
            if(index==0)
            {
                ret_val=(codec_reg[reg_addr]>>8)&0xFF;
            }
            else if(index == 1)
            {
                ret_val=codec_reg[reg_addr]&0xFF;
                DEBUG_HW(MAS_HW_DEBUG,"MAS %s read %s (%x) => %x\n",
                    MAS_CMD_STR(cmd),codec_reg_name[reg_addr],reg_addr,codec_reg[reg_addr]);
            }      
            break;
        default:
            printf("MAS wrong subaddress : %x\n",cmd);
    }    
    
    index++;
    return ret_val;
}

void i2c_MAS::write(int val)
{
    if(index == 0)
    {        
        cmd = val;
        //DEBUG_HW(MAS_HW_DEBUG,"MAS new sub: %s (%x)\n",MAS_CMD_STR(cmd),cmd);
        //reg_addr=0;
    }
    else
    {
        switch(cmd)
        {
            case 0x6A:
                MAS_WRITE_REG(control_reg,0);
                break;
            case 0x76:
                MAS_WRITE_REG(DCCF_reg,0);
                break;
            case 0x77:
                MAS_WRITE_REG(DCFR_reg,0);
                break;
            case 0x68:
            case 0x69:
                if(index == 1)
                {
                    reg_addr = val&0xFF;
                }
                else if(index == 2)
                {
                    reg_addr = (reg_addr<<8) | val & 0xFF;                    
                }
                else
                {
                    switch((reg_addr>>12)&0xF)
                    {
                        case 0x0:
                        case 0x1:
                        case 0x2:
                        case 0x3:
                            if(reg_addr == 0)
                            {
                                DEBUG_HW(MAS_HW_DEBUG,"MAS %s: FREEZE\n",MAS_CMD_STR(cmd));
                            }
                            else
                            {
                                DEBUG_HW(MAS_HW_DEBUG,"MAS %s: Start code at %x\n",MAS_CMD_STR(cmd),reg_addr);
                            }
                            break;
                        case 0x5:
                            DEBUG_HW(MAS_HW_DEBUG,"MAS %s: SYNC\n",MAS_CMD_STR(cmd));
                            break;
                        case 0x6:
                            if(index == 3)
                            {
                                xfer_addr = val &0xFF;
                            }
                            else if(index == 4)
                            {
                                xfer_addr = (xfer_addr << 8) | (val & 0xFF);
                                xfer_size = reg_addr & 0xFFF;
                                DEBUG_HW(MAS_HW_DEBUG,"MAS %s: fast prog dwld at %x, size=%x\n",MAS_CMD_STR(cmd),xfer_addr,xfer_size);
                            }
                            break;
                        case 0xA:
                            if(index == 3)
                            {
                                mas_data = val & 0xFF;
                            }
                            else if(index == 4)
                            {
                                xfer_addr = (reg_addr && 0xFF0) >> 4;
                                mas_data = ((reg_addr & 0xF) << 16) | (mas_data<<8) | (val&0xFF);
                                DEBUG_HW(MAS_HW_DEBUG,"MAS %s: reg %x write %x\n",MAS_CMD_STR(cmd),xfer_addr,mas_data);
                            }
                            break;
                        case 0xC:
                        case 0xD:
                            if(index == 3)
                            {
                                xfer_size = val & 0xFF;
                            }
                            else if(index == 4)
                            {
                                xfer_size = (xfer_size<<8) | (val & 0xFF);
                            }
                            else if(index == 5)
                            {
                                xfer_addr = val & 0xFF;
                            }
                            else if(index == 6)
                            {
                                int dest=(reg_addr>>12) & 0x1;
                                int type=((reg_addr>>8)&0xf)==4;
                                xfer_addr = (xfer_addr<<8) | (val & 0xFF);
                                DEBUG_HW(MAS_HW_DEBUG,"MAS %s: starting %s xfer read  from %s @%x size=%x (sub_cmd=%x)\n",
                                    MAS_CMD_STR(cmd),type?"short":"norm",dest?"D1":"D0",xfer_addr,xfer_size,reg_addr);
                                mas_data = (dest?D1_ram->read(xfer_addr,4):D0_ram->read(xfer_addr,4))&0xFFFFF;
                            }
                            break;    
                        case 0xE:
                        case 0xF:
                        {
                            int dest=(reg_addr>>12) & 0x1;
                            int type=((reg_addr>>8)&0xf)==4;
                            if(index == 3)
                            {
                                xfer_size = val & 0xFF;
                            }
                            else if(index == 4)
                            {
                                xfer_size = (xfer_size<<8) | (val & 0xFF);
                            }
                            else if(index == 5)
                            {
                                xfer_addr = val & 0xFF;
                            }
                            else if(index == 6)
                            {
                                xfer_addr = (xfer_addr<<8) | (val & 0xFF);
                                DEBUG_HW(MAS_HW_DEBUG,"MAS %s: starting %s xfer write to %s @%x size=%x (sub_cmd=%x)\n",
                                    MAS_CMD_STR(cmd),type?"short":"norm",dest?"D1":"D0",xfer_addr,xfer_size,reg_addr);
                                mas_data=0;
                            }
                            else
                            {
                                if(type)
                                {
                                    mas_data = (mas_data << 8) | (val & 0xFF);
                                    if((index-7)%2 == 1)
                                    {
                                        DEBUG_HW(MAS_HW_DEBUG,"MAS writting: %x at %x (left %x)\n",mas_data,xfer_addr,xfer_size-1);
                                        
                                        if(xfer_addr==0x7F1)
                                            mas_data &= 0xFFFFE;
                                        
                                        if(dest)
                                            D1_ram->write(xfer_addr,mas_data&0xFFFFF,4);
                                        else
                                            D0_ram->write(xfer_addr,mas_data&0xFFFFF,4);
                                            
                                        if(xfer_addr == 0x7F6 & !dest)
                                        {                                            
                                            D0_ram->write(0x7F7,mas_data&0x4C,4);
                                        }
                                        xfer_size--;
                                        xfer_addr++;
                                        mas_data=0;                                        
                                    }
                                }
                                else
                                {
                                    mas_data = (mas_data << 8) | (val & 0xFF);
                                    if((index-7)%4 == 3)
                                    {
                                        DEBUG_HW(MAS_HW_DEBUG,"MAS writting: %x at %x (left %x)\n",mas_data,xfer_addr,xfer_size-1);
                                        
                                        if(xfer_addr==0x7F1)
                                            mas_data &= 0xFFFFE;
                                        
                                        if(dest)
                                            D1_ram->write(xfer_addr,mas_data&0xFFFFF,4);
                                        else
                                            D0_ram->write(xfer_addr,mas_data&0xFFFFF,4);
                                            
                                        if(xfer_addr == 0x7F6 & !dest)
                                        {
                                            D0_ram->write(0x7F7,mas_data&0xFFFFF,4);
                                        }
                                        xfer_size--;
                                        xfer_addr++;
                                        mas_data=0;
                                    }
                                }
                            }
                        }
                    }
                }                
                break;
            case 0x6C:
            case 0x6D:
                if(index == 1)
                {
                    reg_addr = val&0xFF;
                }
                else if(index == 2)
                {
                    reg_addr = (reg_addr<<8) | val & 0xFF;
                }
                else if(index == 3)
                {
                    mas_data = val&0xFF;
                }
                else if(index == 4)
                {
                    mas_data = mas_data << 8 | (val&0xFF);
                    codec_reg[reg_addr]=mas_data;
                    DEBUG_HW(MAS_HW_DEBUG,"MAS %s reg %s (%x) set to %x\n",MAS_CMD_STR(cmd),codec_reg_name[reg_addr],reg_addr,mas_data);
                }             
                break;
            default:
                printf("MAS wrong subaddress : %x\n",cmd);
        }        
    }
    index++;
}
        
void i2c_MAS::start(int direction)
{
    index = 0;
}

void i2c_MAS::stop(void)
{
    
}
