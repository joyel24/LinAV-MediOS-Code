/* 
*   include/target/arch_GMINI4XX/usb_fw.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __USB_FW_H
#define __USB_FW_H



#include <kernel/target/arch/usb_fw.h>
#include <kernel/evt.h>
#include <kernel/kernel.h>

extern int kusb_state;
extern int kfw_state;

#define USB_FW_CHK  {                    \
    if(kusbIsConnected()!=kusb_state)    \
    {                                    \
        struct evt_t _evt;               \
        kusb_state=kusbIsConnected();    \
        printk("[USB FW] usb %s\n",kusb_state==1?"connected":"disconnected");  \
        _evt.evt=EVT_USB;                \
        _evt.evt_class = CONNECT_CLASS;  \
        _evt.data=kusb_state;            \
        evt_send(&_evt);                  \
    }                                    \
    if(kFWIsConnected()!=kfw_state)      \
    {                                    \
        struct evt_t _evt;               \
        kfw_state=kFWIsConnected();      \
        printk("[USB FW] FW %s\n",kfw_state==1?"connected":"disconnected"); \
        _evt.evt=EVT_FW_EXT;             \
        _evt.evt_class = CONNECT_CLASS;  \
        _evt.data=kfw_state;             \
        evt_send(&_evt);                  \
    }                                    \
}

void enableUsbFw(void);
void disableUsbFw(void);
int usb_isConnected(void);
int FW_isConnected(void);

void init_usb_fw(void);

#endif
