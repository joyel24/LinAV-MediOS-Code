/*
*   kernel/exit.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/kernel.h>
#include <kernel/irq.h>
#include <kernel/ata.h>
#include <kernel/delay.h>

#include <kernel/exit.h>

void halt_device(void)
{
    printk("[exit] device halt\n");

    ata_stopHD(ATA_FORCE_STOP); /* we need to call halt_hd later to unmount all partitions */

    kudelay(100);
    clf();
    outw(0,0x30a1a);
    while(1) /* nothing */;
}

void reload_firmware(void)
{
    printk("[exit] firmware reload\n");

    arch_reload_firmware();
}
