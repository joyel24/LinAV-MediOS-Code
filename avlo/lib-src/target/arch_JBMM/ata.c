/*
*   lib/target/arch_AV1XX/ata.c
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
#include <gio.h>
#include <ata.h>

void arch_ata_reset_HD(void)
{
    printf("HD reset\n");
    cpld_set_port_2(CPLD_HD_RESET);
    cpld_clear_port_2(CPLD_HD_RESET);

}

void arch_ata_powerUp_HD(void)
{
    gio_set(GIO_HD_POWER);
}

void arch_ata_powerDown_HD(void)
{
   gio_clear(GIO_HD_POWER);
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
    gio_dir(GIO_HD_POWER,GIO_OUT);
    
    
    gio_set(GIO_HD_POWER);

    outw(0x13a1,0x30a04);
    outw(0x0,0x30a08);
    outw(0x1,0x30a18);
    
    arch_ata_select_HD();
    
    arch_ata_reset_HD();
    
}

void arch_ide_intr_action(int irq)
{

}


