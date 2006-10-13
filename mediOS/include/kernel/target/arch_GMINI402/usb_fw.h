/*
*   include/target/arch_GMINI402/usb_fw.h
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

#define kusbIsConnected()                 (GIO_IS_SET(GIO_USB_CONNECTED))

#define USB_enable(){                                      \
                                                           \
    ata_reset();                                           \
                                                           \
    mdelay(100);                                           \
                                                           \
    cpld_write(CPLD0,1);                                   \
                                                           \
    mdelay(10);                                            \
                                                           \
    CPLD_SET_PORT2(CPLD_FX2_SELECT);                       \
    CPLD_CLEAR_PORT2(CPLD_FX2_WAKEUP);                     \
    CPLD_SET_PORT2(CPLD_FX2_WAKEUP);                       \
                                                           \
    mdelay(10);                                            \
                                                           \
    CPLD_SET_PORT2(CPLD_FX2_PA6);                          \
                                                           \
}

#define USB_disable(){                                     \
                                                           \
    CPLD_CLEAR_PORT2(CPLD_FX2_WAKEUP);                     \
                                                           \
    mdelay(10);                                            \
                                                           \
    CPLD_CLEAR_PORT2(CPLD_FX2_PA6);                        \
                                                           \
    mdelay(100);                                           \
                                                           \
    CPLD_CLEAR_PORT2(CPLD_FX2_SELECT);                     \
                                                           \
    cpld_write(CPLD0,3);                                   \
                                                           \
}

/* no firewire on Gmini402 */
#define kFWIsConnected()                  0
#define FW_enable()                       
#define FW_disable()                      

#endif
