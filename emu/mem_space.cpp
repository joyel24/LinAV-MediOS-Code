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

#include "mem_space.h"

#include "HW_mem.h"
#include "HW_cpld.h"
#include "HW_TI.h"


mem_space::mem_space(char * flash,char * sdram):HW_access(0x0,0xFFFFFFFF,"AVMEM")
{
    HW_mem * reset_vector=new HW_mem(NULL,0x0,0x3,"Reset");
    add_item(reset_vector);
    reset_vector->write(0x0,0xEA03FFFE,4);
    
    add_item(new HW_mem(NULL,0x4,0x7fff,"IRAM"));
    add_item(new HW_mem(flash,0x100000,0x17ffff,"FLASH"));
    add_item(new HW_mem(sdram,0x3000000,0x3FFFFFF,"SDRAM"));
    
    add_item(new HW_cpld());
    add_item(new HW_TI());
    
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
