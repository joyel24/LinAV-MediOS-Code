/* 
*   HW_30A24.cpp
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

#include <HW_30a24.h>

#include <cmd_line.h> 

#include <emu.h>

#ifdef HAS_HW_30A24

HW_30a24 * hw_30a24_obj;

int do_plug_pw_s(int argc,char ** argv) 
{ 
    hw_30a24_obj->power_connected=1;
    return 0;
}

int do_unplug_pw_s(int argc,char ** argv) 
{ 
    hw_30a24_obj->power_connected=0;
    return 0;
}

int do_plug_usb_s(int argc,char ** argv) 
{ 
    hw_30a24_obj->usb_connected=1;
    return 0;
}

int do_unplug_usb_s(int argc,char ** argv) 
{ 
    hw_30a24_obj->usb_connected=0;
    return 0;
}

HW_30a24::HW_30a24(void):HW_access(0x30a24,0x30a25,"@30A24")
{
    HDD_power = 0;
    power_connected = 0;
    usb_connected = 0;
    
    hw_30a24_obj = this;
    
    add_cmd_fct("plug_usb",do_plug_usb_s,"Emulate usb plugged");
    add_cmd_fct("unplug_usb",do_unplug_usb_s,"Emulate usb unplugged");
    add_cmd_fct("plug_power",do_plug_pw_s,"Emulate power plugged");
    add_cmd_fct("unplug_power",do_unplug_pw_s,"Emulate power unplugged");
}

uint32_t HW_30a24::read(uint32_t addr,int size)
{
    int ret_val = ((HDD_power & 0x1) << 1) | ((power_connected & 0x1) << 5 ) | ((usb_connected & 0x1) << 6);
    DEBUG_HW(HW_30A24_DEBUG,"read 30A24\n");
    DEBUG_HW(HW_30A24_DEBUG,"%s read, size %x, send : %x: %s %s %s\n",name,size,ret_val,
        HDD_power?"HDD powered ON":"HDD powered OFF",power_connected?"power connected":"power NOT connected",
        usb_connected?"usb connected":"usb NOT connected"
    );
    return ret_val;
}

void HW_30a24::write(uint32_t addr,uint32_t val,int size)
{
    DEBUG_HW(HW_30A24_DEBUG,"write 30A24\n");
    DEBUG_HW(HW_30A24_DEBUG,"%s write, size %x, val= %x\n",name,size,val);
}

#endif
