/*
*   kernel/target/arch_AV3XX/exit.c
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

#include <kernel/kernel.h>
#include <kernel/io.h>
#include <kernel/ata.h>
#include <kernel/hardware.h>
#include <kernel/exit.h>
#include <kernel/graphics.h>
#include <kernel/lcd.h>

void reset_device(void);

void arch_reload_firmware(void)
{
    printk("about to reboot\n");
    ata_sofReset();
    ata_stopHD(ATA_FORCE_STOP); /* we need to call halt_hd later to unmount all partitions */
    reset_device();
}

void arch_HaltMsg(void)
{
    int h,w;
    char * msg = "Halting device";
    
    
    gfx_openGraphics();
    
    gfx_clearScreen(COLOR_WHITE);
    
    gfx_fontSet(STD8X13);
    
    gfx_getStringSize(msg,&w,&h);
    
    
    gfx_putS(COLOR_RED,COLOR_WHITE,(SCREEN_WIDTH-w)/2,(SCREEN_HEIGHT-h)/2,msg);
}
