/* 
*   mem_space.cpp
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

#include <mem_space.h>

#include <HW_mem.h>
#include <HW_cpld.h>
#include <HW_TI.h>

#include <cmd_line.h>
#include <bkpt_list.h>
#include <my_print.h>

//#define EXIT_ON_ADDR_ERROR

mem_space * mem_obj;

void init_mem_static_fct(mem_space * mem);

mem_space::mem_space(char * flash,char * sdram):HW_access(0x0,0xFFFFFFFF,"AVMEM")
{
    /* init bkpt_list */
    
    bkpt= new bkpt_list();  

    HW_mem * reset_vector=new HW_mem(NULL,RESET_VECTOR,RESET_VECTOR+0x4,"Reset"); /* one instruction reset vector*/
    add_item(reset_vector);
    reset_vector->write(RESET_VECTOR,RESET_INIT_VAL,4);
    
    add_item(new HW_mem(NULL,IRAM_START,IRAM_END,"IRAM"));
    add_item(new HW_mem(NULL,0x40000,0x50000,"DSP MEM"));    
    add_item(new HW_mem(flash,FLASH_START,FLASH_END,"FLASH"));
    HW_mem * sd = new HW_mem(sdram,SDRAM_START,SDRAM_END,"SDRAM");
    add_item(sd);
    
    hw_cpld = new HW_cpld();    
    hw_TI = new HW_TI(this,sd,hw_cpld);
    
    
    add_item(hw_cpld);
    add_item(hw_TI);
    
    hw_cpld->set30A24(hw_TI->hw_30a24);
    hw_cpld->setDMA(hw_TI->hw_dma);
    hw_cpld->setONOFF(hw_TI->gpio);
#ifdef EXIT_ON_ADDR_ERROR    
    exit_on_not_match = true;
#else
    exit_on_not_match = false;
    data_abt_on_not_match = true;
#endif    
    /* special init for each ARCH */
    ARCH_INIT
    
    
    init_mem_static_fct(this);
}

mem_space::~mem_space()
{
    HW_access * ptr=zone_list;
    HW_access * ptr2;
    
    while(ptr!=NULL)
    {
        ptr2=ptr->nxt;
        delete(ptr);
        ptr=ptr2;
    }
}

extern uint32_t old_PC;

uint32_t mem_space::read(uint32_t addr,int size)
{
    uint32_t val = HW_access::read(addr,size);
           
    if(bkpt->has_bkpt(addr,BKPT_MEM))
    {
        printf("@%08x: read mem (s=%d), get : %x\n",old_PC,size,val);
    }
    return val;
}

void mem_space::write(uint32_t addr,uint32_t val,int size)
{
    if(bkpt->has_bkpt(addr,BKPT_MEM))
    {
        printf("@%08x: write mem (s=%d): %x\n",old_PC,size,val);
    }
    HW_access::write(addr,val,size);
    
    hw_OSD->chk_access(addr,val);
}

void mem_space::set_OSD(HW_OSD * hw_osd)
{
    this->hw_OSD=hw_osd;
}

#include "mem_cmd_line_fct.h"
