#include <ata.h>
#include <uart.h>
#include <string.h>    

void showBuffer(char *source);
void identify();

//
//
//
    char data[512];

int main() {

    uartOuts("\nSelecting HDD...\n");
    ataSelectHDD();
    uartOuts("Making sure HDD has power...\n");
    ataPowerUpHDD();
    
    identify();

    uartOuts("\nSelecting Memory card...\n");
    ataSelectMemoryCard();

    identify();
    
    while(1) {}

}
    
void identify() {
    int c;
    int delay;
    do {
        c = ataWaitForReady();
        if (c!=0) {
            uartOuts("ATA Says not ready!\n");
            for (delay=0;delay<0x1000;delay++) {}
        }
    } while(c!=0);
        
        ataIdentify();
    
    do {
        c = ataWaitForXfer();
        if (c!=0) {
            uartOuts("ATA Says no xfer!\n");
            for (delay=0;delay<0x1000;delay++) {}
        }
    } while(c!=0);
        
    ataReadData(data, 256);
    
    showBuffer(data);
    
}

    char p[] = "xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx | xxxxxxxxxxxxxxxx\n";

void showBuffer(char *source) {
    int i,j;
    char c;

    for (j=0;j<512;j+=16) {
        for (i=0;i<16;i++) {
            c = source[i+j];
            stringPutHex(p+(i*3), c, 2);
            if (c<32 || c>126) c='.';
            p[50+i] = c;
        }
        uartOuts(p);
    }
}
