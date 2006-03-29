/*
*   kernel/target/arch_GMINI4XX/buttons.c
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
BTMASK_BTN1,     /*BTN_1*/
BTMASK_BTN2,     /*BTN_2*/
0,               /*BTN_3*/
0,               /*BTN_4*/
BTMASK_ON,       /*BTN_ON*/
BTMASK_OFF,      /*BTN_OFF*/
0,               /*BT_FAST_DIR*/
0
};

int arch_btn_readHardware(void){
    int val;
    int i,dir,fn,bt;

    if(cpld_getVersion()==5){

      GIO_DIRECTION(GIO_BTN_SELECT_UDLR,GIO_OUT);
      GIO_CLEAR(GIO_BTN_SELECT_UDLR);
      for(i=0;i<256;++i); // delay (keyboard hardware is too slow to read directly)
      dir = (inw(GIO_BITSET1) & 0x78) >> 3;
      GIO_DIRECTION(GIO_BTN_SELECT_UDLR,GIO_IN);

      GIO_DIRECTION(GIO_BTN_SELECT_FN,GIO_OUT);
      GIO_CLEAR(GIO_BTN_SELECT_FN);
      for(i=0;i<256;++i); // delay (keyboard hardware is too slow to read directly)
      fn = (inw(GIO_BITSET1) & 0x38) >> 3;
      GIO_DIRECTION(GIO_BTN_SELECT_FN,GIO_IN);

      GIO_DIRECTION(GIO_BTN_SELECT_SQCR,GIO_OUT);
      GIO_CLEAR(GIO_BTN_SELECT_SQCR);
      for(i=0;i<256;++i); // delay (keyboard hardware is too slow to read directly)
      bt = (inw(GIO_BITSET1) & 0x18) >> 3;
      GIO_DIRECTION(GIO_BTN_SELECT_SQCR,GIO_IN);

    }else{

      dir = inb(BUTTON_PORT0)&0xf;
      for(i=0;i<16;++i); // delay
      fn = inb(BUTTON_PORT1)&0x7;
      for(i=0;i<16;++i); // delay
      bt = inb(BUTTON_PORT2)&0x3;

    }

    val=(dir | bt << 8 | fn << 4);

    /* ON, OFF keys */
    if(GIO_IS_SET(GIO_ON_BTN))  val |= (0x1<<12);
    if(GIO_IS_SET(GIO_OFF_BTN)) val |= (0x1<<13);

    return (~val)&0x337f;
}

