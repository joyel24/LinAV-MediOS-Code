/* 
*   gio_MAS_EOD.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either exEODess of implied.
*/

#ifndef __GIO_MAS_EOD_H
#define __GIO_MAS_EOD_H

#include "emu.h"
#include "HW_gpio.h"

class gio_MAS_EOD:public gpio_port {
    public:
        gio_MAS_EOD(void);
        
        void set_gpio(void);
        void clear_gpio(void);
        void gpio_dir_chg(int dir);
        
        int do_cmd_eod(void);
        
        int cnt,cnt_loop;;
        
        void chkEOD(void);

};



#endif // __GIO_MAS_EOD_H
