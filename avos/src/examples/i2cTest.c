#include <i2c.h>
#include <uart.h>
#include <string.h>    
#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <fonts.h>

void showBuffer(char *source);
void docmd();

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
    struct graphicsBuffer spriteShadow = {0, 2, 12, 18, 1, 0, -1, 0, 0, 0, 0, &pal16, &pal32};
    int addr= 0xd0;             // 0x3c, 0x90, 0xd0
    char dispAddr[] = "xx";
    
int main() {
    int b;

    osdInit();

//    i2cInit();
    
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

    pal16[1] = 0x0202;
    graphicsString(&screenBitmap, 0, 0, &sprite4_6, &std4x6_, 5, 0,
                "I2CTest By DoGgEr");
    while(1) {
        pal16[1] = 0x0202;                    
        stringPutHex(dispAddr, addr, 2);

        graphicsString(&screenBitmap, 160, 0, &sprite4_6, &std4x6_, 5, 0,
                    dispAddr);

        docmd();

        b =buttonsGetStatus();

        if (b & BUTTONS_AV300_RIGHT) {
            addr+=2;
            do {
                b = buttonsGetStatus();
            } while(b & BUTTONS_AV300_RIGHT);
        } else if (b & BUTTONS_AV300_LEFT) {
            addr-=2;
            do {
                b = buttonsGetStatus();
            } while(b & BUTTONS_AV300_LEFT);
        }
        
    }
}

    char timeSt[] = "xx:xx:xx.xx";

void docmd() {
    int c;
    
    for (c=0;c<512;c++)
        data[c] = 0;
    
    uartOuts("Getting data...\n");

    c = i2cRead(addr, 0, data, 64);

    if (addr==0xd0) {
        data[0x0c] &= ~0x40;
        i2cWrite(addr, 0x0c, data+0x0c, 1);
        stringPutHex(timeSt+9, data[0], 2);
        stringPutHex(timeSt+6, data[1], 2);
        stringPutHex(timeSt+3, data[2], 2);
        stringPutHex(timeSt, data[3], 2);
        pal16[1] = 0x0202;
        graphicsString(&screenBitmap, 160-(5*14)-7, 120, &spriteShadow, &shadow_, 14, 0,
                    timeSt);
    }
    
    if (c==0) uartOuts("Returned 0\n");

    showBuffer(data);
}

    char p[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx | xxxxxxxxxxxxxxxx\n";

void showBuffer(char *source) {
    int i,j,y=0;
    char c;

    for (j=0;j<64;j+=16) {
        for (i=0;i<16;i++) {
            c = source[i+j];
            stringPutHex(p+(i*2), c, 2);
            if (c<32 || c>126) c='.';
            p[35+i] = c;
        }
//        uartOuts(p);

        pal16[1] = 0xffff;
        graphicsString(&screenBitmap, 4, 16 + y*7, &sprite4_6, &std4x6_, 6, 0,
                    p);
        y++;
    }
}
