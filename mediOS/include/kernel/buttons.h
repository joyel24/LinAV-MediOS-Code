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

#define DEFAULT_INIT_DELAY      20
#define DEFAULT_SECOND_DELAY    10
#define DEFAULT_MIN_DELAY       2
#define DEFAULT_DEC_VALUE       1

#define MAX_OFF                     300

#define NB_BUTTONS                  0x10

#include <kernel/target/arch/buttons.h>
extern int old_state;
void btn_processPress(int val);
void btn_init(void);

int btn_readState(void);
int arch_btn_readState(void);

extern int nb_off_press;
extern int nb_pressed[NB_BUTTONS];
extern int press_step[NB_BUTTONS];
extern int mx_press[NB_BUTTONS];

#define BTN_CHK    {                     \
    int __val,__val2;                    \
    __val=btn_readState();               \
    if(__val&BTMASK_OFF)                 \
    {                                    \
        nb_off_press++;                  \
        if(nb_off_press>MAX_OFF)         \
        {                                \
            printk("[OFF button] => halt\n"); \
            halt_device();               \
        }                                \
    }                                    \
    else                                 \
        nb_off_press = 0;                \
    if(__val!=0x0)                       \
    {                                    \
        INTERNAL_TMR_CHK(__val2);        \
        if(__val2)                       \
            btn_processPress(__val);     \
    }                                    \
    else                                 \
    {                                    \
        memset(nb_pressed,0x0,sizeof(int)*NB_BUTTONS);\
        memset(mx_press,0x0,sizeof(int)*NB_BUTTONS);\
        memset(press_step,0x0,sizeof(int)*NB_BUTTONS);\
    }                                     \
}

struct btn_repeatParam {
    int init_delay;
    int second_delay;
    int min_delay;
    int dec_value;
};

#define BTMASK_UP            0x0001
#define BTMASK_DOWN          0x0002
#define BTMASK_LEFT          0x0004
#define BTMASK_RIGHT         0x0008

#define BTMASK_F1            0x0010
#define BTMASK_F2            0x0020
#define BTMASK_F3            0x0040
#define BTMASK_F4            0x0080

#define BTMASK_BTN1          0x0100
#define BTMASK_BTN2          0x0200
#define BTMASK_BTN3          0x0400
#define BTMASK_BTN4          0x0800

#define BTMASK_ON            0x1000
#define BTMASK_OFF           0x2000
#define BTMASK_FAST_DIR      0x4000

#endif
