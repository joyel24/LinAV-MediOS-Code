/*
* asm/arch/av3xx_graphics.h
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __ASM_ARCH_AV3XX_KBD_TST_H
#define __ASM_ARCH_AV3XX_KBD_TST_H

#include <asm/arch/av3xx_common.h>

/*#define        BUTTONS_AV300_UP            0x0001
#define        BUTTONS_AV300_LEFT          0x0002
#define        BUTTONS_AV300_RIGHT         0x0010
#define        BUTTONS_AV300_DOWN          0x0020
#define        BUTTONS_AV300_MENU3         0x0040
#define        BUTTONS_AV300_MENU1         0x0100
#define        BUTTONS_AV300_MENU2         0x0200
#define        BUTTONS_AV300_JOYPRESS      0x0400
#define        BUTTONS_AV300_ON            0x1000
#define        BUTTONS_AV300_OFF           0x2000
#define        BUTTONS_AV300_ANY           0x3773*/

#define        BUTTONS_AV300_UP            0x0000
#define        BUTTONS_AV300_LEFT          0x0001
#define        BUTTONS_AV300_RIGHT         0x0002
#define        BUTTONS_AV300_DOWN          0x0003
#define        BUTTONS_AV300_MENU3         0x0004
#define        BUTTONS_AV300_MENU1         0x0005
#define        BUTTONS_AV300_MENU2         0x0006
#define        BUTTONS_AV300_JOYPRESS      0x0007
#define        BUTTONS_AV300_ON            0x0008
#define        BUTTONS_AV300_OFF           0x0009
#define        EVT_AV300_TIMER             0x000a
#define        EVT_AV300_WKUP              0x000b
#define        EVT_AV300_USB               0x000c
#define        EVT_AV300_PWR               0x000d
#define        EVT_AV300_CF_IN             0x000e
#define        EVT_AV300_CF_OUT            0x000f

#define        MAX_PRESSED                 3
#define        AV_FREQ                     HZ/50

#define        MAX_OFF                     15

#define        STEP_SIZE                   1

int  av3xx_chk_button(unsigned long ptr);
int  av3xx_button_state(void);
int  av3xx_button_init(void);
void av3xx_move_mouse(int but);
int  av3xx_button_get_mouse(struct av3xx_pos * pos);
int  av3xx_button_set_mouse(struct av3xx_pos * pos);
int  av3xx_button_set_mouse_param(struct mouseParam * ptrParam);
int  av3xx_button_get_mouse_param(struct mouseParam * ptrParam);
void av3xx_add_event(int evt);
int  av3xx_get_event(void);
void av3xx_clear_buffer(void);
int  av3xx_wait_event(void);
void av3xx_start_timer(void);
void av3xx_stop_timer(void);
int  av3xx_timer_state(void);
void av3xx_set_timer_freq(int val);
void av3xx_wakeup_evt(void);
void av3xx_do_pause(void);
void av3xx_release_app(void);

#endif
