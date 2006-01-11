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

int arch_read_btn(void){
    int val;
    int i,dir,menu,bt;

    if(cpld_get_version()==5){

      gio_dir(GIO_BTN_SELECT_UDLR,GIO_OUT);
      gio_clear(GIO_BTN_SELECT_UDLR);
      for(i=0;i<256;++i); // delay (keyboard hardware is too slow to read directly)
      dir = (inw(GIO_BITSET1) & 0x78) >> 3;
      gio_dir(GIO_BTN_SELECT_UDLR,GIO_IN);

      gio_dir(GIO_BTN_SELECT_MENU,GIO_OUT);
      gio_clear(GIO_BTN_SELECT_MENU);
      for(i=0;i<256;++i); // delay (keyboard hardware is too slow to read directly)
      menu = (inw(GIO_BITSET1) & 0x38) >> 3;
      gio_dir(GIO_BTN_SELECT_MENU,GIO_IN);

      gio_dir(GIO_BTN_SELECT_SQCR,GIO_OUT);
      gio_clear(GIO_BTN_SELECT_SQCR);
      for(i=0;i<256;++i); // delay (keyboard hardware is too slow to read directly)
      bt = (inw(GIO_BITSET1) & 0x18) >> 3;
      gio_dir(GIO_BTN_SELECT_SQCR,GIO_IN);

    }else{

      dir = inb(BUTTON_PORT0)&0xf;
      for(i=0;i<16;++i); // delay
      menu = inb(BUTTON_PORT1)&0x7;
      for(i=0;i<16;++i); // delay
      bt = inb(BUTTON_PORT2)&0x3;

    }

    val=(dir | menu << 4 | bt << 7);

    /* ON, OFF keys */
    if(gio_is_set(GIO_ON_BTN))  val |= (0x1<<9);
    if(gio_is_set(GIO_OFF_BTN)) val |= (0x1<<10);

    return (~val)&0x7ff;
}

