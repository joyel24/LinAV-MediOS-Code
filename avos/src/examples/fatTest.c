#include <ata.h>
#include <uart.h>
#include <string.h>    
#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <fonts.h>
#include <usb.h>
#include <fat.h>

    
int main() {
    ataPowerUpHDD();
    ataSelectHDD();
    uartOuts("About to call FAT routine...\n");
    
    fat_mount(0x3f);
    
    uartOuts("Back!\n");
    
    while(1) {}
}

