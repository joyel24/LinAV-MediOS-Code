/* 
*   include/usb_fw.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/


#ifndef __USB_FW_H
#define __USB_FW_H

int kusbIsConnected(void);
int kFWIsConnected(void);
void FW_enable(void);
void FW_disable(void);
void init_usb_fw(void);

#endif
