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
#define MAX_OFF                     15

#define BUTTON_UP            0x0000
#define BUTTON_LEFT          0x0001
#define BUTTON_RIGHT         0x0002
#define BUTTON_DOWN          0x0003
#define BUTTON_MENU3         0x0004
#define BUTTON_MENU1         0x0005
#define BUTTON_MENU2         0x0006
#define BUTTON_JOYPRESS      0x0007
#define BUTTON_ON            0x0008
#define BUTTON_OFF           0x0009

#define NB_BUTTONS            10

extern int old_state;
void process_button_press(int val);
void init_buttons(void);
//READ_BUTTONS ;
#define BTN_CHK    {                     \
    int __val ;                          \
    READ_BUTTONS(__val)                  \
    if(old_state==0);                    \
        old_state=__val;                 \
    if(__val!=old_state || __val!=0x3FF) \
        process_button_press(__val);     \
}

#endif
