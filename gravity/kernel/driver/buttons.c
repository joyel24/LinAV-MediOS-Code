/* 
*   kernel/driver/buttons.c
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
#include <kernel/kernel.h>
#include <kernel/config.h>
#include <kernel/exit.h>

#include <kernel/gio.h>

#include <kernel/hw_chk.h>

#include <kernel/buttons.h>

#ifdef USE_DEBUG_ON_SCREEN
#include <kernel/kgraphics.h>
#endif

#define BTN_NOT_PRESSED(val,btn)    (val&(0x1<<btn))
#define BTN_PRESSED(val,btn)        !(val&(0x1<<btn))

__IRAM_DATA int mx_press = MAX_PRESSED;

__IRAM_DATA int nb_pressed[NB_BUTTONS];
__IRAM_DATA int nb_off_press=0;
__IRAM_DATA int nb_debug_switch=0;

extern int inHold;

__IRAM_DATA struct hw_chk_s btn_chker;

__IRAM_CODE void chk_button(void)
{
    int btn,val,fastDir=0;
    
    val =  inw(BUTTON_PORT0)&0x3;
    val|=((inw(BUTTON_PORT1)&0x7)<<2);
    val|=((inw(BUTTON_PORT2)&0x7)<<5);
    /* ON, OFF keys */
    if(gio_is_set(GIO_ON_BTN))  val |= (0x1<<8);
    if(gio_is_set(GIO_OFF_BTN)) val |= (0x1<<9);
    
#ifdef USE_DEBUG_ON_SCREEN  
    if(BTN_PRESSED(val,BUTTON_ON) && BTN_PRESSED(val,BUTTON_MENU1))
    {
        if(nb_debug_switch==0)
        {
            nb_debug_switch=mx_press*2;
#if 0            
            if(lcd_get_state()==0)
            {
                /* the lcd is off => turn on and discard the event */
                lcd_keyPress();
                return;
            }
            else
                lcd_launchTimer(); /* postpone the lcd timer */                
            halt_launchTimer(); /* postpone the poweroff timer */
#endif
            error_scr_switch();
        }
        else
            nb_debug_switch--;
        return;
    }
    else
        nb_debug_switch=0;
#endif

    for(btn=0;btn<NB_BUTTONS;btn++)
    {
        if(BTN_NOT_PRESSED(val,btn)) /* the btn is NOT pressed */
        {
            if(nb_pressed[btn]!=0)
                nb_pressed[btn]=0;   /* reset nb_pressed */
            if(btn==BUTTON_OFF)
                nb_off_press=0;    /* if off btn released -> reset nb_off_press */
            if(btn==BUTTON_ON)
                nb_debug_switch=0;          
        }
        else            /* the btn i is pressed */
        {    
            if(btn==BUTTON_OFF)    /* OFF btn pressed => check if we have to halt */
            {
                nb_off_press++;
                if(nb_off_press>MAX_OFF)
                {
                    printk("[OFF button] => halt\n");
                    halt_device();                    
                }
            }

               
            if(!(btn==BUTTON_JOYPRESS && fastDir)) /* discard BTN_JOY evt if fastDir is set */
            {   
#if 0              
                if(nb_pressed[btn]==0)
                {
                    if(!inHold)
                    {
                        nb_pressed[btn]=mx_press;
                        if(lcd_get_state()==0)
                        {
                            /* the lcd is off => turn on and discard the event */
                            lcd_keyPress();
                            break;
                        }
                        else
                            lcd_launchTimer(); /* postpone the lcd timer */
                            
                        halt_launchTimer(); /* postpone the poweroff timer */

                        process_btn_evt(btn);                       
                        //printk("BTN %d pressed\n",btn);
                    }
                    else
                    {
                        FM_putTmpText("** HOLD **",30);
                    }                                
                }
                else
                    nb_pressed[btn]--;
#endif                    
                /* a key is pressed if key num < 4 => it's a dir key we might be in fast dir mode
                    we need to discard BTN_JOY events   */                        
                if(btn<4)
                    fastDir=1;
            }
                
        }
    }
}

void init_buttons(void)
{
    int btn;
    for(btn=0;btn<NB_BUTTONS;btn++)
        nb_pressed[btn]=0;
    /* set GIO for ON/OFF to input */
    gio_dir(GIO_ON_BTN,GIO_IN);
    gio_dir(GIO_OFF_BTN,GIO_IN);
            
    btn_chker.action=chk_button;
    add_hw_chker(&btn_chker);
            
    printk("[init] buttons\n");
}
