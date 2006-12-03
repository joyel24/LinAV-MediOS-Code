/*
*   kernel/target/arch_AV4XX/buttons.c
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
#include <kernel/gio.h>
#include <kernel/hardware.h>
#include <kernel/cpld.h>
#include <kernel/target/arch/buttons.h>

int btn_mask[NB_BUTTONS] =
{
BTMASK_UP ,      /*BTN_UP*/
BTMASK_DOWN,     /*BTN_DOWN*/
BTMASK_LEFT,     /*BTN_LEFT*/
BTMASK_RIGHT,    /*BTN_RIGHT*/
BTMASK_F1,       /*BTN_F1*/
BTMASK_F2,       /*BTN_F2*/
BTMASK_F3,       /*BTN_F3*/
0,               /*BTN_F4*/
0,               /*BTN_1*/
0,               /*BTN_2*/
0,               /*BTN_3*/
0,               /*BTN_4*/
BTMASK_ON,       /*BTN_ON*/
BTMASK_OFF,      /*BTN_OFF*/
0,               /*BT_FAST_DIR*/
0
};

int arch_btn_readHardware(void){
    int val;
    int dir,fn,on_off;
    int P1,P2;

    P1 =  (inw(BUTTON_PORT0));
    //for(i=0;i<16;++i); // delay
    P2 = P1>>8;
    P1 &= 0xFF;

    

    dir=fn=on_off=0;
        /*  U      |     D         |       L       |     R           */
    dir = (P1&0x1) | ((P1>>4)&0x2) | ((P1&0x2)<<1) | (((P1>>4)&0x1)<<3) ;
       /*  F1              |   F2             |    F3     */
    fn  = ((P2>>4)&0x2)>>1 | ((P2>>4)&0x1)<<1 | (P2&0x1)<<2 ;
          /*  ON                |   OFF */
    on_off = (((P2>>4)&0x4)>>2) | (P2&0x2) ;

    /* ON, OFF keys */

    val = (dir|(fn<<4)|(on_off<<12))&0xFFFF;

    return val;
}

