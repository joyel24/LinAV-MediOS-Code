#include <ata.h>
#include <uart.h>
#include <string.h>    
#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <fonts.h>

void showBuffer(char *source);
void identify();

//
//
//
    static int pal32[2] = {
        0x8080c0e0, 0xffffffff
    };
    static int pal16[2] = {
        0x0000, 0xffff
    };

    struct graphicsBuffer screenBitmap;
    char data[512];
    struct graphicsBuffer sprite4_6 = {0, 1, 4, 6, 1, 0, -1, 0, 0, 0, 0, &pal16, &pal32};
    struct graphicsBuffer sprite7_13 = {0, 1, 7, 13, 1, 0, 0, 0, 0, 0, 0, &pal16, &pal32};
    
    
int main() {
    int b;
    int source=0;
    
    osdInit();

    osdSetComponentConfig(OSD_VIDEO1, 0);
    osdSetComponentConfig(OSD_VIDEO2, 0);
    osdSetComponentConfig(OSD_BITMAP1, 0);
    osdSetComponentConfig(OSD_BITMAP2, 0);
    osdSetComponentConfig(OSD_CURSOR1, 0);
    osdSetComponentConfig(OSD_CURSOR2, 0);

    screenBitmap.offset = 0x03800000;
    screenBitmap.bytesPerLine = 320*2;
    screenBitmap.width = 320;
    screenBitmap.height = 240;
    screenBitmap.bitsPerPixelShift = 4;
    screenBitmap.bitsPerPixel = 16;

    graphicsBoxf(&screenBitmap, 0, 0, 320, 240, 0x0000);    
    
    osdSetComponentSize(OSD_BITMAP1, 320*2, 240);
    osdSetComponentPosition(OSD_BITMAP1, 0x14, 0x12);
    osdSetComponentOffset(OSD_BITMAP1, 0x03800000);
    osdSetComponentSourceWidth(OSD_BITMAP1, 0x14);
    osdSetComponentConfig(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT);


    while(1) {
        graphicsBoxf(&screenBitmap, 0, 0, 320, 240, 0x0000);    
        pal16[1] = 0x0202;
        graphicsString(&screenBitmap, 0, 0, &sprite4_6, &std4x6_, 5, 0,
                    "ATATest By DoGgEr");

        if (source==0) {
            graphicsString(&screenBitmap, 0, 8, &sprite4_6, &std4x6_, 5, 0,
                    "Selecting HDD...        ");
            uartOuts("\nSelecting HDD...\n");
            ataPowerUpHDD();
            ataSelectHDD();
        } else {
            graphicsString(&screenBitmap, 0, 8, &sprite4_6, &std4x6_, 5, 0,
                    "Selecting Memory card...        ");
            uartOuts("\nSelecting Memory card...\n");
            ataPowerDownHDD();
            ataSelectMemoryCard();
        }

        identify();

        do {
            b =buttonsGetStatus();
        } while(!(b & BUTTONS_AV300_ANY));

        if (b & BUTTONS_AV300_MENU1) source=0;
        if (b & BUTTONS_AV300_MENU2) source=1;
        
        do {
            b =buttonsGetStatus();
        } while((b & BUTTONS_AV300_ANY));    
    }
}

void identify() {
    int c;
    int delay;
    c = ataWaitForReady();
    if (c!=0) {
        pal16[1] = 0x0101;
        graphicsString(&screenBitmap, 0, 8, &sprite4_6, &std4x6_, 5, 0,
                "ATA Says not ready!        ");            
        uartOuts("ATA Says not ready!\n");
        for (delay=0;delay<0x1000;delay++) {}
        return;
    }
        
    ataIdentify();
    
    c = ataWaitForXfer();
    if (c!=0) {
        pal16[1] = 0x0101;
        graphicsString(&screenBitmap, 0, 8, &sprite4_6, &std4x6_, 5, 0,
                "ATA Says no xfer!        ");            
        uartOuts("ATA Says no xfer!\n");
        for (delay=0;delay<0x1000;delay++) {}
        return;
    }
        
    ataReadData(data, 256);
    showBuffer(data);
}

    char p[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx | xxxxxxxxxxxxxxxx\n";

void showBuffer(char *source) {
    int i,j,y=0;
    char c;

    for (j=0;j<512;j+=16) {
        for (i=0;i<16;i++) {
            c = source[i+j];
            stringPutHex(p+(i*2), c, 2);
            if (c<32 || c>126) c='.';
            p[35+i] = c;
        }
        uartOuts(p);

        pal16[1] = 0xffff;
        graphicsString(&screenBitmap, 0, 16 + y*7, &sprite4_6, &std4x6_, 5, 0,
                    p);
        y++;
    }
}
