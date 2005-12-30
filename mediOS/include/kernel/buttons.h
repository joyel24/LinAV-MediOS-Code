/*
*   include/buttons.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __BUTTONS_H
#define __BUTTONS_H

#include <kernel/hardware.h>
#include <kernel/kernel.h>
#include <kernel/gio.h>

#define MAX_PRESSED                 4
#define MAX_OFF                     300

#include <kernel/target/arch/buttons.h>
extern int old_state;
void process_button_press(int val);
void init_buttons(void);

int read_btn(void);
int arch_read_btn(void);

#define BTN_NOT_PRESSED(val,btn)    !(val&(0x1<<btn))
#define BTN_PRESSED(val,btn)        (val&(0x1<<btn))

#define BTN_CHK    {                     \
    int __val ;                          \
    __val=read_btn();                    \
    if(__val!=0x0)                       \
        process_button_press(__val);     \
}

#endif
