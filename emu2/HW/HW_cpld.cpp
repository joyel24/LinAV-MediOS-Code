/* 
*   HW_cpld.cpp
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
#include <string.h>

#include <HW_cpld.h>

#ifdef HAS_HW_30A24
#include <HW_30a24.h>
#endif

#include <HW_ON_OFF.h>

#include <my_print.h>

#define CPLD_V_LOW
#define CPLD_V_HIGH

#if 1
#define BTN_FCT(BTN,MASK)      \
    if(BTN)                    \
        BTN--;                 \
    else                       \
        ret_val |= MASK;        
#else
#define BTN_FCT(BTN,MASK)      \
    if(!BTN)                    \
        ret_val |= MASK;        
#endif
HW_cpld * cpld_obj;

#include <cmd_line.h>        
#include "cpld_cmd_line_fct.h"        
        
HW_cpld::HW_cpld(void):HW_access(CPLD_START,CPLD_END,"CPLD")
{
    exit_on_not_match = false;
    
    init_cpld_static_fct(this);
    
    cpld_ata_mode = 0x0 ; /* only HD/CF is concidered => we use HD */
    cpld_module_type = 0xF; /* no modules */
    cpld2_val = 0;
    fw_connected = 1;
    hdd_on =0;
    bck_light = 0;
    ide_reset = 0;
    
   
    /* btn init */
    for(int k=0;k<0xb;k++)
        btn_var[k]=0   ;
        
    ON_btn = new HW_ON_OFF(ON_GPIO); 
    OFF_btn = new HW_ON_OFF(OFF_GPIO);
}

HW_cpld::~HW_cpld()
{
    
}

#ifdef HAS_HW_30A24
void HW_cpld::set30A24(HW_30a24 * hw_30a24)
{
    this->hw_30a24 = hw_30a24;
}
#endif

void HW_cpld::setONOFF(HW_gpio * gpio)
{
    gpio->register_port(ON_btn->gpio_num,ON_btn);
    gpio->register_port(OFF_btn->gpio_num,OFF_btn);
}

uint32_t HW_cpld::read(uint32_t addr,int size)
{
    uint32_t ret_val=0;
    
    switch(addr)
    {
        case CPLD_START+CPLD_PORT_OFFSET+0x0:
            ret_val=cpld_module_type; 
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD0 - read, size %x: module connected : %x\n",size,ret_val);
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x100:
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD1 - read, size %x: %x (USB %s)\n",size,cpld2_val,cpld2_val&0x1?"enable":"disable");
            ret_val = cpld2_val;
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x200:
            ret_val = 0x10;
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD2 - read, size %x: %x\n",size,ret_val);
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x300:
            ret_val = (fw_connected&0x1)<<3;
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD3 - read, size %x: %x\n",size,ret_val);
            break;
#ifdef AV3XX
        case CPLD_START+CPLD_PORT_OFFSET+0x680:
            BTN_FCT(btn_var[BTN_UP],0x1)
            BTN_FCT(btn_var[BTN_LEFT],0x2)
            ret_val |= 0x4;
            DEBUG_HW(CPLDBTN_HW_DEBUG,"CPLD - read buttons (@0x%08x), size %x => send %x\n",addr,size,ret_val);
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x700:
            BTN_FCT(btn_var[BTN_RIGHT],0x1)
            BTN_FCT(btn_var[BTN_DOWN],0x2)
            BTN_FCT(btn_var[BTN_F3],0x4)
            DEBUG_HW(CPLDBTN_HW_DEBUG,"CPLD - read buttons (@0x%08x), size %x => send %x\n",addr,size,ret_val);
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x780:
            BTN_FCT(btn_var[BTN_F1],0x1)
            BTN_FCT(btn_var[BTN_F2],0x2)
            BTN_FCT(btn_var[BTN_JOY],0x4)
            DEBUG_HW(CPLDBTN_HW_DEBUG,"CPLD - read buttons (@0x%08x), size %x => send %x\n",addr,size,ret_val);
            break;
#endif
#ifdef AV1XX
        case CPLD_START+CPLD_PORT_OFFSET+0x680:
            BTN_FCT(btn_var[BTN_LEFT] ,0x1)
            BTN_FCT(btn_var[BTN_OK]   ,0x2)
            BTN_FCT(btn_var[BTN_RIGHT],0x4)
            BTN_FCT(btn_var[BTN_DOWN] ,0x8)
            DEBUG_HW(CPLDBTN_HW_DEBUG,"CPLD - read buttons (@0x%08x), size %x => send %x\n",addr,size,ret_val);
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x700:
            BTN_FCT(btn_var[BTN_F1]   ,0x1)
            BTN_FCT(btn_var[BTN_F2]   ,0x2)
            BTN_FCT(btn_var[BTN_F3]   ,0x4)
            BTN_FCT(btn_var[BTN_UP]   ,0x8)
            DEBUG_HW(CPLDBTN_HW_DEBUG,"CPLD - read buttons (@0x%08x), size %x => send %x\n",addr,size,ret_val);
            break;
#endif
        default:
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD - read ERROR ukn addr: @0x%08x, size %x\n",addr,size);
            ret_val=0;
            break;
    }
    return ret_val;
}

void HW_cpld::write(uint32_t addr,uint32_t val,int size)
{
    switch(addr)
    {
        case CPLD_START+CPLD_PORT_OFFSET+0x0:
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD0 - write ata mode, size %x: %x\n",size,val);
            cpld_ata_mode = val;
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x100:
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD1 - write, size %x: %x (USB %s)\n",size,val,val&0x1?"enable":"disable");
            cpld2_val = val;
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x200:
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD2 - write, size %x: %x (bck light %s) (ide reset %s)\n",size,val,
                (val&0x4)?"ON":"OFF",(val&0x1)?"ON":"OFF");
            bck_light = val&0x4;
            ide_reset = val&0x1;
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x300:
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD3 - write, size %x: %x (HD is %s)\n",size,val,(val&0x8)?"ON":"OFF");
            hdd_on = val&0x8;
            hw_30a24->HDD_power = hdd_on?1:0;
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x680:
        case CPLD_START+CPLD_PORT_OFFSET+0x700:
        case CPLD_START+CPLD_PORT_OFFSET+0x780:
            DEBUG_HW(CPLDBTN_HW_DEBUG,"CPLD - !!!! write buttons (@0x%08x), size %x\n",addr,size);
            break;
        default:
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD - write ERROR ukn addr: @0x%08x, size %x\n",addr,size);
            break;
    }
}


void HW_cpld::setModule(int module)
{
    this->cpld_module_type=module;
}



