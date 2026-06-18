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

#define kusbIsConnected()   ((inw(USB_STATE) & 0x40)!=0)
#define kFWIsConnected()    ((cpld_read(CPLD3) & 0x8)==0)
#define FW_enable()         cpld_select(CPLD_FW_EXT,0x1)
#define FW_disable()        cpld_select(CPLD_FW_EXT,0x0)

/*int kusbIsConnected(void);
int kFWIsConnected(void);
void FW_enable(void);
void FW_disable(void);*/
void init_usb_fw(void);

#endif
