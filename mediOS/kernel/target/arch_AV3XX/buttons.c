/*
*   kernel/target/arch_AV3XX/buttons.c
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
BTMASK_BTN1,     /*BTN_1*/
0,               /*BTN_2*/
0,               /*BTN_3*/
0,               /*BTN_4*/
BTMASK_ON,       /*BTN_ON*/
BTMASK_OFF,      /*BTN_OFF*/
0,               /*BT_FAST_DIR*/
0
};

int trad_tab[] = {
    0x0,0x1,0x4,0x5,
    0x8,0x9,0xC,0xD,
    0x2,0x3,0x6,0x7,
    0xA,0xB,0xE,0xF
};

int arch_btn_readHardware(void){
    int dir,fn,bt,on_off_fast=0;
    int val;

    dir = (~inw(BUTTON_PORT0))&0x3;
    fn  = (~inw(BUTTON_PORT1))&0x7;
    bt  = (~inw(BUTTON_PORT2))&0x7;
    
    dir = trad_tab[dir|(fn&0x3)<<2];    
    fn  = (fn&0x4)|(bt&0x3);
    bt  = (bt>>2)&0x1;

    if(bt && dir)
    {
       bt = 0;
       on_off_fast=0x4;
    }


    /* ON/OFF keys */
    if(!GIO_IS_SET(GIO_ON_BTN))  on_off_fast |= (0x1);
    if(!GIO_IS_SET(GIO_OFF_BTN)) on_off_fast |= (0x2);
    
    val = (dir|(fn<<4)|(bt<<8)|(on_off_fast<<12))&0xFFFF;
    
    return val;
}

