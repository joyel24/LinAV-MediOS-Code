/*
*   include/buttons.h
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __BUTTONS_H
#define __BUTTONS_H

#include <hardware.h>

#include <target/arch/buttons.h>

int read_btn(void);

#define BTN_NOT_PRESSED(val,btn)    !(val&(0x1<<btn))
#define BTN_PRESSED(val,btn)        (val&(0x1<<btn))

#define BTN_CHK    {                     \
    int __val ;                          \
    __val=read_btn();                    \
    if(old_state==0);                    \
        old_state=__val;                 \
    if(__val!=old_state || __val!=0x3FF) \
        process_button_press(__val);     \
}

#endif
