/*
*   kernel/init/exit.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/io.h>
#include <kernel/hardware.h>
#include <kernel/kernel.h>
#include <kernel/irq.h>
#include <kernel/ata.h>
#include <kernel/delay.h>
#include <kernel/usb_fw.h>

#include <kernel/exit.h>

__IRAM_DATA int powering_off = 0;

extern int kusb_fw_status;

void halt_device(void)
{
    powering_off=1;
    printk("[exit] device halt\n");

    if(kusb_fw_status)
        disableUsbFw();
        
    arch_HaltMsg();
    
    ata_stopHD(ATA_FORCE_STOP); /* we need to call halt_hd later to unmount all partitions */

    udelay(100);
    clf();
    outw(0,0x30a1a);
    while(1) /* nothing */;
}

void reload_firmware(void)
{
    printk("[exit] firmware reload\n");

    arch_reload_firmware();
}
