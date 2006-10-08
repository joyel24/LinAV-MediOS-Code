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

typedef struct{
    unsigned char magic[4];
    int unpackedsize;
    int packedsize;
    int checksum;
} firmware_header;


void arch_reload_firmware(void)
{
    printk("Reload not supported atm => halt\n");
    halt_device();
}

void arch_HaltMsg(void)
{

}
