/* 
*   gio_vid_uart1RX.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <stdlib.h>
#include <stdio.h>

#include <gio_vid_uart1RX.h>

GIO_VID_UART1_RX::GIO_VID_UART1_RX(void):gpio_port(GPIO_VID_OUT_UART1_RX,"VID out / UART1 RX switch")
{
    state = 0;  /* default => btn not pressed */
}

void GIO_VID_UART1_RX::set_gpio(void)
{
    state = 1;
}

void GIO_VID_UART1_RX::clear_gpio(void)
{
    state = 0;
}

