/* 
*   HW_clock.cpp
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

#include <HW_gpio.h>

char * gpio_str[] = { "ON" , "SPDIF/UART1_RX", "UKN", "UKN", "MAS_EOD", "UKN", "IR", "UKN",
                    "MAS_D0", "MAS_D1", "MAS_D2", "MAS_D3", "MAS_D4", "MAS_D5", "MAS_D6", "MAS_D7", 
                    
                    "MAS_PWR", "UKN", "I2C_CLK", "I2C_DA", "UKN", "CPLD_MOD_SENSE", "CPLD_SIGNAL", "BCK_LIGHT",
                    "UKN", "UKN", "UKN", "UKN", "OFF", "VIDEO/UART1_TX", "MAS_RTR", "MAS_PR"};


                    
HW_gpio::HW_gpio(void):HW_access(0x30580,0x30593,"GPIO")
{
    DIR_0    = 0xFFFF;
    DIR_1    = 0xFFFF;
    INV_0    = 0x0;
    INV_1    = 0x0;
    IRQPORT  = 0x0;
    FSEL     = 0x0;
    BITRATE  = 0x0;
    BITSET_0 = 0x0;
    BITSET_1 = 0x0;
    BITCLR_0 = 0x0;
    BITCLR_1 = 0x0;
       
    for(int i=0;i<32;i++)
        port_list[i]=new gpio_port(i);
    
}

uint32_t gpio_mask[]={  0X00000001, 0X00000002,0X00000004,0X00000008,
            0X00000010, 0X00000020,0X00000040,0X00000080,
            0X00000100, 0X00000200,0X00000400,0X00000800,
            0X00001000, 0X00002000,0X00004000,0X00008000,

            0X00010000, 0X00020000,0X00040000,0X00080000,
            0X00100000, 0X00200000,0X00400000,0X00800000,
            0X01000000, 0X02000000,0X04000000,0X08000000,
            0X10000000, 0X20000000,0X40000000,0X80000000};

gpio_port::gpio_port(int gpio_num)
{
    this->gpio_num=gpio_num;
    this->name = "UKN";
    state = 0;
}

gpio_port::gpio_port(int gpio_num,char * name)
{
    this->gpio_num=gpio_num;
    this->name = name;
    state = 0;
}
           
bool gpio_port::is_set(void)
{
    //DEBUG_HW("GPIO%x reading state: %s\n",gpio_num,state?"SET":"CLR");
    return state;
}

void gpio_port::set_gpio(void)
{
    state = 1;
    DEBUG_HW("GPIO%x is being set\n",gpio_num);
}

void gpio_port::clear_gpio(void)
{
    state = 0;
    DEBUG_HW("GPIO%x is being clear\n",gpio_num);
}

void gpio_port::gpio_dir_chg(int dir)
{
    DEBUG_HW("GPIO%x : dir chg to %s\n",gpio_num,dir?"input":"output");
}

void HW_gpio::register_port(int num,gpio_port * port)
{
    delete port_list[num];
    port_list[num] = port;
    //DEBUG_HW("Registering gpio port %x for %s\n",num,port->name);
}

#define IS_GPIO_SET(NUM)        (port_list[NUM]->is_set())
#define SET_GPIO(NUM)           {port_list[NUM]->set_gpio();}
#define CLR_GPIO(NUM)           {port_list[NUM]->clear_gpio();}
#define GPIO_DIR_CHG(NUM,DIR)   {port_list[NUM]->gpio_dir_chg(DIR);}
  
uint32_t HW_gpio::read(uint32_t addr,int size)
{    
    switch(addr)
    {
        case 0x30580:                                              /* DIR 0 */
            DEBUG_HW("GPIO read direction 0 : %04x\n",DIR_0);
            return DIR_0;
        case 0x30582:                                              /* DIR 1 */
            DEBUG_HW("GPIO read direction 1 : %04x\n",DIR_1);
            return DIR_1;
        case 0x30584:                                              /* INV 0 */
            DEBUG_HW("GPIO read inversion 0 : %04x\n",INV_0);
            return INV_0;
        case 0x30586:                                              /* INV 1 */
            DEBUG_HW("GPIO read inversion 1 : %04x\n",INV_1);
            return INV_1;
        case 0x30588:                                              /* SET 0 */
            {        
                uint32_t tmp_val =0;
                for(int k=0;k<0x10;k++)
                {
                    if(DIR_0 & gpio_mask[k])
                    {
                        int res=IS_GPIO_SET(k);
                        if((res && !(INV_0 & gpio_mask[k])) || (!res && (INV_0 & gpio_mask[k])))
                            tmp_val |= gpio_mask[k];
                    }
                
                }
                //DEBUG_HW("GPIO read state 0: %04x\n",tmp_val);
                return tmp_val;
            }
        case 0x3058A:                                              /* SET 1 */
            {        
                uint32_t tmp_val =0;
                for(int k=0;k<0x10;k++)
                {
                    if(DIR_1 & gpio_mask[k])
                    {
                        int res=IS_GPIO_SET(k+0x10);
                        if((res && !(INV_1 & gpio_mask[k])) || (!res && (INV_1 & gpio_mask[k])))
                            tmp_val |= gpio_mask[k];
                    }                
                }
                //DEBUG_HW("GPIO read state 1: %04x\n",tmp_val);
                return tmp_val;
            }
        case 0x3058C:                                              /* CLR 0 */
            {        
                uint32_t tmp_val =0;
                for(int k=0;k<0x10;k++)
                {
                    if(DIR_0 & gpio_mask[k])
                    {
                        int res=IS_GPIO_SET(k);
                        if((res && !(INV_0 & gpio_mask[k])) || (!res && (INV_0 & gpio_mask[k])))
                            tmp_val |= gpio_mask[k];
                    }
                
                }
                //DEBUG_HW("GPIO read state 0: %04x\n",tmp_val);
                return tmp_val;
            }
        case 0x3058E:                                              /* CLR 1 */
            {        
                uint32_t tmp_val =0;
                for(int k=0;k<0x10;k++)
                {
                    if(DIR_1 & gpio_mask[k])
                    {
                        int res=IS_GPIO_SET(k+0x10);
                        if((res && !(INV_1 & gpio_mask[k])) || (!res && (INV_1 & gpio_mask[k])))
                            tmp_val |= gpio_mask[k];
                    }
                
                }
                //DEBUG_HW("GPIO read state 1: %04x\n",tmp_val);
                return tmp_val;
            }
        case 0x30590:                                              /* IRQ */
            DEBUG_HW("GPIO read IRQ : %04x\n",IRQPORT);
            return IRQPORT;
            break;
        case 0x30592:                                              /* FSEL */
            DEBUG_HW("GPIO read FSEL : %04x\n",FSEL);
            return FSEL;
            break;
        case 0x30594:                                              /* BITRATE */
            DEBUG_HW("GPIO read BITRATE : %04x\n",BITRATE);
            return BITRATE;
            break;
    }
    
    return 0;
}

