#include <ata.h>
#include <uart.h>
#include <string.h>    
#include <graphics.h>
#include <osdDSC25.h>
#include <buttons.h>
#include <fonts.h>
#include <usb.h>
#include <system.h>

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
    struct graphicsBuffer sprite4_6 = {0, 1, 4, 6, 1, 0, -1, 0, 0, 0, 0, (int**) &pal16, (int**) &pal32};
    int mode=0;
    int lba=0;
    char showLba[] = "LBA: xxxxxxxx";

int main() {
    int b;
    int source=0;
    int usbs=0;
    
    osdInitA();

    osdSetComponentConfigA(OSD_VIDEO1, 0);
    osdSetComponentConfigA(OSD_VIDEO2, 0);
    osdSetComponentConfigA(OSD_BITMAP1, 0);
    osdSetComponentConfigA(OSD_BITMAP2, 0);
    osdSetComponentConfigA(OSD_CURSOR1, 0);
    osdSetComponentConfigA(OSD_CURSOR2, 0);

    screenBitmap.offset = 0x03800000;
    screenBitmap.bytesPerLine = 320*2;
    screenBitmap.width = 320;
    screenBitmap.height = 240;
    screenBitmap.bitsPerPixelShift = 4;
    screenBitmap.bitsPerPixel = 16;

    graphicsBoxfA(&screenBitmap, 0, 0, 320, 240, 0x0000);
    
    osdSetComponentSizeA(OSD_BITMAP1, 320*2, 240);
    osdSetComponentPositionA(OSD_BITMAP1, 0x14, 0x12);
    osdSetComponentOffsetA(OSD_BITMAP1, 0x03800000);
    osdSetComponentSourceWidthA(OSD_BITMAP1, 0x14);
    osdSetComponentConfigA(OSD_BITMAP1, OSD_COMPONENT_ENABLE
                                     | OSD_BITMAP_8BIT);


    graphicsBoxfA(&screenBitmap, 0, 0, 320, 240, 0x0000);    

    while(1) {
        pal16[1] = 0x0202;
        graphicsStringA(&screenBitmap, 0, 0, &sprite4_6, std4x6_, 5, 0,
                    "ATATest By DoGgEr");

        if (source==0) {
            graphicsStringA(&screenBitmap, 212, 0, &sprite4_6, std4x6_, 5, 0,
                    "F2[HDD]");
            graphicsStringA(&screenBitmap, 0, 8, &sprite4_6, std4x6_, 5, 0,
                    "Power HDD...        ");
            uartOutsA("\nSelecting HDD...\n");
            ataPowerUpHDDA();
            graphicsStringA(&screenBitmap, 0, 8, &sprite4_6, std4x6_, 5, 0,
                    "Selecting HDD...        ");
            ataSelectHDDA();
        } else if (source==1) {
            graphicsStringA(&screenBitmap, 212, 0, &sprite4_6, std4x6_, 5, 0,
                    "F2[MemCard]");
            graphicsStringA(&screenBitmap, 0, 8, &sprite4_6, std4x6_, 5, 0,
                    "Selecting MemCard...        ");
            uartOutsA("\nSelecting Memory card...\n");
            ataPowerDownHDDA();
            ataSelectA(source);
        } else {
            graphicsStringA(&screenBitmap, 212, 0, &sprite4_6, std4x6_, 5, 0,
                    "F2[Other]");
            graphicsStringA(&screenBitmap, 0, 8, &sprite4_6, std4x6_, 5, 0,
                    "Selecting Other...        ");
            uartOutsA("\nSelecting Other...\n");
            ataPowerDownHDDA();
            ataSelectA(source);
        }

        stringPutHexA(showLba + 5, lba, 8);
        graphicsStringA(&screenBitmap, 212, 8, &sprite4_6, std4x6_, 5, 0,
                    showLba);
        
        docmd();

        do {
            b =buttonsGetStatusA();
        } while(!(b & BUTTONS_AV300_ANY));

        if (b & BUTTONS_AV300_MENU2) {
            if (source==0) source=1;
            else if(source==1) source=0;
        }
        if (b & BUTTONS_AV300_MENU1) mode=!mode;

        source &= 7;

        if (b & BUTTONS_AV300_RIGHT) lba++;
        if (b & BUTTONS_AV300_LEFT) lba--;
        if (b & BUTTONS_AV300_DOWN) lba+=0x100;
        if (b & BUTTONS_AV300_UP) lba-=0x100;
        if (b & BUTTONS_AV300_MENU3) {
            usbs = !usbs;
            if (usbs) {
                usbEnableA();
            } else {
                usbDisableA();
            }
        }
        if (b & BUTTONS_AV300_OFF) return;
        
        do {
            b =buttonsGetStatusA();
        } while((b & BUTTONS_AV300_ANY));    
    }
}

void docmd() {
    int c=0;
    int delay=0;

    pal16[1] = 0x0101;
    if (mode==0) {
        graphicsStringA(&screenBitmap, 108, 0, &sprite4_6, std4x6_, 5, 0,
                    "F1[Identify]");
        c = ataIdentifyDeviceA(data);
    } else {
        graphicsStringA(&screenBitmap, 108, 0, &sprite4_6, std4x6_, 5, 0,
                    "F1[Read MBR]");
        c = ataReadSectorsA(lba, 1, data);    
    }
    
    if (c!=0) {
        pal16[1] = 0x0101;
        graphicsStringA(&screenBitmap, 0, 8, &sprite4_6, std4x6_, 5, 0,
                "ATA Error!        ");            
        uartOutsA("ATA Error!\n");
        for (delay=0;delay<0x1000;delay++) {}
        return;
    }
        
    showBuffer(data);
}

    char p[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx | xxxxxxxxxxxxxxxx\n";

void showBuffer(char *source) {
    int i,j,y=0;
    char c;

    for (j=0;j<512;j+=16) {
        for (i=0;i<16;i++) {
            c = source[i+j];
            stringPutHexA(p+(i*2), c, 2);
            if (c<32 || c>126) c='.';
            p[35+i] = c;
        }
        uartOutsA(p);

        pal16[1] = 0xffff;
        graphicsStringA(&screenBitmap, 4, 16 + y*7, &sprite4_6, std4x6_, 6, 0,
                    p);
        y++;
    }
}
