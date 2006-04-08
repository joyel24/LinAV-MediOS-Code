/*
*   kernel/driver/buttons.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/kernel.h>
#include <kernel/exit.h>

#include <kernel/cpld.h>
#include <kernel/gio.h>
#include <kernel/evt.h>

//#include <kernel/hw_chk.h>
#include <kernel/bat_power.h>

#include <kernel/buttons.h>

#include <kernel/console.h>

__IRAM_DATA int btn_state;
__IRAM_DATA int mx_press[NB_BUTTONS];
__IRAM_DATA struct btn_repeatParam default_repeatParam = {
    DEFAULT_INIT_DELAY,
    DEFAULT_SECOND_DELAY,
    DEFAULT_MIN_DELAY,
    DEFAULT_DEC_VALUE
};
__IRAM_DATA struct btn_repeatParam * current_repeatParam;
__IRAM_DATA int nb_pressed[NB_BUTTONS];
__IRAM_DATA int press_step[NB_BUTTONS];
__IRAM_DATA int nb_off_press;
__IRAM_DATA int nb_debug_switch;



#ifdef HAVE_FM_REMOTE
extern int inHold;
#endif

extern int btn_mask[NB_BUTTONS];

#define BTN_NOT_PRESSED(val,btn)    !(val&btn_mask[btn])
#define BTN_PRESSED(val,btn)        (val&btn_mask[btn])

int need_clean;

__IRAM_CODE int btn_readState(void)
{
    return btn_state;
}

__IRAM_CODE void btn_processPress(int val)
{
    int btn;
    struct evt_t evt;

    for(btn=0;btn<NB_BUTTONS;btn++)
    {
        if(BTN_NOT_PRESSED(val,btn)) /* the btn is NOT pressed */
        {
            nb_pressed[btn]=0;   /* reset nb_pressed */
            mx_press[btn] = current_repeatParam->init_delay;
            press_step[btn] = 0;
            /*if(btn==BTN_OFF)
                nb_off_press=0;   */ /* if off btn released -> reset nb_off_press */
        }
        else            /* the btn i is pressed */
        {    
            
            /*if(btn==BTN_OFF)  */  /* OFF btn pressed => check if we have to halt */
            /*{
                nb_off_press++;
                if(nb_off_press>MAX_OFF)
                {
                    printk("[OFF button] => halt\n");
                    halt_device();                    
                }
            }*/

            if(nb_pressed[btn]==0)
            {
#ifdef HAVE_FM_REMOTE
                if(!inHold)
                {
#endif
                    switch(press_step[btn])
                    {
                        case 0:
                            press_step[btn] = 1;
                            nb_pressed[btn] = current_repeatParam->init_delay;
                            break;
                        case 1:
                            press_step[btn] = 2;
                            nb_pressed[btn] = current_repeatParam->second_delay;
                            mx_press[btn] = current_repeatParam->second_delay;
                            break;
                        case 2:
                            mx_press[btn] -= current_repeatParam->dec_value;
                            if(mx_press[btn]<current_repeatParam->min_delay)
                                mx_press[btn]=current_repeatParam->min_delay;
                            nb_pressed[btn] = mx_press[btn];
                            break;
                        default:
                            press_step[btn] = 0;
                            mx_press[btn] = current_repeatParam->init_delay;
                    }
#if 0
                    if(lcd_get_state()==0)
                    {
                        /* the lcd is off => turn on and discard the event */
                        lcd_keyPress();
                        break;
                    }
                    else
                        lcd_launchTimer(); /* postpone the lcd timer */
                    halt_launchTimer(); /* postpone the poweroff timer */
#endif

                    if(!con_screenIsVisible())
                    {
                      if (val&BTMASK_F1 && btn+1==BTN_ON) con_screenSwitch();
                      
                      // post the event
                      evt.evt=btn+1;
                      evt.evt_class=BTN_CLASS;
                      evt.data=(void*)mx_press[btn];
                      evt_send(&evt);
                    }
                    else
                    {
                        switch(btn+1)
                        {
                            case BTN_ON:
                                con_screenSwitch();
                                break;
                            case BTN_OFF:
                                con_clear();
                                break;
                            case BTN_UP:
                                con_screenScroll(-1);
                                break;
                            case BTN_DOWN:
                                con_screenScroll(1);
                                break;
                        }
                    }


                    //printk("BTN %d pressed\n",btn);
#ifdef HAVE_FM_REMOTE
                }
                else
                {
                    //FM_putTmpText("** HOLD **",30);
                    printk("** HOLD **\n");
                }
#endif
            }
            else
                nb_pressed[btn]--;
        }
    }
}

void btn_init(void)
{
    int btn;

    current_repeatParam = & default_repeatParam;

    nb_off_press=0;
    nb_debug_switch=0;
    need_clean=0;
    btn_state=0;


    for(btn=0;btn<NB_BUTTONS;btn++)
    {
        nb_pressed[btn]=0;
        mx_press[btn] = current_repeatParam->init_delay;
        press_step[btn] = 0;
    }
    /* set GIO for ON/OFF to input */
    GIO_DIRECTION(GIO_ON_BTN,GIO_IN);
    GIO_DIRECTION(GIO_OFF_BTN,GIO_IN);
            
    printk("[init] buttons\n");
}