void HW_gpio::write(uint32_t addr,uint32_t val,int size)
{
    switch(addr)
    {
        case 0x30580:                                              /* DIR 0 */
            {        
                int tmp = val ^ DIR_0;          // compute what has changed
                int tmp2 = val;
                //DEBUG_HW("GPIO write DIR_0 (%04x): ",val);
                for(int k = 0; k<0x10;k++)
                {
                    if(tmp&0x1)
                    {
                        //DEBUG_HW("%s(GPIO%x)=> %s",gpio_str[k],k,(tmp2&0x1)?"READ":"WRITE");
                        if((tmp2&0x1)!=0x1) /* we are in write mode now */
                        {
                            if(BITSET_0 & gpio_mask[k])
                            {
                                //DEBUG_HW(" & SET");
                                SET_GPIO(k);
                            }
                            
                            if(BITCLR_0 & gpio_mask[k])
                            {
                                //DEBUG_HW(" & CLR");
                                CLR_GPIO(k);
                            }                         
                        }   
                        GPIO_DIR_CHG(k,tmp2&0x1);
                        //DEBUG_HW("|");                     
                    }
                    tmp=tmp >> 1;
                    tmp2=tmp2 >> 1;
                }
                DIR_0 = val & 0xFFFF;
                DEBUG_HW("\n");                
                break;
            }
        case 0x30582:                                              /* DIR 1 */
            {        
                int tmp = val ^ DIR_1;          // compute what has changed
                int tmp2 = val;
                //DEBUG_HW("GPIO write DIR_1 (%04x): ",val);
                for(int k = 0; k<0x10;k++)
                {
                    if(tmp&0x1)
                    {
                        //DEBUG_HW("%s(GPIO%x)=> %s",gpio_str[k+0x10],k+0x10,(tmp2&0x1)?"READ":"WRITE");
                        if((tmp2&0x1)!=0x1) /* we are in write mode now */
                        {
                            if(BITSET_1 & gpio_mask[k])
                            {
                                //DEBUG_HW(" & SET");
                                SET_GPIO(k+0x10);
                            }
                            
                            if(BITCLR_1 & gpio_mask[k])
                            {
                                //DEBUG_HW(" & CLR");
                                CLR_GPIO(k+0x10);
                            }
                            GPIO_DIR_CHG(k,tmp2&0x1);
                        }   
                        //DEBUG_HW("|");
                    }
                    tmp=tmp >> 1;
                    tmp2=tmp2 >> 1;
                }
                DIR_1 = val & 0xFFFF;
                DEBUG_HW("\n");
                break;
            }
        case 0x30584:                                              /* INV 0 */
            DEBUG_HW("GPIO write INV_0 : %04x change to %04x\n",INV_0,val&0xFFFF);
            INV_0=val&0xFFFF;
            break;
        case 0x30586:                                              /* INV 1 */
            DEBUG_HW("GPIO write INV_1 : %04x change to %04x\n",INV_1,val&0xFFFF);
            INV_1=val&0xFFFF;
            break;
        case 0x30588:                                              /* SET 0 */
            {
                //DEBUG_HW("GPIO set 0: %04x (with DIR mask:%08x): ",val,DIR_0);        
                //val &= ~DIR_0;        
                for(int k=0;k<0x10;k++)
                {
                    if(val & 0x1)
                    {
                        //DEBUG_HW("%s(GPIO%x)",gpio_str[k],k);
                        if(DIR_0 & gpio_mask[k])
                        {
                            BITSET_0 |=gpio_mask[k];
                            //DEBUG_HW("(delayed)");
                        }
                        else
                            SET_GPIO(k);
                        //DEBUG_HW("|");
                    }
                    val = val >> 1;                
                }
            }
            //DEBUG_HW("\n");
            break;
        case 0x3058A:                                              /* SET 1 */
            {
                //DEBUG_HW("GPIO set 1: %04x (with DIR mask:%08x): ",val,DIR_1);        
                //val &= ~DIR_1;        
                for(int k=0;k<0x10;k++)
                {
                    if(val & 0x1)
                    {
                        //DEBUG_HW("%s(GPIO%x)",gpio_str[k+0x10],k+0x10);
                        if(DIR_1 & gpio_mask[k])
                        {
                            BITSET_1 |=gpio_mask[k];
                            //DEBUG_HW("(delayed)");
                        }
                        else
                            SET_GPIO(k+0x10);
                        //DEBUG_HW("|");
                    }
                    val = val >> 1;                
                }
            }
            //DEBUG_HW("\n");
            break;
        case 0x3058C:                                              /* CLR 0 */
            {
                //DEBUG_HW("GPIO clr 0: %04x (with DIR mask:%08x): ",val,DIR_0);        
                //val &= ~DIR_0;        
                for(int k=0;k<0x10;k++)
                {
                    if(val & 0x1)
                    {
                        //DEBUG_HW("%s(GPIO%x)",gpio_str[k+0x10],k+0x10);
                        if(DIR_0 & gpio_mask[k])
                        {
                            BITCLR_0 |=gpio_mask[k];
                            //DEBUG_HW("(delayed)");
                        }
                        else
                            CLR_GPIO(k+0x10);
                        //DEBUG_HW("|");
                    }
                    val = val >> 1;                
                }
            }
            //DEBUG_HW("\n");
            break;
        case 0x3058E:                                              /* CLR 1 */
            {
                //DEBUG_HW("GPIO clr 1: %04x (with DIR mask:%08x): ",val,DIR_1);        
                //val &= ~DIR_1;        
                for(int k=0;k<0x10;k++)
                {
                    if(val & 0x1)
                    {
                        //DEBUG_HW("%s(GPIO%x)",gpio_str[k+0x10],k+0x10);
                        if(DIR_1 & gpio_mask[k])
                        {
                            BITCLR_1 |=gpio_mask[k];
                            //DEBUG_HW("(delayed)");
                        }
                        else
                            CLR_GPIO(k+0x10);
                        //DEBUG_HW("|");
                    }
                    val = val >> 1;                
                }
            }
            //DEBUG_HW("\n");
            break;
        case 0x30590:                                              /* IRQ */            
            IRQPORT=val & 0xFF;
            DEBUG_HW("GPIO write IRQ : %04x\n",IRQPORT);
            break;
        case 0x30592:                                              /* FSEL */            
            FSEL = val & 0x7FFF;
            DEBUG_HW("GPIO write FSEL : %04x\n",FSEL);
            break;
        case 0x30594:                                              /* BITRATE */
            BITRATE = val & 0xFFFF;
            DEBUG_HW("GPIO write BITRATE : %04x\n",BITRATE);
            break;
    }
}
