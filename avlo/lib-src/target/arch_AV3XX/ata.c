/*
*   lib/target/arch_AV3XX/ata.c
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
#include <kernel.h>

#include <io.h>
#include <hardware.h>
#include <cpld.h>
#include <ata.h>

void arch_ata_reset_HD(void)
{
    printf("HD reset\n");
    cpld_set_port_2(CPLD_HD_RESET);
    cpld_clear_port_2(CPLD_HD_RESET);
}

void arch_ata_powerUp_HD(void)
{
    cpld_set_port_3(CPLD_HD_POWER); /* powering up HD */

}

void arch_ata_powerDown_HD(void)
{
    cpld_clear_port_3(CPLD_HD_POWER);
}

void arch_ata_select_HD(void)
{
    cpld_select(CPLD_HD_CF,CPLD_SEL_HD);
}

void arch_ata_select_CF(void)
{
    cpld_select(CPLD_HD_CF,CPLD_SEL_CF);
}

void arch_init_ata(void)
{
//    outb(0x8,IDE_CONTROL);          /* enabling ints */
//    enable_irq(IRQ_IDE);
    arch_ata_reset_HD();
}

void arch_ide_intr_action(int irq)
{
//    printk("get IDE irq: %d\n",irq);
}


