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

#ifndef __ARCH_USB_FW_H
#define __ARCH_USB_FW_H

#include <kernel/io.h>
#include <kernel/cpld.h>
#include <kernel/gio.h>

/** USB state **/

#define USB_STATE                         0x30a24
#define USB_enable()                      
#define USB_disable()                     
#define kusbIsConnected()                 (GIO_IS_SET(GIO_USB_CONNECTED))
#define kFWIsConnected()                  0
#define FW_enable()                       
#define FW_disable()                      

#endif
