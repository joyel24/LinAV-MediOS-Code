/* 
*   include/usb_fw.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/


#ifndef __USB_FW_H
#define __USB_FW_H

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/cpld.h>
#include <kernel/evt.h>
#include <kernel/kernel.h>

extern int kusb_state;
extern int kfw_state;

#define USB_FW_CHK  {                    \
    if(kusbIsConnected()!=kusb_state)    \
    {                                    \
        kusb_state=kusbIsConnected();    \
        printk("[USB FW] usb %s\n",kusb_state==1?"connected":"disconnected");  \
        send_evt(EVT_USB);               \
    }                                    \
    if(kFWIsConnected()!=kfw_state)      \
    {                                    \
        kfw_state=kFWIsConnected();      \
        printk("[USB FW] FW %s\n",kfw_state==1?"connected":"disconnected"); \
        send_evt(EVT_FW_EXT);            \
    }                                    \
}

void init_usb_fw(void);

#endif
