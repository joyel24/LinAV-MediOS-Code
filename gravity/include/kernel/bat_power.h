/* 
*   include/bat_power.h
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __BAT_POWER_H
#define __BAT_POWER_H

#include <kernel/hardware.h>
#include <kernel/io.h>
#include <kernel/tsc2003.h>

#define AV_TIMER_ON_BAT        0
#define AV_TIMER_ON_DC         1

#define LCD_FREQ_DEFAULT_0  20
#define LCD_FREQ_DEFAULT_1  20

#define HALT_FREQ_DEFAULT_0 180
#define HALT_FREQ_DEFAULT_1 180

#define HD_FREQ_DEFAULT_0    10 /* 10s timer */
#define HD_FREQ_DEFAULT_1    10 /* 10s timer */

/*
int kpowerConnected(void);
int kgetBatLevel(void);
*/
#define kpowerConnected()   (inw(POWER_STATE) & 0x20)
#define kgetBatLevel()      tsc2003getVal(CMD_BAT0|INTERNAL_ON)

#define lcd_off()           lcd_set_state(0)
#define lcd_on()            lcd_set_state(1)

void lcd_set_state(int state);
int  lcd_get_state(void);
/*
void lcd_off(void);
void lcd_on(void);
*/
void lcd_launchTimer(void);
void lcd_timer_action(void);

void halt_launchTimer(void);
void halt_timer_action(void);

void lcd_keyPress(void);

void hd_launchTimer(void);
void hd_timer_fct(void);
void hd_timer_on(int num);
int  hd_timer_state(int num);
void hd_timer_off(int num);

void chgTimer(void);
int  getCurrentTimer(void);
void chk_DC_connector(void);

void init_power(void);

#endif
