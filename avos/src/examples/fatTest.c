#include <ata.h>
#include <fat.h>
#include <uart.h>
#include <string.h> 

void showBuffer(char *source);

char buff[65535];

struct dirEntry dirBuffer[1000];

char hex82[] = "xxxxxxxx";

char dirLine[] = "xxxxxxxx.xxx x\n";

int main() {
    int c, i;
    ataPowerUpHDD();
    ataSelectHDD();

    c = fatInit(0x3f);
    stringPutHex(hex82, c, 8);
    uartOuts("[fatTest.c] fatInit returned = ");
    uartOuts(hex82);
    uartOuts("\n");
    
    c = fatReadFile(0, (char*) dirBuffer);
    stringPutHex(hex82, c, 8);
    uartOuts("[fatTest.c] fatReadFile returned = ");
    uartOuts(hex82);
    uartOuts("\n");

    showBuffer((char*) dirBuffer);
    
    for (c=0;c<1000;c++) {
        if (dirBuffer[c].name[0]==0) {
            break;    
        } else if (dirBuffer[c].name[0]!=0xe5) {
            if (!(dirBuffer[c].attr & FAT_ATTR_VOLUME_ID) &&
               (dirBuffer[c].attr!= FAT_ATTR_LONG_NAME)) {
                for (i=0;i<8;i++) dirLine[i] = dirBuffer[c].name[i];
                for (i=0;i<3;i++) dirLine[i+9] = dirBuffer[c].ext[i];
                uartOuts(dirLine);
            }
        }
    }

    uartOuts("All done!");
    
    while(1) {}
}

char p[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx | xxxxxxxxxxxxxxxx\n";

void showBuffer(char *source) {
    int i,j;
    char c;

    for (j=0;j<512*10;j+=16) {
        for (i=0;i<16;i++) {
            c = source[i+j];
            stringPutHex(p+(i*2), c, 2);
            if (c<32 || c>126) c='.';
            p[35+i] = c;
        }
        uartOuts(p);
    }
}
