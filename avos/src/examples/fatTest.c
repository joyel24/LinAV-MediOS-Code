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
    
    if (c==0) uartOuts("returned 0\n");
    
    d = opendir(dirname);
    
    if (d>0) {
        uartOuts("OK,\n");
    
        while(1) {
            dent = readdir(d);
            if (dent==0) break;
            
            uartOuts("> ");
            uartOuts(dent->d_name);
            uartOuts("\n");
        }
        closedir(d);
    
    }
    
    uartOuts("Back!\n");
    
    while(1) {}
}

