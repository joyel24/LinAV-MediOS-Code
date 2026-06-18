/*
*   kernel/target/arch_AV4XX/exit.c
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
#include <sys_def/font.h>
#include <sys_def/colordef.h>
#include <sys_def/string.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/irq.h>
#include <kernel/graphics.h>
#include <kernel/lcd.h>
#include <kernel/osd.h>
#include <kernel/clkc.h>
#include <kernel/exit.h>


__attribute__((section(".fwuncomp_code"))) void arch_reload_firmware(void){
    void (*DM_restart)(void) = (void (*)(void)) 0x0;
//    unsigned long i;
    // disable interrupts
    __cli();
    __clf();

    /*for(i=0;i<0x8000;i++) {
       *((unsigned char*)i) = *((unsigned char*)(0x18C0000 + i));
    } */
    gfx_closeGraphics();

    outb(0x8,OMAP_REQUEST_BASE);
    while(inb(OMAP_REQUEST_BASE));
    //for(
   //  while(1);
   // gfx_planeSetBufferOffset (BMAP1,0x900000);
   // gfx_planeSetSize(BMAP1,320,240,8);
   // gfx_planeSetPos(BMAP1,0x9B,0x2A);

    DM_restart();

   // outb(OMAP_RESTART_REQUEST,OMAP_REQUEST_BASE);
   // while(inb(OMAP_REQUEST_BASE));
}

void arch_HaltMsg(void)
{

}
