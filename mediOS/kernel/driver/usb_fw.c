/* 
*   kernel/driver/usb_fw.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/cpld.h>
#include <kernel/usb_fw.h>
#include <kernel/kernel.h>
#include <kernel/evt.h>

int kusb_state,kfw_state;

int kusb_fw_status=0;
int k_usb_fw=0; /* variable to know what is used for connection 1: usb, 2:fw*/

int usbFwEnabled(void)
{
    return kusb_fw_status;
}

void setUsbFw(int state)
{
    if(kusb_fw_status!=state)
    {
        if(state==1)
        {
            if(kusbIsConnected())
            {
                USB_enable();
                k_usb_fw=1;
                printk("[usb-FW_EXT] usb enable\n");
            }
            else if(kFWIsConnected())
            {
                FW_enable();
                k_usb_fw=2;
                printk("[usb-FW_EXT] FW_EXT enable\n");
            }
        }
        else
        {
            if(k_usb_fw==1)
            {
               USB_disable();
                printk("[usb-FW_EXT] usb disable\n");
            }
            else if(k_usb_fw==2)
            {
                FW_disable();
                printk("[usb-FW_EXT] FW_EXT disable\n");
            }
            k_usb_fw=0;                
        }
        kusb_fw_status=state;
    }
    else
    {
        if(state)
            printk("[usb state] warning: enabling usb while it is already enabled\n");
        else
            printk("[usb state] warning: disabling usb while it is already disabled\n");
    }
}


void enableUsbFw(void)
{
    setUsbFw(1);
}

void disableUsbFw(void)
{
    setUsbFw(0);
}

int usb_isConnected(void)
{
    return kusbIsConnected();
}

int FW_isConnected(void)
{
    return kFWIsConnected();
}


void init_usb_fw(void)
{
    kusb_state=kusbIsConnected();
    kfw_state=kFWIsConnected();
    kusb_fw_status=0;
        
    printk("[init] usb FW (usb %s - FW %s)\n",kusb_state==1?"connected":"disconnected",kfw_state==1?"connected":"disconnected");
}
