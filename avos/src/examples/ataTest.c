#include <ata.h>
#include <uart.h>
#include <string.h>    

void showBuffer(char *source);

//
//
//
    char data[512];

int main() {
    int c;

    ataSelectHDD();
    ataPowerUpHDD();

    showBuffer(data);
    
    while(1) {}
    
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
