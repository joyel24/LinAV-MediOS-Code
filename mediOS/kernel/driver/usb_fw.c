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
#include <kernel/timer.h>
#include <kernel/ata.h>

#include <sys_def/stddef.h>

int kusb_state,kfw_state;

int kusb_fw_status=0;
int k_usb_fw=0; /* variable to know what is used for connection 1: usb, 2:fw*/

struct tmr_s usbFw_tmr;

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
                printk("[USB/FW] USB enable\n");
            }
            else if(kFWIsConnected())
            {
                FW_enable();
                k_usb_fw=2;
                printk("[USB/FW] FW_EXT enable\n");
            }
        }
        else
        {
            if(k_usb_fw==1)
            {
               USB_disable();
                printk("[USB/FW] USB disable\n");
            }
            else if(k_usb_fw==2)
            {
                FW_disable();
                printk("[USB/FW] FW_EXT disable\n");
            }
            k_usb_fw=0;                
        }
        kusb_fw_status=state;
    }
    else
    {
        if(state)
            printk("[USB/FW] warning: enabling USB while it is already enabled\n");
        else
            printk("[USB/FW] warning: disabling USB while it is already disabled\n");
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

void usbFw_cableChk(void)
{
    if(kusbIsConnected()!=kusb_state)
    {
        struct evt_t _evt;
        kusb_state=kusbIsConnected();
        printk("[USB/FW] USB %s\n",kusb_state==1?"connected":"disconnected");
        _evt.evt=(kusb_state)?EVT_USB_IN:EVT_USB_OUT;
        _evt.evt_class = CONNECT_CLASS;
        _evt.data=NULL;
        evt_send(&_evt);
    }
    if(kFWIsConnected()!=kfw_state)
    {
        struct evt_t _evt;
        kfw_state=kFWIsConnected();
        printk("[USB/FW] FW %s\n",kfw_state==1?"connected":"disconnected");
        _evt.evt=(kfw_state)?EVT_FW_EXT_IN:EVT_FW_EXT_OUT;
        _evt.evt_class = CONNECT_CLASS;
        _evt.data=NULL;
        evt_send(&_evt);
    }
}

void init_usb_fw(void)
{
    kusb_state=kusbIsConnected();
    kfw_state=kFWIsConnected();
    kusb_fw_status=0;

    tmr_setup(&usbFw_tmr,"USB/FW cable chk");
    usbFw_tmr.action = usbFw_cableChk;
    usbFw_tmr.freeRun = 1;
    usbFw_tmr.stdDelay=1*HZ; /* 1s period */
    tmr_start(&usbFw_tmr);

    printk("[init] USB/FW (USB %s - FW %s)\n",
        kusb_state==1?"connected":"disconnected",kfw_state==1?"connected":"disconnected");
}
