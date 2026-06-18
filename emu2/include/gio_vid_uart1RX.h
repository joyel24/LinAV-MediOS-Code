/* 
*   gio_vid_uart1_rx.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GIO_VID_UART1_RX_H
#define __GIO_VID_UART1_RX_H

#include "emu.h"
#include "HW_gpio.h"



class GIO_VID_UART1_RX:public gpio_port {
    public:
        GIO_VID_UART1_RX(void);
        
        void set_gpio(void);
        void clear_gpio(void);
};



#endif 
