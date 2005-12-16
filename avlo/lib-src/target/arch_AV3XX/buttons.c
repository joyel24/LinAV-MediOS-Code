/*
*   lib/target/arch_AV3XX/buttons.c
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stddef.h>

#include <io.h>
#include <gio.h>
#include <hardware.h>

int read_btn(void){
    int val;

    val =  inw(BUTTON_PORT0)&0x3;
    val|=((inw(BUTTON_PORT1)&0x7)<<2);
    val|=((inw(BUTTON_PORT2)&0x7)<<5);
    /* ON, OFF keys */
    if(gio_is_set(GIO_ON_BTN))  val |= (0x1<<8);
    if(gio_is_set(GIO_OFF_BTN)) val |= (0x1<<9);
    val = (~val)&0x3FF;

    return val;
}

