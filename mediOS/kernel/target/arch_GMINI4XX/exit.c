/*
*   kernel/target/arch_GMINI4XX/exit.c
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
#include <sys_def/string.h>

#include <kernel/io.h>
#include <kernel/hardware.h>

void (*firmware_start)(void) = (void (*)(void)) SDRAM_START;

void arch_reload_firmware(void){
  //clear firmware screen
  memset((char *)0x18d1500,0,240*176*2);

  //jump to sdram start (where the uncompressed firmware is)
  firmware_start();
}

void arch_HaltMsg(void)
{

}

