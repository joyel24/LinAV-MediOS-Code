/* 
*   kernel/driver/bat_power.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/i2c.h>
#include <kernel/tsc2003.h>
#include <kernel/exit.h>
#include <kernel/cpld.h>
#include <kernel/gio.h>
#include <kernel/kernel.h>
#include <kernel/timer.h>
#include <kernel/hw_chk.h>
#include <kernel/ata.h>
//#include <evt.h>

#include <kernel/bat_power.h>

int kpowerConnected(void)
{
    int val=inw(POWER_STATE);
    return (val >> 0x5)&0x1;
}

int kgetBatLevel(void)
{
    return tsc2003getVal(CMD_BAT0|INTERNAL_ON);
}

int lcd_state=1;
int lcd_bright=10;
int lcd_timer_used[2]={1,0};
int lcd_freq_rep[2]={LCD_FREQ_DEFAULT_0,LCD_FREQ_DEFAULT_1};
struct timer_s lcdOnOff_timer;

int halt_timer_used[2]={1,0};
int halt_freq_rep[2]={HALT_FREQ_DEFAULT_0,HALT_FREQ_DEFAULT_1};
struct timer_s halt_timer;

int hd_freq_rep[2]={HD_FREQ_DEFAULT_0,HD_FREQ_DEFAULT_1};
int hd_timer_used[2]={1,1};
int hd_sleep_state=0;
struct timer_s hd_timer;

struct hw_chk_s dc_chker;

void lcd_set_state(int state)
{
    if(state!=lcd_get_state())
    {
        lcd_state=state;
        if(state) /* turn on */
        {
            cpld_set_port_2(CPLD_LCD);
            gio_clear(GIO_LCD_BACKLIGHT);
            printk("Turning on lcd\n");
        }
        else
        {
            cpld_clear_port_2(CPLD_LCD);
            gio_set(GIO_LCD_BACKLIGHT);
            printk("Turning off lcd\n");
        }
   }
}

int lcd_get_state(void)
{
    return lcd_state;
}

void lcd_off(void)
{
    lcd_set_state(0);    
}

void lcd_on(void)
{
    lcd_set_state(1);    
}

void lcd_launchTimer(void)
{
    int num=getCurrentTimer();
    if(lcd_freq_rep[num]!=0 && lcd_timer_used[num] && lcd_state)
    {
        lcdOnOff_timer.expires = tick + (lcd_freq_rep[num]*HZ); /* lcd_freq_rep in sec */
        start_timer(&lcdOnOff_timer);
    }
}

void lcd_timer_action(void)
{
    int num=getCurrentTimer();    
    if(lcd_timer_used[num])
    {
        lcd_off();
/*        if(FM_is_connected())
            FM_lightsOFF();        */
    }
}

void halt_launchTimer(void)
{
    int num=getCurrentTimer();
    if(halt_freq_rep[num]!=0 && halt_timer_used[num])
    {
        halt_timer.expires = tick + (halt_freq_rep[num]*HZ); /* lcd_freq_rep in sec */
        start_timer(&halt_timer);
    }
}

void halt_timer_action(void)
{
    int num=getCurrentTimer();
    if(halt_timer_used[num])
    {
        printk("[POWER OFF timer] => halt\n");
        halt_device();
    }
}

void lcd_keyPress(void)
{
    int num=getCurrentTimer();
    if(lcd_timer_used[num])
    {
        lcd_on();
/*        if(FM_is_connected())
            FM_lightsON();*/
        lcd_launchTimer();        
    }
}

void hd_launchTimer(void)
{
    int num=getCurrentTimer();
    if(hd_freq_rep[num]!=0 && hd_timer_used[num] && !hd_sleep_state)
    {
        hd_timer.expires = tick + (hd_freq_rep[num]*HZ); /* hd_freq_rep in sec */
        start_timer(&hd_timer);
    }
}

extern int kusb_status;

void hd_timer_fct(void)
{
    int num=getCurrentTimer();
    if(hd_timer_used[num] && !kusb_status)
    {
        ata_stop_HD();
    }
    if(kusb_status)
        hd_launchTimer(); // we have enable the usb => keep the timer running
}

void hd_timer_on(int num)
{
    hd_timer_used[num]=1;
    if(num == getCurrentTimer())
        hd_launchTimer();
}

int hd_timer_state(int num)
{
    return hd_timer_used[num];
}

void hd_timer_off(int num)
{
    hd_timer_used[num]=0;
    stop_timer(&hd_timer);
}

void chgTimer(void)
{
    int num=getCurrentTimer();
    stop_timer(&halt_timer);
    stop_timer(&lcdOnOff_timer);
    stop_timer(&hd_timer);
    
    halt_launchTimer();
    lcd_launchTimer();
    hd_launchTimer();
    
    printk("DC changed => changing timers (LCD:%d,HALT:%d,HD:%d)\n",
            lcd_timer_used[num],
            halt_timer_used[num],
            hd_timer_used[num]);
}

int getCurrentTimer(void)
{
    if(kpowerConnected())
        return AV_TIMER_ON_DC;
    else
        return AV_TIMER_ON_BAT;
}

int kpwrState;

void chk_DC_connector(void)
{
    if(kpowerConnected()!=kpwrState)
    {
        kpwrState=kpowerConnected();
        /* change the timers */
        chgTimer();
           
//        send_evt(EVT_PWR);
        printk("DC connector %s\n",kpwrState==1?"plugged":"unplugged");

    }
}

void init_power(void)
{ 
    setup_timer(&lcdOnOff_timer,"lcdOnOff");
    lcdOnOff_timer.action = lcd_timer_action;
    setup_timer(&halt_timer,"halt");
    halt_timer.action = halt_timer_action;
    setup_timer(&hd_timer,"HD");
    hd_timer.action = hd_timer_fct;
    
    lcd_state=1;
    lcd_bright=10;
    lcd_timer_used[0]=1;
    lcd_timer_used[1]=0;
    lcd_freq_rep[0]=LCD_FREQ_DEFAULT_0;
    lcd_freq_rep[1]=LCD_FREQ_DEFAULT_1;

    halt_timer_used[0]=1;
    halt_timer_used[1]=0;
    halt_freq_rep[0]=HALT_FREQ_DEFAULT_0;
    halt_freq_rep[1]=HALT_FREQ_DEFAULT_1;

    hd_freq_rep[0]=HD_FREQ_DEFAULT_0;
    hd_freq_rep[1]=HD_FREQ_DEFAULT_1;
    hd_timer_used[0]=1;
    hd_timer_used[1]=1;
    hd_sleep_state=0;
    
    halt_launchTimer();
    lcd_launchTimer();
    hd_launchTimer();
    
    kpwrState=kpowerConnected();
    ini_hw_chker(&dc_chker);
    dc_chker.name="DC in";
    dc_chker.action=chk_DC_connector;
    add_hw_chker(&dc_chker);
      
    
    printk("[init] power : Bat level: %x, DC %s connected\n",kgetBatLevel(),kpwrState==0?"not":"is");
}
