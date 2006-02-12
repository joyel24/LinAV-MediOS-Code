/* 
*   include/kernel/bat_power.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __BAT_POWER_H
#define __BAT_POWER_H

#include <kernel/target/arch/bat_power.h>

#include <int_timer.h>

#define LCD_FREQ_DEFAULT_0  20
#define LCD_FREQ_DEFAULT_1  20

#define HALT_FREQ_DEFAULT_0 180
#define HALT_FREQ_DEFAULT_1 180

#define HD_FREQ_DEFAULT_0    10 /* 10s timer */
#define HD_FREQ_DEFAULT_1    10 /* 10s timer */

void set_timer_status(int timer_type, int power_mode, int status);
void set_timer_delay(int timer_type, int power_mode, int delay);
int  get_timer_status(int timer_type, int power_mode);
int  get_timer_delay(int timer_type, int power_mode);

void set_timer_status_freq(int timer_type, int power_mode, int val,int type);
int get_timer_status_freq(int timer_type, int power_mode,int type);

int DC_isConnected(void);
int batLevel(void);

void lcd_setBrightness(int val);
int  lcd_getBrightness(void);

extern int kpwrState;
void process_DC_change(void);

#define BAT_POWER_CHK {              \
    if(POWER_CONNECTED!=kpwrState) \
        process_DC_change();         \
}

#define INTERNAL_TMR_CHK(VAR) {  \
    if(lcd_get_state()==0)       \
    {                            \
        lcd_keyPress();          \
        VAR=0;                   \
    }                            \
    else                         \
    {                            \
        lcd_launchTimer();       \
        VAR = 1;                 \
    }                            \
    halt_launchTimer();          \
}

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
