/* 
*   HW_TI.cpp
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

#include <HW_TI.h>
#include <HW_null.h>

#include <HW_mem.h>

#include <HW_uart.h>
#include <HW_clock.h>
#include <HW_gpio.h>
#include <HW_timer.h>
#include <i2c_gpio.h>


#include <HW_30a24.h>
#include <HW_cpld.h>
#include <HW_TI_ver.h>
#include <HW_ECR.h>
#include <HW_OSD.h>
#include <HW_IRQ.h>
#include <HW_access.h>

HW_TI::HW_TI(mem_space * memSpace,HW_mem * mem,HW_cpld * hw_cpld):HW_access(0x30000,0x3FFFF,"DSC25")
{
    exit_on_not_match = false;
    
    this->memSpace=memSpace;
    
    HW_OSD * osd = new HW_OSD(mem);
    
    memSpace->set_OSD(osd);
    
    add_item(osd);
    
    add_item(new HW_uart(0x30300,0x30310,"UART0"));
    add_item(new HW_uart(0x30380,0x30390,"UART1"));
    add_item(new HW_clock());
    HW_gpio * gpio = new HW_gpio();
    add_item(gpio);
    hw_30a24 = new HW_30a24();
    hw_dma = new HW_dma(mem,hw_cpld);
    add_item(hw_30a24);    
    add_item(hw_dma);
    
    HW_irq = new HW_IRQ();
    
    add_item(HW_irq);
    
    for(int i=0;i<4;i++)
    {
        timer_list[i]=new HW_timer(i,HW_irq);
        add_item(timer_list[i]);
    }
    
    add_item(new HW_TI_ver());
    add_item(new HW_ECR());
    
    
    
    new i2c_master(gpio);
    
}

HW_TI::~HW_TI()
{
    
}

