/* 
*   kernel/usb_fw.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/cpld.h>
#include <kernel/hw_chk.h>
#include <kernel/usb_fw.h>
#include <kernel/kernel.h>
//#include <evt.h>

int kusb_state,kfw_state;
struct hw_chk_s usb_fw_chker;

int kusb_status=0;

void chk_usb_fw(void)
{
    int new_state;
    new_state=kusbIsConnected();
    if(new_state!=kusb_state)
    {
        kusb_state=new_state;
        printk("[USB FW] usb %s\n",kusb_state==1?"connected":"disconnected");
        //send_evt(EVT_USB);
    }
    
    new_state=kFWIsConnected();
    if(new_state!=kfw_state)
    {
        kfw_state=new_state;
        printk("[USB FW] FW %s\n",kfw_state==1?"connected":"disconnected");
        //send_evt(EVT_FW_EXT);
    }
}
/*
int kusbIsConnected(void)
{
   int val=inw(USB_STATE);
   if((val >> 0x6)&0x1)
        return 1;
   return 0;
}

int kFWIsConnected(void)
{
    int val=cpld_read(CPLD3);
    if(val&0x8)
        return 0;
    else
        return 1;
}

void FW_enable(void)
{
    cpld_select(CPLD_FW_EXT,0x1);
}

void FW_disable(void)
{
    cpld_select(CPLD_FW_EXT,0x0);
}
*/

void init_usb_fw(void)
{
    kusb_state=kusbIsConnected();
    kfw_state=kFWIsConnected();
    kusb_status=0;
    ini_hw_chker(&usb_fw_chker);
    usb_fw_chker.name="usb/fw";
    usb_fw_chker.action=chk_usb_fw;
    add_hw_chker(&usb_fw_chker);
    
    printk("[init] usb FW (usb %s - FW %s)\n",kusb_state==1?"connected":"disconnected",kfw_state==1?"connected":"disconnected");
}
