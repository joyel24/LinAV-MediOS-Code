/* 
*   kernel/driver/bat_power.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/



#include <kernel/exit.h>

#include <kernel/kernel.h>
#include <kernel/timer.h>
#include <kernel/ata.h>
#include <kernel/evt.h>
#include <kernel/disk.h>

#include <kernel/fm_remote.h>

#include <kernel/bat_power.h>
#include <kernel/target/arch/lcd.h>

int lcd_state=1;
int lcd_bright=10;
int lcd_timer_used[2]={1,0};
int lcd_freq_rep[2]={LCD_FREQ_DEFAULT_0,LCD_FREQ_DEFAULT_1};
struct tmr_s lcdOnOff_timer;

int halt_timer_used[2]={1,0};
int halt_freq_rep[2]={HALT_FREQ_DEFAULT_0,HALT_FREQ_DEFAULT_1};
struct tmr_s halt_timer;

int hd_freq_rep[2]={HD_FREQ_DEFAULT_0,HD_FREQ_DEFAULT_1};
int hd_timer_used[2]={1,1};
int hd_sleep_state=0;
struct tmr_s hd_timer;

void lcd_setBrightness(int val)
{
    lcd_bright = val;
}

int lcd_getBrightness(void)
{
    return lcd_bright;
}

void set_timer_status_freq(int timer_type, int power_mode, int val,int type)
{
    int * timer_status;
    int * timer_freq;
    switch(timer_type)
    {
        case LCD_TIMER:
            timer_status=lcd_timer_used;
            timer_freq=lcd_freq_rep;
            break;
        case HD_TIMER:
            timer_status=hd_timer_used;
            timer_freq=hd_freq_rep;
            break;
        case HALT_TIMER:
            timer_status=halt_timer_used;
            timer_freq=halt_freq_rep;
            break;
        default:
            return;
    }
    
    if(power_mode!=TIMER_MODE_BAT && power_mode!=TIMER_MODE_DC)
        return;
    
    if(type == 1)
    {
        if(val!=MODE_ENABLE &&val!=MODE_DISABLE)
            return;        
        timer_status[power_mode] = val;
    }
    else
    {
        timer_freq[power_mode] = val;
    }
}

void set_timer_status(int timer_type, int power_mode, int status)
{
    set_timer_status_freq(timer_type,power_mode,status,1);
}

void set_timer_delay(int timer_type, int power_mode, int delay)
{
    set_timer_status_freq(timer_type,power_mode,delay,0);
}

int get_timer_status_freq(int timer_type, int power_mode,int type)
{
    int * timer_status;
    int * timer_freq;
    switch(timer_type)
    {
        case LCD_TIMER:
            timer_status=lcd_timer_used;
            timer_freq=lcd_freq_rep;
            break;
        case HD_TIMER:
            timer_status=hd_timer_used;
            timer_freq=hd_freq_rep;
            break;
        case HALT_TIMER:
            timer_status=halt_timer_used;
            timer_freq=halt_freq_rep;
            break;
        default:
            return -1;
    }
    
    if(power_mode!=TIMER_MODE_BAT && power_mode!=TIMER_MODE_DC)
        return -1;
    
    if(type==1)
    {
        return timer_status[power_mode] ;
    }
    else
    {
        return timer_freq[power_mode] ;
    }    
}

int get_timer_status(int timer_type, int power_mode)
{
    return get_timer_status_freq(timer_type,power_mode,1);
}

int get_timer_delay(int timer_type, int power_mode)
{
    return get_timer_status_freq(timer_type,power_mode,0);
}

void lcd_set_state(int state)
{
    if(state!=lcd_get_state())
    {
        lcd_state=state;
        if(state) /* turn on */
        {
            lcd_ON();
            printk("Turning on lcd\n");
        }
        else
        {
            lcd_OFF();
            printk("Turning off lcd\n");
        }
   }
}

int lcd_get_state(void)
{
    return lcd_state;
}

void lcd_launchTimer(void)
{
    int num=getCurrentTimer();
    if(lcd_freq_rep[num]!=0 && lcd_timer_used[num] && lcd_state)
    {
        lcdOnOff_timer.expires = tick + (lcd_freq_rep[num]*HZ); /* lcd_freq_rep in sec */
        tmr_start(&lcdOnOff_timer);
    }
}

void lcd_timer_action(void)
{
    int num=getCurrentTimer();
    if(lcd_timer_used[num])
    {
        lcd_off();
#ifdef HAVE_FM_REMOTE
        if(FM_is_connected())
            FM_lightsOFF();
#endif
    }
}

void halt_launchTimer(void)
{
    int num=getCurrentTimer();
    if(halt_freq_rep[num]!=0 && halt_timer_used[num])
    {
        halt_timer.expires = tick + (halt_freq_rep[num]*HZ); /* lcd_freq_rep in sec */
        tmr_start(&halt_timer);
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
#ifdef HAVE_FM_REMOTE
        if(FM_is_connected())
            FM_lightsON();
#endif
        lcd_launchTimer();        
    }
}

void hd_launchTimer(void)
{
    int num=getCurrentTimer();
    if(hd_freq_rep[num]!=0 && hd_timer_used[num] && !hd_sleep_state)
    {
        hd_timer.expires = tick + (hd_freq_rep[num]*HZ); /* hd_freq_rep in sec */
        tmr_start(&hd_timer);
    }
}

extern int kusb_fw_status;

void hd_timer_fct(void)
{
    int num=getCurrentTimer();
    if(hd_timer_used[num] && !kusb_fw_status)
    {
        ata_stopHD(ATA_DELAY_STOP);
    }
  
    if(kusb_fw_status)
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
    tmr_stop(&hd_timer);
}

void chgTimer(void)
{
    int num=getCurrentTimer();
    tmr_stop(&halt_timer);
    tmr_stop(&lcdOnOff_timer);
    tmr_stop(&hd_timer);

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
    if(POWER_CONNECTED)
        return TIMER_MODE_DC;
    else
        return TIMER_MODE_BAT;
}

int kpwrState;

void process_DC_change(void)
{
    struct evt_t evt;
    kpwrState=POWER_CONNECTED;
    /* change the timers */
    chgTimer();
    evt.evt = EVT_PWR;
    evt.evt_class = CONNECT_CLASS;
    evt.data = (int)kpwrState;
    evt_send(&evt);
    printk("DC connector %s\n",kpwrState==1?"plugged":"unplugged");
}

int DC_isConnected(void)
{
    return POWER_CONNECTED;
}

int batLevel(void)
{
    return GET_BAT_LEVEL;
}

void init_power(void)
{
    tmr_setup(&lcdOnOff_timer,"lcdOnOff");
    lcdOnOff_timer.action = lcd_timer_action;
    tmr_setup(&halt_timer,"halt");
    halt_timer.action = halt_timer_action;
    tmr_setup(&hd_timer,"HD");
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

    kpwrState=POWER_CONNECTED;    
    printk("[init] power : Bat level: %x, DC %s connected\n",GET_BAT_LEVEL,kpwrState==0?"not":"is");
}
