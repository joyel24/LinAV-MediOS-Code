/* 
*   kernel/halt_device.c
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
    printk("let's halt the device\n");    
    
    ata_stop_HD(); /* we need to call halt_hd later to unmount all partitions */

    udelay(100);
    clf();
    while(1)
        outw(0,0x30a1a);
}
