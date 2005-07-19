/* 
*   HW_ON_OFF.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_ON_OFF_H
#define __HW_ON_OFF_H

#include "emu.h"
#include "HW_gpio.h"

#define ON_GPIO   0
#define OFF_GPIO  1

class HW_ON_OFF:public gpio_port {
    public:
        HW_ON_OFF(int num);
        //void set_gpio(void);
        //void clear_gpio(void);
        //void gpio_dir_chg(int dir);
        
        //const char * name;
        
        //int state;
        
    /*protected:
        int gpio_num;*/
};



#endif // __HW_ON_OFF_H
