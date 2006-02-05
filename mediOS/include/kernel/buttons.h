/*
*   include/buttons.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __KBUTTONS_H
#define __KBUTTONS_H

#include <kernel/hardware.h>
#include <kernel/kernel.h>
#include <kernel/gio.h>
#include <buttons.h>

#define DEFAULT_INIT_DELAY      20
#define DEFAULT_SECOND_DELAY    10
#define DEFAULT_MIN_DELAY       2
#define DEFAULT_DEC_VALUE       1

#define MAX_OFF                     200

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
extern int need_clean;
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
        {                                \
            need_clean=1;                \
            btn_processPress(__val);     \
        }                                \
    }                                    \
    else if(need_clean)                  \
    {                                    \
        need_clean=0;                    \
        memset(nb_pressed,0x0,sizeof(int)*NB_BUTTONS);\
        memset(press_step,0x0,sizeof(int)*NB_BUTTONS);\
    }                                     \
}

struct btn_repeatParam {
    int init_delay;
    int second_delay;
    int min_delay;
    int dec_value;
};



#endif
