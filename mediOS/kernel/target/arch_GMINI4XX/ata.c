/*
*   kernel/target/arch_GMINI4XX/ata.c
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
#include <kernel/hardware.h>
#include <kernel/cpld.h>
#include <kernel/ata.h>
#include <kernel/delay.h>

void arch_ata_reset_HD(void)
{
    cpld_clear_port_1(CPLD_HD_RESET);
    cpld_set_port_1(CPLD_HD_RESET);
}

void arch_ata_powerUp_HD(void)
{
    cpld_set_port_1(CPLD_HD_POWER); /* powering up HD */
}

void arch_ata_powerDown_HD(void)
{
    cpld_clear_port_1(CPLD_HD_POWER);
}

void arch_ata_select_HD(void)
{
}

void arch_ata_select_CF(void)
{
}

void arch_init_ata(void)
{
  arch_ata_powerUp_HD();
  arch_ata_reset_HD();
}

void arch_ide_intr_action(int irq)
{
}


