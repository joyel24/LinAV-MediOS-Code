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


    graphicsBoxf(&screenBitmap, 0, 0, 320, 240, 0x0000);    

    while(1) {
        pal16[1] = 0x0202;
        graphicsString(&screenBitmap, 0, 0, &sprite4_6, std4x6_, 5, 0,
                    "ATATest By DoGgEr");

        if (source==0) {
            graphicsString(&screenBitmap, 212, 0, &sprite4_6, std4x6_, 5, 0,
                    "F2[HDD]");
            graphicsString(&screenBitmap, 0, 8, &sprite4_6, std4x6_, 5, 0,
                    "Power HDD...        ");
            uartOuts("\nSelecting HDD...\n");
            ataPowerUpHDD();
            graphicsString(&screenBitmap, 0, 8, &sprite4_6, std4x6_, 5, 0,
                    "Selecting HDD...        ");
            ataSelectHDD();
        } else if (source==1) {
            graphicsString(&screenBitmap, 212, 0, &sprite4_6, std4x6_, 5, 0,
                    "F2[MemCard]");
            graphicsString(&screenBitmap, 0, 8, &sprite4_6, std4x6_, 5, 0,
                    "Selecting MemCard...        ");
            uartOuts("\nSelecting Memory card...\n");
            ataPowerDownHDD();
            ataSelect(source);
        } else {
            graphicsString(&screenBitmap, 212, 0, &sprite4_6, std4x6_, 5, 0,
                    "F2[Other]");
            graphicsString(&screenBitmap, 0, 8, &sprite4_6, std4x6_, 5, 0,
                    "Selecting Other...        ");
            uartOuts("\nSelecting Other...\n");
            ataPowerDownHDD();
            ataSelect(source);
        }

        stringPutHex(showLba + 5, lba, 8);
        graphicsString(&screenBitmap, 212, 8, &sprite4_6, std4x6_, 5, 0,
                    showLba);
        
        docmd();

        do {
            b =buttonsGetStatus();
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
                usbEnable();
            } else {
                usbDisable();
            }
        }
        
        do {
            b =buttonsGetStatus();
        } while((b & BUTTONS_AV300_ANY));    
    }
}

void docmd() {
    int c=0;
    int delay=0;

    pal16[1] = 0x0101;
    if (mode==0) {
        graphicsString(&screenBitmap, 108, 0, &sprite4_6, std4x6_, 5, 0,
                    "F1[Identify]");
        c = ataIdentifyDevice(data);
    } else {
        graphicsString(&screenBitmap, 108, 0, &sprite4_6, std4x6_, 5, 0,
                    "F1[Read MBR]");
        c = ataReadSectors(lba, 1, data);    
    }
    
    if (c!=0) {
        pal16[1] = 0x0101;
        graphicsString(&screenBitmap, 0, 8, &sprite4_6, std4x6_, 5, 0,
                "ATA Error!        ");            
        uartOuts("ATA Error!\n");
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
            stringPutHex(p+(i*2), c, 2);
            if (c<32 || c>126) c='.';
            p[35+i] = c;
        }
        uartOuts(p);

        pal16[1] = 0xffff;
        graphicsString(&screenBitmap, 4, 16 + y*7, &sprite4_6, std4x6_, 6, 0,
                    p);
        y++;
    }
}
