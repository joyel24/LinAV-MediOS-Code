#include <ata.h>
#include <uart.h>
#include <string.h>    
#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <fonts.h>
#include <usb.h>
#include <fat.h>
#include <dir.h>


DIR *d;
struct dirent * dent;
char * dirname = "/";

    
int errno;

int main() {
    int c;
    ataPowerUpHDD();
    ataSelectHDD();
    uartOuts("About to call FAT routine...\n");
    
    c = fat_mount(0x3f);
    
    uartOuts("Done!\n");
    
    while(1) {}
}

