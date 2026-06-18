/* 
*   gio_MAS_data.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GIO_MAS_DATA_H
#define __GIO_MAS_DATA_H

#include "emu.h"
#include "HW_gpio.h"


class i2c_MAS;

class gio_MAS_data:public gpio_port {
    public:
        gio_MAS_data(int num,i2c_MAS * mas);
        
        void set_gpio(void);
        void clear_gpio(void);
    private:
        i2c_MAS * mas;
};



#endif // __GIO_MAS_DATA_H
